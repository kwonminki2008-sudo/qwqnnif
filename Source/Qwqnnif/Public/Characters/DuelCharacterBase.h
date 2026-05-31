#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DuelCharacterBase.generated.h"

class UDamageType;
class UHealthComponent;
class UStaticMeshComponent;

UCLASS(Abstract)
class QWQNNIF_API ADuelCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ADuelCharacterBase();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool PerformAttack();

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsAlive() const;

	UFUNCTION(BlueprintPure, Category = "Combat")
	float GetAttackRange() const { return AttackRange; }

	UFUNCTION(BlueprintPure, Category = "Combat")
	float GetAttackCooldown() const { return AttackCooldown; }

	UFUNCTION(BlueprintPure, Category = "Combat")
	UHealthComponent* GetHealthComponent() const { return HealthComponent; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UHealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0.0"))
	float AttackDamage = 25.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0.0"))
	float AttackRange = 180.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "1.0"))
	float AttackRadius = 70.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0.0"))
	float AttackCooldown = 0.8f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	UFUNCTION()
	virtual void HandleDeath(AActor* DeadActor);

	virtual bool CanAttack() const;

private:
	float LastAttackTime = -1000.0f;
};
