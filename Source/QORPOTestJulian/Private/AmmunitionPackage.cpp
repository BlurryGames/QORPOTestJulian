#include "AmmunitionPackage.h"

void AAmmunitionPackage::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	Execute_OnInteract(this, OtherActor);
}

void AAmmunitionPackage::OnInteract_Implementation(AActor* Caller)
{
	AShooterPlayer* Player = Cast<AShooterPlayer>(Caller);
	if (IsValid(Player))
	{
		Player->AddAmmunition(AmmunitionPack);
		Execute_OnTurnEnabled(this, false);
	}
}
