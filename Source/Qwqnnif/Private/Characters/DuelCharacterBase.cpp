#include "Characters/DuelCharacterBase.h"

#include "Combat/HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ADuelCharacterBase::ADuelCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(RootComponent);
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -20.0f));
	BodyMesh->SetRelativeScale3D(FVector(0.75f, 0.75f, 1.4f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyMeshAsset(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (BodyMeshAsset.Succeeded())
	{
		BodyMesh->SetStaticMesh(BodyMeshAsset.Object);
	}
}

void ADuelCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &ADuelCharacterBase::HandleDeath);
	}
}

bool ADuelCharacterBase::PerformAttack()
{
	if (!CanAttack())
	{
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	LastAttackTime = World->GetTimeSeconds();

	const FVector Start = GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);
	const FVector End = Start + GetActorForwardVector() * AttackRange;
	const FCollisionShape AttackShape = FCollisionShape::MakeSphere(AttackRadius);

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(DuelAttack), false, this);
	QueryParams.AddIgnoredActor(this);

	TArray<FHitResult> Hits;
	World->SweepMultiByChannel(Hits, Start, End, FQuat::Identity, ECC_Pawn, AttackShape, QueryParams);

	for (const FHitResult& Hit : Hits)
	{
		ADuelCharacterBase* Target = Cast<ADuelCharacterBase>(Hit.GetActor());
		if (!Target || Target == this || !Target->IsAlive())
		{
			continue;
		}

		TSubclassOf<UDamageType> ResolvedDamageType = DamageTypeClass;
		if (!ResolvedDamageType)
		{
			ResolvedDamageType = UDamageType::StaticClass();
		}
		UGameplayStatics::ApplyDamage(
			Target,
			AttackDamage,
			GetController(),
			this,
			ResolvedDamageType);
		return true;
	}

	return false;
}

bool ADuelCharacterBase::IsAlive() const
{
	return HealthComponent && !HealthComponent->IsDead();
}

void ADuelCharacterBase::HandleDeath(AActor* DeadActor)
{
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DetachFromControllerPendingDestroy();
	SetLifeSpan(5.0f);
}

bool ADuelCharacterBase::CanAttack() const
{
	const UWorld* World = GetWorld();
	return World && IsAlive() && World->GetTimeSeconds() - LastAttackTime >= AttackCooldown;
}
