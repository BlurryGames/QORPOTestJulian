#include "RayWeapon.h"

ARayWeapon::ARayWeapon() : Super()
{
	ObjectParams.AddObjectTypesToQuery(ECC_PhysicsBody);
	ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
	LineTraceParams.AddIgnoredActor(this);
}

bool ARayWeapon::HandleFire_Implementation()
{
	UWorld* World = GetWorld();
	const bool bSucces = IsValid(World) && Super::HandleFire_Implementation();
	if (!bSucces)
	{
		return bSucces;
	}

	FVector PivotPosition = IsValid(MuzzleComponent) ? MuzzleComponent->GetComponentLocation() : GetActorLocation();
	World->LineTraceSingleByObjectType(LineTraceHitResult, PivotPosition, 
		PivotPosition + (IsValid(MuzzleComponent) ? MuzzleComponent->GetForwardVector() : GetActorForwardVector()) * MaxRange,
		ObjectParams, LineTraceParams);

	AActor* HitActor = LineTraceHitResult.GetActor();
	UAttributesComponent* AttributesComponent = IsValid(HitActor) ? HitActor->FindComponentByClass<UAttributesComponent>() : nullptr;
	if (IsValid(HitActor))
	{
		HitActor->TakeDamage(Damage, FDamageEvent(), GetInstigatorController(), this);
	}

	return bSucces;
}