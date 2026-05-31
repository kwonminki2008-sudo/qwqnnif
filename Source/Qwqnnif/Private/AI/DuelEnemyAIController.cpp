#include "AI/DuelEnemyAIController.h"

#include "Characters/DuelCharacterBase.h"
#include "Kismet/GameplayStatics.h"

ADuelEnemyAIController::ADuelEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADuelEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledEnemy = Cast<ADuelCharacterBase>(InPawn);
}

void ADuelEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!ControlledEnemy || !ControlledEnemy->IsAlive())
	{
		StopMovement();
		ClearFocus(EAIFocusPriority::Gameplay);
		return;
	}

	APawn* TargetPawn = FindTargetPawn();
	if (!TargetPawn)
	{
		StopMovement();
		ClearFocus(EAIFocusPriority::Gameplay);
		return;
	}

	const float DistanceToTarget = FVector::Dist(ControlledEnemy->GetActorLocation(), TargetPawn->GetActorLocation());
	if (DistanceToTarget > DetectionRadius)
	{
		StopMovement();
		ClearFocus(EAIFocusPriority::Gameplay);
		return;
	}

	SetFocus(TargetPawn);

	const float AttackDistance = ControlledEnemy->GetAttackRange() + AttackAcceptanceRadius;
	if (DistanceToTarget <= AttackDistance)
	{
		StopMovement();

		const FVector ToTarget = TargetPawn->GetActorLocation() - ControlledEnemy->GetActorLocation();
		ControlledEnemy->SetActorRotation(FRotator(0.0f, ToTarget.Rotation().Yaw, 0.0f));
		ControlledEnemy->PerformAttack();
		return;
	}

	StopMovement();

	const FVector ToTarget = TargetPawn->GetActorLocation() - ControlledEnemy->GetActorLocation();
	const FVector DirectionToTarget = FVector(ToTarget.X, ToTarget.Y, 0.0f).GetSafeNormal();
	ControlledEnemy->AddMovementInput(DirectionToTarget, 1.0f);
}

APawn* ADuelEnemyAIController::FindTargetPawn() const
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	const ADuelCharacterBase* PlayerDuelCharacter = Cast<ADuelCharacterBase>(PlayerPawn);

	if (!PlayerPawn || (PlayerDuelCharacter && !PlayerDuelCharacter->IsAlive()))
	{
		return nullptr;
	}

	return PlayerPawn;
}
