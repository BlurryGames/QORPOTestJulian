#include "InteractableInterface.h"

void IInteractableInterface::OnInteract_Implementation(AActor* Caller)
{
	AActor* Owner = Cast<AActor>(this);
	ensureMsgf(false, TEXT("Pickup() not implemented on object '%s' called by '%s'"),
		IsValid(Owner) ? *Owner->GetName() : TEXT("Owner is not an AActor or not exist"),
		Caller ? TEXT("Valid Caller") : TEXT("Caller not exist"));
}

void IInteractableInterface::OnTurnEnabled_Implementation(const bool bEnabled)
{
	AActor* Owner = Cast<AActor>(this);
	if (!IsValid(Owner))
	{
		return;
	}

	Owner->SetActorTickEnabled(bEnabled);
	Owner->SetActorEnableCollision(bEnabled);
	Owner->SetHidden(!bEnabled);
	if (bEnabled)
	{
		Owner->SetActorLocationAndRotation(OriginalPosition, OriginalRotation);
	}
}

void IInteractableInterface::OnInteractionAnimation_Implementation(const float DeltaTime)
{
	AActor* Owner = Cast<AActor>(this);
	ensureMsgf(false, TEXT("OnInteractionAvailableAnimation() not implemented on object '%s'"),
		IsValid(Owner) ? *Owner->GetName() : TEXT("Owner is not an AActor or not exist"));
}