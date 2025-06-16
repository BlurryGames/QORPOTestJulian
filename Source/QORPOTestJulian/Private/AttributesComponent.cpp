#include "AttributesComponent.h"

void UAttributesComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
}

void UAttributesComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	OnHealthChanged.Clear();
}

const float UAttributesComponent::GetMaxHealth() const
{
	return MaxHealth;
}

const float UAttributesComponent::GetHealth() const
{
	return CurrentHealth;
}

bool UAttributesComponent::HealthReaction(const float Amount)
{
	const float Health = CurrentHealth;
	CurrentHealth = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
	bool bSucces = Health != CurrentHealth;
	if (bSucces)
	{
		OnHealthChanged.Broadcast(CurrentHealth);
	}

	return bSucces;
}

void UAttributesComponent::ResetHealth()
{
	CurrentHealth = MaxHealth;
}
