// Copyright (c) Julián López Barañano. All Rights Reserved.

/**
 * @file AmmunitionPackage.cpp
 * @brief Implements the logic for the AAmmunitionPackage class, which represents an interactable ammunition pickup in the game world.
 *
 * This class handles overlap and interaction events, granting ammunition to the player when collected.
 * It is designed to be extended and supports both C++ and Blueprint customization.
 */

#include "../Public/AmmunitionPackage.h"

/**
 * Called when another actor begins to overlap with this ammunition package.
 * Triggers the interaction logic, allowing the overlapping actor to collect the ammunition.
 *
 * @param OtherActor The actor that started overlapping with this package.
 */
void AAmmunitionPackage::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	Execute_OnInteract(this, OtherActor);
}

/**
 * Called when this item is interacted with (e.g., via overlap or explicit interaction).
 * If the caller is a valid player, grants ammunition and disables the package.
 *
 * @param Caller The actor that initiated the interaction.
 */
void AAmmunitionPackage::OnInteract_Implementation(AActor* Caller)
{
	AShooterPlayer* Player = Cast<AShooterPlayer>(Caller);
	if (IsValid(Player))
	{
		Player->AddAmmunition(AmmunitionPack);
		Execute_OnTurnEnabled(this, false);
	}
}
