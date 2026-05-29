#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DuelGameMode.generated.h"

class ADuelEnemyCharacter;

UCLASS()
class QWQNNIF_API ADuelGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADuelGameMode();

	virtual void StartPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Duel")
	TSubclassOf<ADuelEnemyCharacter> EnemyClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Duel")
	FVector EnemySpawnOffset = FVector(700.0f, 0.0f, 100.0f);

private:
	void SpawnEnemyIfNeeded();
	void RegisterCombatant(AActor* Combatant);

	UFUNCTION()
	void HandleCombatantDeath(AActor* DeadActor);
};
