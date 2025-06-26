// Copyright (c) Julián López Barañano. All Rights Reserved.

/**
 * @file HealingPackage.cpp
 * @brief Implements the logic for the AHealingPackage class, which represents an interactable healing item in the game world.
 *
 * This class handles overlap and interaction events, applying a healing effect to the interacting actor.
 * It is designed to be extended and supports both C++ and Blueprint customization.
 */

#include "../Public/HealingPackage.h"

/**
 * Called when another actor begins to overlap with this healing package.
 * Triggers the interaction logic, allowing the overlapping actor to receive healing.
 *
 * @param OtherActor The actor that started overlapping with this package.
 */
void AHealingPackage::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	Execute_OnInteract(this, OtherActor);
}

/**
 * Called when this item is interacted with (e.g., via overlap or explicit interaction).
 * Applies healing to the caller and disables the package if healing is successful.
 *
 * @param Caller The actor that initiated the interaction.
 */
void AHealingPackage::OnInteract_Implementation(AActor* Caller)
{
	if (FMath::IsNearlyEqual(Caller->TakeDamage(HealingPack, HealEvent, GetInstigatorController(), this), HealEvent.HealSuccess))
	{
		Execute_OnTurnEnabled(this, false);
	}
}