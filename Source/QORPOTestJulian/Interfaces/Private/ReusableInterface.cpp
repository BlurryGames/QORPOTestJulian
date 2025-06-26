// Copyright (c) Julián López Barañano. All Rights Reserved.

/**
 * @file ReusableInterface.cpp
 * @brief Implements the default logic for the IReusableInterface, which defines reusable and resettable behavior for actors.
 *
 * This file provides the default C++ implementations for the reusable interface methods, including position/rotation management,
 * enabling/disabling actors, collision handling, and networked damage application. By default, these methods provide reusable
 * item logic and can be extended or overridden in derived classes. Designed for actors that need to be reset, respawned, or toggled
 * between active and inactive states in the game world.
 */

#include "../Public/ReusableInterface.h"
#include "../../Core/Public/ShooterPlayerController.h"

/**
 * Returns the original world position of the actor.
 * @return The original position as an FVector.
 */
FVector IReusableInterface::GetOriginalPosition_Implementation()
{
	return OriginalPosition;
}

/**
 * Returns the original world rotation of the actor.
 * @return The original rotation as an FRotator.
 */
FRotator IReusableInterface::GetOriginalRotation_Implementation()
{
	return OriginalRotation;
}

/**
 * Sets the original world position and rotation for the actor.
 * Used to reset the actor to its initial state.
 * @param Position The position to set as original.
 * @param Rotation The rotation to set as original.
 */
void IReusableInterface::SetOriginalPositionAndRotation_Implementation(FVector Position, FRotator Rotation)
{
	if (OriginalPosition != Position)
	{
		OriginalPosition = Position;
	}

	if (OriginalRotation != Rotation)
	{
		OriginalRotation = Rotation;
	}
}

/**
 * Sets the original world position for the actor.
 * @param Position The position to set as original.
 */
void IReusableInterface::SetOriginalPosition_Implementation(FVector Position)
{
	if (OriginalPosition != Position)
	{
		OriginalPosition = Position;
	}
}

/**
 * Sets the original world rotation for the actor.
 * @param Rotation The rotation to set as original.
 */
void IReusableInterface::SetOriginalRotation_Implementation(FRotator Rotation)
{
	if (OriginalRotation != Rotation)
	{
		OriginalRotation = Rotation;
	}
}

/**
 * Enables or disables the actor and its components.
 * When enabled, resets the actor's position and rotation, shows the actor, enables ticking and collision.
 * When disabled, hides the actor, disables ticking and collision, and updates all registered primitive components.
 * @param bEnabled Whether the actor should be enabled.
 */
void IReusableInterface::OnTurnEnabled_Implementation(const bool bEnabled)
{
	bEnableStatus = bEnabled;
	AActor* Self = Cast<AActor>(this);
	if (IsValid(Self))
	{
		if (bEnabled)
		{
			Self->SetActorLocation(OriginalPosition);
			Self->SetActorRotation(OriginalRotation);
		}

		Self->SetHidden(!bEnabled);
		Self->SetActorTickEnabled(bEnabled);
		Self->SetActorEnableCollision(bEnabled);
	}
	
	TArray<UPrimitiveComponent*> Keys = TArray<UPrimitiveComponent*>();
	CollisionEnabledTypes.GetKeys(Keys);
	for (UPrimitiveComponent* P : Keys)
	{
		if (IsValid(P))
		{
			P->SetHiddenInGame(!bEnabled);
			P->SetCollisionEnabled(bEnabled ? CollisionEnabledTypes[P].GetValue() : ECollisionEnabled::NoCollision);
			P->UpdateOverlaps();
		}
	}
}

/**
 * Registers a primitive component for collision and visibility management.
 * Stores its original collision state for later restoration.
 * @param PrimitiveComponent The component to register.
 */
void IReusableInterface::AddEnabledType_Implementation(UPrimitiveComponent* PrimitiveComponent)
{
	if (IsValid(PrimitiveComponent))
	{
		CollisionEnabledTypes.FindOrAdd(PrimitiveComponent);
		CollisionEnabledTypes[PrimitiveComponent] = PrimitiveComponent->GetCollisionEnabled();
	}
}

/**
 * Applies damage to a target actor using the networked damage system.
 * Calls the server-side damage function on the instigating player controller if available.
 * @param DamageReceiver The actor receiving damage.
 * @param DamageAmount The amount of damage to apply.
 * @param DamageEvent The damage event struct.
 */
void IReusableInterface::DoDamage_Implementation(AActor* DamageReceiver, float DamageAmount, FDamageEvent const& DamageEvent)
{
	AActor* Self = Cast<AActor>(this);
	if (!IsValid(Self))
	{
		return;
	}

	AShooterPlayerController* ShooterPlayerController = Self->GetInstigatorController<AShooterPlayerController>();
	if (IsValid(ShooterPlayerController))
	{
		ShooterPlayerController->Server_DoDamage(DamageReceiver, DamageAmount, DamageEvent, ShooterPlayerController, Self);
	}
}