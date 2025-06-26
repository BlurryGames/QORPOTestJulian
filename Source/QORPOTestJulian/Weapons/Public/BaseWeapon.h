#pragma once

#include "CoreMinimal.h"
#include "../../Interactables/Public/BaseItem.h"
#include "Components/AudioComponent.h"

#include "BaseWeapon.generated.h"

class AShooterPlayer;

/**
 * Delegate for broadcasting reload events.
 * @param Amount The number of bullets reloaded.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloaded, const int, Amount);

/**
 * ABaseWeapon
 *
 * Abstract base class for all weapon actors in the game.
 * Handles firing, reloading, magazine management, owner assignment, and network replication.
 * Designed to be extended for custom weapon behavior and supports both C++ and Blueprint customization.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API ABaseWeapon : public ABaseItem
{
	GENERATED_BODY()

public:
	/** Event triggered when the weapon is reloaded. */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnReloaded OnReloaded;

	/** Default constructor. Initializes components and default values. */
	ABaseWeapon();

	/**
	 * Sets the owner of the weapon and manages event bindings for the previous and new owner.
	 * @param NewOwner The new owner actor.
	 */
	virtual void SetOwner(AActor* NewOwner) override;

	/**
	 * Returns the world position the weapon is aiming at, based on the muzzle's forward vector and max range.
	 * @return The aim position as an FVector.
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon|Stats")
	const FVector GetAimPosition() const;

	/**
	 * Returns the current number of bullets in the magazine.
	 * @return The magazine count.
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon|Stats")
	const int GetMagazine() const;

protected:
	/** Scene component representing the muzzle location for spawning projectiles. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* MuzzleComponent = nullptr;

	/** Audio component for weapon sound effects. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAudioComponent* AudioComponent = nullptr;

	/** Timer handle for managing reload timing. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	FTimerHandle ReloadTimerHandle = FTimerHandle();

	/** Timer handle for managing firing cadence. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	FTimerHandle CadencyTimerHandle = FTimerHandle();

	/** Timer handle for managing interval-based firing. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	FTimerHandle IntervalTimerHandle = FTimerHandle();

	/** Maximum range of the weapon's projectiles or hitscan. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stats", meta = (ClampMin = 10.0f, ClampMax = 10000.0f))
	float MaxRange = 10000.0f;

	/** Time in seconds required to reload the weapon. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stats", meta = (ClampMin = 0.4f, ClampMax = 10.0f))
	float ReloadTime = 2.0f;

	/** Time in seconds between consecutive shots. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stats", meta = (ClampMin = 0.1f, ClampMax = 10.0f))
	float CadencyTime = 0.2f;

	/** Proportion of the cadence time used for interval-based firing. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stats", meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float IntervalProportionTime = 0.0f;

	/** Number of bullets consumed per shot. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stats", meta = (ClampMin = 1, ClampMax = 100))
	int ShotCost = 1;

	/** Maximum number of bullets the magazine can hold. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stats", meta = (ClampMin = 1, ClampMax = 100))
	int MagazineCapacity = 30;

	/** Current number of bullets in the magazine. Replicated. */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Stats")
	int Magazine = MagazineCapacity;

	/** Current interval count for interval-based firing. Replicated. */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Stats")
	int IntervalCount = 0;

	/** Whether the weapon's trigger is currently active. Replicated. */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|State")
	bool bActiveTrigger = false;

	/** Called when the weapon is spawned or the game starts. Initializes the firing cadence timer. */
	virtual void BeginPlay() override;

	/** Registers properties for network replication. */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Called every frame. Handles debug drawing and per-frame logic. */
	virtual void Tick(float DeltaTime) override;

	/** Called when the weapon is removed from the world. Unbinds events from the owner. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * Handles interaction with the weapon (e.g., when picked up by a player).
	 * Sets the owner, disables collision, and resets relevant states.
	 * @param Caller The actor that initiated the interaction.
	 */
	virtual void OnInteract_Implementation(AActor* Caller) override;

	/**
	 * Binds or unbinds weapon events to the specified player.
	 * @param Player The player to bind/unbind events to.
	 * @param bConnect True to bind, false to unbind.
	 * @return True if the player is valid and events were set.
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon|State")
	bool SetEvents(AShooterPlayer* Player, const bool bConnect = true);

	/**
	 * Handles the firing logic for the weapon.
	 * Manages magazine count, firing cadence, and triggers the fire mechanism.
	 * @return True if the weapon fired successfully.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|State")
	bool HandleFire();

	/**
	 * Handles the reload logic when the player requests a reload.
	 * Starts the reload timer and prepares the reload delegate.
	 * @param BulletsAmount The number of bullets to reload.
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon|State")
	void HandleReloadSpent(const int BulletsAmount);

	/**
	 * Handles the shoot held event, managing trigger state and firing cadence.
	 * @param bHold True if the shoot button is held, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon|State")
	void HandleShootHeld(const bool bHold);

	/**
	 * Completes the reload process, updating the magazine count and resuming firing if needed.
	 * @param BullettsAmount The number of bullets to reload.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|State")
	void HandleReloadCompleted(const int BullettsAmount);

	/**
	 * Multicast function to play the fire mechanism (e.g., sound) across the network.
	 */
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Weapon|State")
	void Multicast_FireMechanism();

private:
	/** Delegate used internally to manage the firing cadence. */
	const FTimerDelegate CadencyDelegate = FTimerDelegate::CreateUFunction(this, GET_FUNCTION_NAME_CHECKED(ABaseWeapon, HandleFire));
	/** Delegate used internally to manage the reload process. */
	FTimerDelegate ReloadDelegate = FTimerDelegate();
};
