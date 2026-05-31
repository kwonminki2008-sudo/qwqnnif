#pragma once

#include "CoreMinimal.h"
#include "Characters/DuelCharacterBase.h"
#include "DuelPlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class QWQNNIF_API ADuelPlayerCharacter : public ADuelCharacterBase
{
	GENERATED_BODY()

public:
	ADuelPlayerCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void StartAttack();
};
