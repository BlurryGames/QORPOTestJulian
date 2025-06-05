#include "AttributesComponent.h"

UAttributesComponent::UAttributesComponent()
{

}

void UAttributesComponent::HealthReaction(const float Amount, bool bDamage)
{
	const float Health = CurrentHealth;
	CurrentHealth = FMath::Clamp(Health + (bDamage ? -abs(Amount) : abs(Amount)), 0.0f, MaxHealth);
	if (Health != CurrentHealth)
	{
		OnHealthChanged.Broadcast(CurrentHealth);
	}
}
