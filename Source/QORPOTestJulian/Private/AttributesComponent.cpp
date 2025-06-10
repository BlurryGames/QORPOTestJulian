#include "AttributesComponent.h"

void UAttributesComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
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

	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::FromInt(CurrentHealth));
	return bSucces;
}
