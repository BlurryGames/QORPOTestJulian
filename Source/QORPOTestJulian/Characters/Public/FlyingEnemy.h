#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"

#include "FlyingEnemy.generated.h"

/**
 * AFlyingEnemy
 *
 * Abstract base class for flying enemy pawns.
 * Inherits from ABaseEnemy and overrides the target update logic to provide custom movement behavior for flying enemies.
 *
 * This class is intended to be extended for specific flying enemy types and supports Blueprint extension.
 * The main difference from the base enemy is the way it handles movement towards its target, using direct path movement.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API AFlyingEnemy : public ABaseEnemy
{
	GENERATED_BODY()
	
protected:
	/**
	 * Updates the current target for the flying enemy and moves towards it.
	 *
	 * This implementation selects the closest valid target from the Targets array.
	 * If a valid target is found, the enemy requests movement in the direction of the target using the floating movement component.
	 * This function is called every tick and can be overridden in Blueprints for custom behavior.
	 */
	virtual void OnUpdateTarget_Implementation() override;
};
