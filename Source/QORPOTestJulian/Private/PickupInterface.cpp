#include "PickupInterface.h"

void IPickupInterface::OnPickup_Implementation(AShooterPlayer* Caller)
{
	AActor* Owner = Cast<AActor>(this);
	ensureMsgf(false, TEXT("Pickup() not implemented on object '%s' called by '%s'"), 
		IsValid(Owner) ? *Owner->GetName() : TEXT("Owner is not an AActor or not exist"), 
		Caller ? TEXT("Valid Caller") : TEXT("Caller not exist"));
}

void IPickupInterface::OnDrop_Implementation()
{
	AActor* Owner = Cast<AActor>(this);
	ensureMsgf(false, TEXT("Drop() not implemented on object '%s'"), IsValid(Owner) ? *Owner->GetName() : TEXT("Owner is not an AActor or not exist"));
}

void IPickupInterface::OnTurnEnabled_Implementation(const bool bEnabled)
{
	AActor* Owner = Cast<AActor>(this);
	if (IsValid(Owner))
	{
		Owner->SetActorTickEnabled(bEnabled);
		Owner->SetActorEnableCollision(bEnabled);
		Owner->SetActorHiddenInGame(!bEnabled);
	}
}