#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "../../Core/Public/HealEvent.h"
#include "../../Components/Public/AttributesComponent.h"

#include "ShooterPlayer.generated.h"

class ABaseWeapon;
class ADoor;

/**
 * Delegate broadcast when the weapon magazine is updated.
 * @param Magazine The new magazine value.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponMagazineUpdated, const int, Magazine);

/**
 * Delegate broadcast when the player's ammunition is updated.
 * @param Amount The new ammunition value.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerAmmunitionUpdated, const int, Amount);

/**
 * Delegate broadcast when a reload is performed.
 * @param Amount The amount of ammunition spent.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadSpent, const int, Amount);

/**
 * Delegate broadcast when the shoot input is held or released.
 * @param bHold True if the shoot input is held, false otherwise.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShootHeld, const bool, bHold);

/**
 * AShooterPlayer
 *
 * Main player character class for the shooter game.
 * Handles player input, movement, health, weapon management, interaction, and network replication.
 * Designed for extension in both C++ and Blueprints.
 */
UCLASS(Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API AShooterPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	/** Event triggered when the player's ammunition is updated. */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPlayerAmmunitionUpdated OnPlayerAmmunitionUpdated;

	/** Event triggered when the weapon's magazine is updated. */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWeaponMagazineUpdated OnWeaponMagazineUpdated;

	/** Event triggered when a reload is performed. */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnReloadSpent OnReloadSpent;

	/** Event triggered when the shoot input is held or released. */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnShootHeld OnShootHeld;

	/** Default constructor. Initializes components and default values. */
	AShooterPlayer();

	/**
	 * Handles incoming damage or healing events for the player.
	 * @param DamageAmount The amount of damage or healing.
	 * @param DamageEvent The event describing the type of damage or healing.
	 * @param EventInstigator The controller responsible for the event.
	 * @param DamageCauser The actor that caused the event.
	 * @return The final amount of damage or healing applied.
	 */
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/**
	 * Returns the attributes component for this player.
	 * @return The attributes component.
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	UAttributesComponent* GetAttributesComponent() const;

	/**
	 * Returns the current amount of ammunition.
	 * @return The ammunition count.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stats|Equipment")
	const int GetAmmunition() const;

	/**
	 * Adds ammunition to the player and broadcasts the update.
	 * @param Amount The amount of ammunition to add.
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void AddAmmunition(const int Amount);

protected:
	/** Scene component used as the socket for attaching weapons. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* WeaponSocketComponent = nullptr;

	/** Component managing the player's attributes such as health. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAttributesComponent* AttributesComponent = nullptr;

	/** The currently equipped weapon. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Equipment")
	ABaseWeapon* CurrentWeapon = nullptr;

	/** Current movement direction based on input. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Movement")
	FVector MovementDirection = FVector::ZeroVector;

	/** Hit result for interaction line traces. */
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	FHitResult LineTraceHitInteraction = FHitResult();

	/** Maximum range for player interaction (e.g., with doors or objects). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (ClampMin = 0.0f, ClampMax = 1000.0f))
	float InteractionRange = 200.0f;

	/** Default walking speed for the player. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Movement")
	float DefaultSpeed = 0.0f;

	/** Multiplier applied to speed when sprinting. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|Movement", meta = (ClampMin = 1.1f, ClampMax = 10.0f))
	float SprintMultiplier = 1.6f;

	/** Current amount of ammunition, replicated to clients. */
	UPROPERTY(ReplicatedUsing = OnReplicateAmmunition, EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|Equipment", meta = (ClampMin = 0, ClampMax = 10000))
	int Ammunition = 0;

	/** Collision object query parameters for interaction traces. */
	FCollisionObjectQueryParams ObjectParams = FCollisionObjectQueryParams(ECC_WorldDynamic);

	/** Collision query parameters for interaction traces. */
	FCollisionQueryParams LineTraceParams = FCollisionQueryParams();

	/**
	 * Sets up player input bindings for movement, actions, and interactions.
	 * @param PlayerInputComponent The input component to bind actions and axes to.
	 */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

	/**
	 * Registers properties for network replication.
	 * @param OutLifetimeProps The array to add replicated properties to.
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * Called every frame.
	 * Handles movement input and draws debug lines for interaction.
	 * @param DeltaTime Time elapsed since the last tick.
	 */
	virtual void Tick(float DeltaTime) override;

	/**
	 * Called when another actor begins to overlap with this player.
	 * Attempts to equip a weapon if the overlapping actor is a weapon.
	 * @param OtherActor The actor that started overlapping.
	 */
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	/**
	 * Adds pitch input to the controller, considering player settings for inverted pitch.
	 * @param Value The pitch input value.
	 */
	virtual void AddControllerPitchInput(float Value) override;

	/**
	 * Called when the player is removed from the world.
	 * Handles weapon unequip logic.
	 * @param EndPlayReason The reason for removal.
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * Calculates the current movement direction based on input.
	 * @return The movement direction vector.
	 */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetMovementDirection() const;

	/**
	 * Equips the specified weapon, attaching it to the player and binding events.
	 * @param Weapon The weapon to equip.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnEquipWeapon(ABaseWeapon* Weapon);

	/**
	 * Unequips the current weapon, detaching it and unbinding events.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnUnequipWeapon();

	/**
	 * Called when the ammunition value is replicated.
	 * Broadcasts the updated ammunition value.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stats|Equipment")
	void OnReplicateAmmunition();

	/**
	 * Handles changes in the player's health.
	 * Destroys the player if health reaches zero.
	 * @param HealthResult The new health value.
	 * @param TotalHealth The maximum health value.
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void HandleHealthChange(const float HealthResult, const float TotalHealth);

	/**
	 * Handles weapon reload events, updating ammunition and broadcasting changes.
	 * @param AmmunitionSpent The amount of ammunition spent during reload.
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void HandleReloaded(const int AmmunitionSpent);

	/** Handles forward movement input. */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void HandleMoveForward();

	/** Handles backward movement input. */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void HandleMoveBackward();

	/** Handles right movement input. */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void HandleMoveRight();

	/** Handles left movement input. */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void HandleMoveLeft();

	/** Handles sprint input, toggling between default and sprint speed. */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void HandleSprint();

	/** Handles crouch input, toggling crouch state. */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void HandleCrouch();

	/** Handles jump input. */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void HandleJump();

	/** Handles the start of shooting input. Broadcasts the shoot held event. */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Interaction")
	void HandleStartShoot();

	/** Handles the stop of shooting input. Broadcasts the shoot held event. */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Interaction")
	void HandleStopShoot();

	/** Handles reload input. Broadcasts the reload spent event. */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Interaction")
	void HandleReload();

	/**
	 * Handles interaction input, performing a line trace and interacting with doors if hit.
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void HandleInteraction();

	/**
	 * Server-side implementation for interacting with a door.
	 * @param Door The door to interact with.
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Interaction")
	void Server_DoorInteraction(ADoor* Door);

	/**
	 * Server-side implementation for updating the pitch view.
	 * @param PitchInput The pitch value to update.
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Movement")
	void Server_UpdatePitchView(const float PitchInput);

	/**
	 * Multicast implementation for updating the pitch view on all clients.
	 * @param PitchInput The pitch value to update.
	 */
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Movement")
	void Multicast_UpdatePitchView(const float PitchInput);
};
