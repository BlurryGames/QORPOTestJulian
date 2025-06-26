#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "../../Characters/Public/ShooterPlayer.h"

#include "AmmunitionPackage.generated.h"

/**
 * AAmmunitionPackage
 *
 * Represents an interactable ammunition package in the game world.
 * When a player overlaps or interacts with this actor, it provides additional ammunition to the player.
 * Designed to be placed in the level or spawned dynamically, and supports Blueprint extension.
 */
UCLASS(Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API AAmmunitionPackage : public ABaseItem
{
	GENERATED_BODY()
	
protected:
	/**
	 * The amount of ammunition this package provides to the player upon pickup or interaction.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (ClampMin = 1, ClampMax = 1000))
	int AmmunitionPack = 30;

	/**
	 * Called when another actor begins to overlap with this ammunition package.
	 * If the overlapping actor is a valid player, attempts to give ammunition.
	 * @param OtherActor The actor that started overlapping.
	 */
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	/**
	 * Called when this item is interacted with (e.g., via a use key or interaction system).
	 * If the caller is a valid player, gives ammunition to the player.
	 * @param Caller The actor that initiated the interaction.
	 */
	virtual void OnInteract_Implementation(AActor* Caller) override;
};
