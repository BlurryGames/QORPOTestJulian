#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "ReusableInterface.generated.h"

/**
 * UReusableInterface
 *
 * Unreal Engine interface class for reusable and resettable actors.
 * Allows actors to implement logic for resetting, enabling/disabling, and managing their original state.
 * Designed for use with items, interactables, and any actor that needs to be toggled or respawned in the game world.
 */
UINTERFACE(Blueprintable, BlueprintType)
class UReusableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IReusableInterface
 *
 * Interface for actors that can be reused, reset, or toggled between active and inactive states.
 * Provides methods for managing original position/rotation, enabling/disabling, collision handling, and networked damage.
 * Intended to be implemented by actors that require respawn, reset, or reusable logic.
 */
class QORPOTESTJULIAN_API IReusableInterface
{
	GENERATED_BODY()

public:
	/**
	 * Returns the original world position of the actor.
	 * @return The original position as an FVector.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status")
	FVector GetOriginalPosition();

	/**
	 * Returns the original world rotation of the actor.
	 * @return The original rotation as an FRotator.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status")
	FRotator GetOriginalRotation();

	/**
	 * Sets the original world position and rotation for the actor.
	 * Used to reset the actor to its initial state.
	 * @param Position The position to set as original.
	 * @param Rotation The rotation to set as original.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status")
	void SetOriginalPositionAndRotation(FVector Position, FRotator Rotation);

	/**
	 * Sets the original world position for the actor.
	 * @param Position The position to set as original.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status")
	void SetOriginalPosition(FVector Position);

	/**
	 * Sets the original world rotation for the actor.
	 * @param Rotation The rotation to set as original.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status")
	void SetOriginalRotation(FRotator Rotation);

	/**
	 * Enables or disables the actor and its components.
	 * When enabled, resets the actor's position and rotation, shows the actor, enables ticking and collision.
	 * When disabled, hides the actor, disables ticking and collision, and updates all registered primitive components.
	 * @param bEnabled Whether the actor should be enabled.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status")
	void OnTurnEnabled(const bool bEnabled = true);

protected:
	/** Map of registered primitive components and their original collision states. */
	TMap<UPrimitiveComponent*, TEnumAsByte<ECollisionEnabled::Type>> CollisionEnabledTypes = TMap<UPrimitiveComponent*, TEnumAsByte<ECollisionEnabled::Type>>();
	
	/** The original world position of the actor. */
	FVector OriginalPosition = FVector::ZeroVector;

	/** The original world rotation of the actor. */
	FRotator OriginalRotation = FRotator::ZeroRotator;

	/** Whether the actor is currently enabled/active. */
	bool bEnableStatus = true;

	/**
	 * Registers a primitive component for collision and visibility management.
	 * Stores its original collision state for later restoration.
	 * @param PrimitiveComponent The component to register.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status")
	void AddEnabledType(UPrimitiveComponent* PrimitiveComponent);

	/**
	 * Applies damage to a target actor using the networked damage system.
	 * Calls the server-side damage function on the instigating player controller if available.
	 * @param DamageReceiver The actor receiving damage.
	 * @param DamageAmount The amount of damage to apply.
	 * @param DamageEvent The damage event struct.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void DoDamage(AActor* DamageReceiver, float DamageAmount, FDamageEvent const& DamageEvent);

	/** Default C++ implementation for GetOriginalPosition. */
	virtual FVector GetOriginalPosition_Implementation();

	/** Default C++ implementation for GetOriginalRotation. */
	virtual FRotator GetOriginalRotation_Implementation();

	/** Default C++ implementation for SetOriginalPositionAndRotation. */
	virtual void SetOriginalPositionAndRotation_Implementation(FVector Position, FRotator Rotation);

	/** Default C++ implementation for SetOriginalPosition. */
	virtual void SetOriginalPosition_Implementation(FVector Position);

	/** Default C++ implementation for SetOriginalRotation. */
	virtual void SetOriginalRotation_Implementation(FRotator Rotation);

	/** Default C++ implementation for OnTurnEnabled. */
	virtual void OnTurnEnabled_Implementation(const bool bEnabled = true);

	/** Default C++ implementation for AddEnabledType. */
	virtual void AddEnabledType_Implementation(UPrimitiveComponent* PrimitiveComponent);

	/** Default C++ implementation for DoDamage. */
	virtual void DoDamage_Implementation(AActor* DamageReceiver, float DamageAmount, FDamageEvent const& DamageEvent);
};
