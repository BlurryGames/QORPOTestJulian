#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"

#include "AttributesComponent.generated.h"

/**
 * Delegate broadcast when the health value changes.
 * @param HealthResult The new health value after the change.
 * @param TotalHealth The maximum health value.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, const float, HealthResult, const float, TotalHealth);

/**
 * UAttributesComponent
 *
 * Component that manages health attributes for an actor.
 * Handles health value, health changes, replication, and provides utility functions for health manipulation.
 * Designed to be used in both C++ and Blueprints.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class QORPOTESTJULIAN_API UAttributesComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	/** Event triggered when the health value changes. */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChanged OnHealthChanged;

	/** Default constructor. Initializes default values and enables replication. */
	UAttributesComponent();

	/**
	 * Returns the maximum health value.
	 * @return The maximum health.
	 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	const float GetMaxHealth() const;

	/**
	 * Returns the current health value.
	 * @return The current health.
	 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	const float GetHealth() const;

	/**
	 * Applies a health change (damage or healing) to the component.
	 * Broadcasts the OnHealthChanged event if the health value changes.
	 * @param Amount The amount to change health by (positive for healing, negative for damage).
	 * @return True if the health value was changed, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool HealthReaction(const float Amount);

	/**
	 * Resets the current health to the maximum health value.
	 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void ResetHealth();

protected:
	/** The maximum health value for the owning actor. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes", meta = (ClampMin = 1.0f, ClampMax = 1000.0f))
	float MaxHealth = 100.0f;

	/** The current health value, replicated to clients. */
	UPROPERTY(ReplicatedUsing = OnReplicateCurrentHealth, EditAnywhere, BlueprintReadWrite, Category = "Attributes", meta = (ClampMin = 1.0f, ClampMax = 1000.0f))
	float CurrentHealth = MaxHealth;

	/** Called when the game starts. Initializes health values. */
	virtual void BeginPlay() override;

	/**
	 * Registers properties for network replication.
	 * @param OutLifetimeProps The array to add replicated properties to.
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * Called when the component is removed from the world.
	 * @param EndPlayReason The reason for removal.
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * Called when the CurrentHealth property is replicated.
	 * Broadcasts the OnHealthChanged event.
	 */
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void OnReplicateCurrentHealth();
};
