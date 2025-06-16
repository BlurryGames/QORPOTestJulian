#include "InteractableInterface.h"

void IInteractableInterface::OnInteract_Implementation(AActor* Caller)
{
	AActor* Owner = Cast<AActor>(this);
	ensureMsgf(false, TEXT("Pickup() not implemented on object '%s' called by '%s'"),
		IsValid(Owner) ? *Owner->GetName() : TEXT("Owner is not an AActor or not exist"),
		Caller ? TEXT("Valid Caller") : TEXT("Caller not exist"));
}

void IInteractableInterface::OnInteractionAnimation_Implementation(const float DeltaTime)
{
	AActor* Owner = Cast<AActor>(this);
	ensureMsgf(false, TEXT("OnInteractionAvailableAnimation() not implemented on object '%s'"),
		IsValid(Owner) ? *Owner->GetName() : TEXT("Owner is not an AActor or not exist"));
}