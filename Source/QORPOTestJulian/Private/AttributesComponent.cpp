#include "AttributesComponent.h"

UAttributesComponent::UAttributesComponent()
{
	SetIsReplicatedByDefault(true);
}

void UAttributesComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
}

void UAttributesComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAttributesComponent, CurrentHealth);
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
	const float Health = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);
	const bool bSuccess = Health != CurrentHealth;
	if (bSuccess)
	{
		CurrentHealth = Health;
		OnHealthChanged.Broadcast(Health, MaxHealth);
	}

	return bSuccess;
}

void UAttributesComponent::ResetHealth()
{
	CurrentHealth = MaxHealth;
}

void UAttributesComponent::OnReplicateCurrentHealth()
{
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}