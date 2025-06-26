#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "BaseProjectile.h"

#include "ProjectileWeapon.generated.h"

/**
 * AProjectileWeapon
 *
 * Weapon class that manages and fires reusable projectile actors.
 * Handles projectile pooling, spawning, and firing logic, as well as owner assignment and interaction.
 * Designed to be extended for custom projectile weapon behavior and supports both C++ and Blueprint customization.
 */
UCLASS(Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API AProjectileWeapon : public ABaseWeapon
{
	GENERATED_BODY()

public:
	/** Default constructor. Initializes the projectiles container component. */
	AProjectileWeapon();

protected:
	/** Scene component that acts as the parent for all pooled projectiles. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Projectile")
	USceneComponent* ProjectilesContainerComponent = nullptr;

	/** Array containing all pooled projectile instances managed by this weapon. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Projectile")
	TArray<ABaseProjectile*> ProjectilesContainer = TArray<ABaseProjectile*>();

	/** The class type of projectile to spawn and pool. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Projectile")
	TSubclassOf<ABaseProjectile> ProjectileClass = nullptr;

	/** Index of the next projectile to be fired from the pool. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Projectile")
	int CurrentIndex = -1;

	/**
	 * Called when the weapon is spawned or the game starts.
	 * Spawns and pools all projectiles for this weapon if authority is present.
	 */
	virtual void BeginPlay() override;

	/**
	 * Handles the firing logic for the weapon.
	 * Activates the next available projectile from the pool and fires it from the muzzle location.
	 * @return True if the weapon fired successfully.
	 */
	virtual bool HandleFire_Implementation() override;

	/**
	 * Handles interaction with the weapon (e.g., when picked up by a player).
	 * Sets the owner for all pooled projectiles to match the weapon's owner.
	 * @param Caller The actor that initiated the interaction.
	 */
	virtual void OnInteract_Implementation(AActor* Caller) override;
};
