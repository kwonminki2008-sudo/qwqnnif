#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DuelEnemyAIController.generated.h"

class ADuelCharacterBase;

UCLASS()
class QWQNNIF_API ADuelEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	ADuelEnemyAIController();

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI", meta = (ClampMin = "0.0"))
	float DetectionRadius = 2500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI", meta = (ClampMin = "0.0"))
	float AttackAcceptanceRadius = 80.0f;

private:
	UPROPERTY()
	TObjectPtr<ADuelCharacterBase> ControlledEnemy;

	APawn* FindTargetPawn() const;
};
