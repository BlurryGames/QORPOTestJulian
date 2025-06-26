// Copyright (c) Julián López Barañano. All Rights Reserved.

/**
 * @file AttributesComponent.cpp
 * @brief Implements the logic for the UAttributesComponent class, which manages health attributes for an actor.
 *
 * This component handles health value storage, health changes, replication, and provides utility functions for health manipulation.
 * It is designed to be used in both C++ and Blueprints, and supports networked games.
 */

#include "../Public/AttributesComponent.h"

/**
 * Default constructor.
 * Enables replication by default for this component.
 */
UAttributesComponent::UAttributesComponent()
{
	SetIsReplicatedByDefault(true);
}

/**
 * Called when the game starts.
 * Initializes the current health to the maximum health value.
 */
void UAttributesComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
}

/**
 * Registers properties for network replication.
 * @param OutLifetimeProps The array to add replicated properties to.
 */
void UAttributesComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAttributesComponent, CurrentHealth);
}

/**
 * Called when the component is removed from the world.
 * Clears the OnHealthChanged delegate to avoid dangling references.
 * @param EndPlayReason The reason for removal.
 */
void UAttributesComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	OnHealthChanged.Clear();
}

/**
 * Returns the maximum health value.
 * @return The maximum health.
 */
const float UAttributesComponent::GetMaxHealth() const
{
	return MaxHealth;
}

/**
 * Returns the current health value.
 * @return The current health.
 */
const float UAttributesComponent::GetHealth() const
{
	return CurrentHealth;
}

/**
 * Applies a health change (damage or healing) to the component.
 * Clamps the new health value between 0 and MaxHealth.
 * Broadcasts the OnHealthChanged event if the health value changes.
 * @param Amount The amount to change health by (positive for healing, negative for damage).
 * @return True if the health value was changed, false otherwise.
 */
bool UAttributesComponent::HealthReaction(const float Amount)
{
	const float Health = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);
	const bool bSuccess = Health != CurrentHealth;
	if (bSuccess)
	{
		CurrentHealth = Health;
		OnHealthChanged.Broadcast(Health, MaxHealth);
	}

	return bSuccess;
}

/**
 * Resets the current health to the maximum health value.
 */
void UAttributesComponent::ResetHealth()
{
	CurrentHealth = MaxHealth;
}

/**
 * Called when the CurrentHealth property is replicated.
 * Broadcasts the OnHealthChanged event to update listeners.
 */
void UAttributesComponent::OnReplicateCurrentHealth()
{
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}