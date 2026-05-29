#include "Characters/DuelEnemyCharacter.h"

#include "AI/DuelEnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

ADuelEnemyCharacter::ADuelEnemyCharacter()
{
	AIControllerClass = ADuelEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCharacterMovement()->MaxWalkSpeed = 430.0f;
}
