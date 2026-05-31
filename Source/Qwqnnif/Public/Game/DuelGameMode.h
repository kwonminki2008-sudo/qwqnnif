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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Duel|Arena")
	bool bSpawnDefaultArena = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Duel|Arena")
	FVector ArenaFloorLocation = FVector(0.0f, 0.0f, -100.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Duel|Arena")
	FVector ArenaFloorScale = FVector(30.0f, 30.0f, 0.2f);

private:
	void SpawnDefaultArenaIfNeeded();
	void SpawnEnemyIfNeeded();
	void RegisterCombatant(AActor* Combatant);

	UFUNCTION()
	void HandleCombatantDeath(AActor* DeadActor);
};
