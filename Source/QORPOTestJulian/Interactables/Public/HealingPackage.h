#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "../../Core/Public/HealEvent.h"

#include "HealingPackage.generated.h"

/**
 * AHealingPackage
 *
 * Represents an interactable healing package in the game world.
 * When a player overlaps or interacts with this actor, it applies a healing effect using a custom heal event.
 * Designed to be placed in the level or spawned dynamically, and supports Blueprint extension.
 */
UCLASS(Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API AHealingPackage : public ABaseItem
{
	GENERATED_BODY()

protected:
	/**
	 * The healing event struct used to apply healing to the interacting actor.
	 * Configurable in Blueprints for custom healing logic.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Heal")
	FHealEvent HealEvent = FHealEvent();

	/**
	 * The amount of health restored when this package is collected or interacted with.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Heal", meta = (ClampMin = 1.0f, ClampMax = 1000.0f))
	float HealingPack = 20.0f;

	/**
	 * Called when another actor begins to overlap with this healing package.
	 * Triggers the interaction logic, allowing the overlapping actor to receive healing.
	 * @param OtherActor The actor that started overlapping.
	 */
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	/**
	 * Called when this item is interacted with (e.g., via overlap or explicit interaction).
	 * Applies healing to the caller and disables the package if healing is successful.
	 * @param Caller The actor that initiated the interaction.
	 */
	virtual void OnInteract_Implementation(AActor* Caller) override;
};
