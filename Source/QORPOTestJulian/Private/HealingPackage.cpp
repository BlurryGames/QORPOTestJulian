#include "HealingPackage.h"

void AHealingPackage::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	Execute_OnInteract(this, OtherActor);
}

void AHealingPackage::OnInteract_Implementation(AActor* Caller)
{
	if (FMath::IsNearlyEqual(Caller->TakeDamage(HealingPack, HealEvent, GetInstigatorController(), this), HealEvent.HealSuccess))
	{
		Execute_OnTurnEnabled(this, false);
	}
}