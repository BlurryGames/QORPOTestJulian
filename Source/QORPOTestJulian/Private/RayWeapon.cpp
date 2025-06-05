#include "RayWeapon.h"

void ARayWeapon::HandleFire_Implementation()
{
	if (bActiveTrigger && Magazine > 0)
	{
		FVector PivotPosition = MuzzleComponent->GetComponentLocation();
		bool bHit = GetWorld()->LineTraceSingleByObjectType(LineTraceHitResult,
			PivotPosition, PivotPosition + MuzzleComponent->GetForwardVector() * MaxRange,
			ObjectParams, LineTraceParams);
		AActor* HitActor = LineTraceHitResult.GetActor();
		UAttributesComponent* AttributesComponent = IsValid(HitActor) ? HitActor->FindComponentByClass<UAttributesComponent>() : nullptr;
		if (bHit && IsValid(AttributesComponent))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, *HitActor->GetName());
			AttributesComponent->HealthReaction(Damage);
		}
	}

	Super::HandleFire_Implementation();
}