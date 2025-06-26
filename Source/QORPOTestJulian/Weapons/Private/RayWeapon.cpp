// Copyright (c) Julián López Barañano. All Rights Reserved.

/**
 * @file RayWeapon.cpp
 * @brief Implements the logic for the ARayWeapon class, which represents a hitscan weapon using line traces for instant impact.
 *
 * This class handles initialization of trace parameters, firing logic using line traces, and applies damage to hit actors.
 * Designed to be extended for custom hitscan weapon behavior and supports both C++ and Blueprint customization.
 */

#include "../Public/RayWeapon.h"
#include "../../Core/Public/HealEvent.h"

 /**
  * Default constructor.
  * Initializes object query parameters for line tracing and sets up ignored actors.
  */
ARayWeapon::ARayWeapon() : Super()
{
	ObjectParams.AddObjectTypesToQuery(ECC_PhysicsBody);
	ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
	LineTraceParams.AddIgnoredActor(this);
}

/**
 * Handles the firing logic for the weapon.
 * Performs a line trace from the muzzle or actor location in the forward direction up to MaxRange.
 * Applies damage to the first valid hit actor.
 * @return True if the weapon fired successfully.
 */
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
	if (IsValid(HitActor))
	{
		HitActor->TakeDamage(Damage, FDamageEvent(), GetInstigatorController(), this);
	}

	return bSucces;
}