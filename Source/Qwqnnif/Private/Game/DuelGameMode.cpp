#include "Game/DuelGameMode.h"

#include "Characters/DuelCharacterBase.h"
#include "Characters/DuelEnemyCharacter.h"
#include "Characters/DuelPlayerCharacter.h"
#include "Combat/HealthComponent.h"
#include "Components/LightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/Engine.h"
#include "Engine/SkyLight.h"
#include "Engine/StaticMeshActor.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

ADuelGameMode::ADuelGameMode()
{
	DefaultPawnClass = ADuelPlayerCharacter::StaticClass();
	EnemyClass = ADuelEnemyCharacter::StaticClass();
}

void ADuelGameMode::StartPlay()
{
	Super::StartPlay();

	SpawnDefaultArenaIfNeeded();
	RegisterCombatant(UGameplayStatics::GetPlayerPawn(this, 0));
	SpawnEnemyIfNeeded();
}

void ADuelGameMode::SpawnDefaultArenaIfNeeded()
{
	UWorld* World = GetWorld();
	if (!World || !bSpawnDefaultArena)
	{
		return;
	}

	for (TActorIterator<AStaticMeshActor> MeshIt(World); MeshIt; ++MeshIt)
	{
		if (MeshIt->GetStaticMeshComponent() && MeshIt->GetStaticMeshComponent()->GetStaticMesh())
		{
			return;
		}
	}

	UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh)
	{
		AStaticMeshActor* Floor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), ArenaFloorLocation, FRotator::ZeroRotator);
		if (Floor && Floor->GetStaticMeshComponent())
		{
			Floor->SetActorScale3D(ArenaFloorScale);
			Floor->GetStaticMeshComponent()->SetStaticMesh(CubeMesh);
			Floor->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("BlockAll"));
		}
	}

	ADirectionalLight* DirectionalLight = World->SpawnActor<ADirectionalLight>(
		ADirectionalLight::StaticClass(),
		FVector(0.0f, 0.0f, 700.0f),
		FRotator(-45.0f, -35.0f, 0.0f));
	if (DirectionalLight && DirectionalLight->GetLightComponent())
	{
		DirectionalLight->GetLightComponent()->SetIntensity(4.0f);
	}

	ASkyLight* SkyLight = World->SpawnActor<ASkyLight>(ASkyLight::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	if (SkyLight && SkyLight->GetLightComponent())
	{
		SkyLight->GetLightComponent()->SetIntensity(1.0f);
	}
}

void ADuelGameMode::SpawnEnemyIfNeeded()
{
	UWorld* World = GetWorld();
	if (!World || !EnemyClass)
	{
		return;
	}

	for (TActorIterator<ADuelEnemyCharacter> EnemyIt(World); EnemyIt; ++EnemyIt)
	{
		if (EnemyIt->IsAlive())
		{
			RegisterCombatant(*EnemyIt);
			return;
		}
	}

	const APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	const FVector PlayerLocation = PlayerPawn ? PlayerPawn->GetActorLocation() : FVector::ZeroVector;
	const FRotator PlayerRotation = PlayerPawn ? PlayerPawn->GetActorRotation() : FRotator::ZeroRotator;
	const FVector SpawnLocation = PlayerLocation + PlayerRotation.RotateVector(EnemySpawnOffset);
	const FRotator SpawnRotation = PlayerPawn ? (PlayerLocation - SpawnLocation).Rotation() : FRotator::ZeroRotator;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ADuelEnemyCharacter* SpawnedEnemy = World->SpawnActor<ADuelEnemyCharacter>(EnemyClass, SpawnLocation, SpawnRotation, SpawnParams);
	RegisterCombatant(SpawnedEnemy);
}

void ADuelGameMode::RegisterCombatant(AActor* Combatant)
{
	ADuelCharacterBase* DuelCharacter = Cast<ADuelCharacterBase>(Combatant);
	if (!DuelCharacter || !DuelCharacter->GetHealthComponent())
	{
		return;
	}

	DuelCharacter->GetHealthComponent()->OnDeath.AddUniqueDynamic(this, &ADuelGameMode::HandleCombatantDeath);
}

void ADuelGameMode::HandleCombatantDeath(AActor* DeadActor)
{
	if (!GEngine || !DeadActor)
	{
		return;
	}

	const bool bPlayerDied = DeadActor == UGameplayStatics::GetPlayerPawn(this, 0);
	const FString ResultMessage = bPlayerDied
		? TEXT("Defeat! The enemy won the duel.")
		: TEXT("Victory! The enemy was defeated.");

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, bPlayerDied ? FColor::Red : FColor::Green, ResultMessage);
}
