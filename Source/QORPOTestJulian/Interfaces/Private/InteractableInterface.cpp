// Copyright (c) Julián López Barañano. All Rights Reserved.

/**
 * @file InteractableInterface.cpp
 * @brief Implements the default logic for the IInteractableInterface, which defines interaction behavior for actors.
 *
 * This file provides the default C++ implementations for the interaction and animation methods of the interface.
 * By default, these methods log an error if not overridden, ensuring that derived classes implement their own logic.
 * Designed to be extended by any actor that should support interaction in the game world.
 */

#include "../Public/InteractableInterface.h"

 /**
  * Default implementation for the OnInteract event.
  * Logs an error if not overridden in a derived class, indicating that interaction logic is missing.
  *
  * @param Caller The actor that initiated the interaction.
  */
void IInteractableInterface::OnInteract_Implementation(AActor* Caller)
{
	AActor* Owner = Cast<AActor>(this);
	ensureMsgf(false, TEXT("Pickup() not implemented on object '%s' called by '%s'"),
		IsValid(Owner) ? *Owner->GetName() : TEXT("Owner is not an AActor or not exist"),
		Caller ? TEXT("Valid Caller") : TEXT("Caller not exist"));
}

/**
 * Default implementation for the OnInteractionAnimation event.
 * Logs an error if not overridden in a derived class, indicating that animation logic is missing.
 *
 * @param DeltaTime Time elapsed since the last tick.
 */
void IInteractableInterface::OnInteractionAnimation_Implementation(const float DeltaTime)
{
	AActor* Owner = Cast<AActor>(this);
	ensureMsgf(false, TEXT("OnInteractionAvailableAnimation() not implemented on object '%s'"),
		IsValid(Owner) ? *Owner->GetName() : TEXT("Owner is not an AActor or not exist"));
}