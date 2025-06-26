#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"

#include "RayWeapon.generated.h"

/**
 * ARayWeapon
 *
 * Weapon class that implements hitscan (raycast) firing logic.
 * Uses line traces to instantly detect and apply damage to hit actors along the weapon's firing direction.
 * Designed to be extended for custom hitscan weapon behavior and supports both C++ and Blueprint customization.
 */
UCLASS(Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API ARayWeapon : public ABaseWeapon
{
	GENERATED_BODY()

public:
	/** Default constructor. Initializes trace parameters and ignored actors. */
	ARayWeapon();
	
protected:
	/** Stores the result of the most recent line trace (hit scan). */
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	FHitResult LineTraceHitResult = FHitResult();

	/** Amount of damage dealt by the weapon on a successful hit (should be negative for damage). */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Stats", meta = (ClampMin = -1000.0f, ClampMax = -0.1f))
	float Damage = -10.0f;

	/** Object query parameters used for line tracing (defines which object types can be hit). */
	FCollisionObjectQueryParams ObjectParams = FCollisionObjectQueryParams(ECC_Pawn);

	/** Additional parameters for line tracing (e.g., ignored actors). */
	FCollisionQueryParams LineTraceParams = FCollisionQueryParams();

	/**
	 * Handles the firing logic for the weapon.
	 * Performs a line trace from the muzzle or actor location in the forward direction up to MaxRange.
	 * Applies damage to the first valid hit actor.
	 * @return True if the weapon fired successfully.
	 */
	virtual bool HandleFire_Implementation() override;
};
