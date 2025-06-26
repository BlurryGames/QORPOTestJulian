// Copyright (c) Julián López Barañano. All Rights Reserved.

/**
 * @file ProjectileWeapon.cpp
 * @brief Implements the logic for the AProjectileWeapon class, which represents a weapon that manages and fires reusable projectile actors.
 *
 * This class handles the spawning, pooling, and firing of projectiles, as well as owner assignment and interaction logic.
 * It is designed to be extended for custom projectile weapon behavior and supports both C++ and Blueprint customization.
 */

#include "../Public/ProjectileWeapon.h"

/**
 * Default constructor.
 * Initializes the projectiles container component and sets its transform to be absolute.
 */
AProjectileWeapon::AProjectileWeapon() : Super()
{
	ProjectilesContainerComponent = CreateDefaultSubobject<USceneComponent>(FName("ProjectilesContainerComponent"));
	ProjectilesContainerComponent->SetAbsolute(true, true, true);
	ProjectilesContainerComponent->SetupAttachment(GetRootComponent());
}

/**
 * Called when the weapon is spawned or the game starts.
 * Spawns and pools all projectiles for this weapon if authority is present.
 */
void AProjectileWeapon::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!HasAuthority() || !IsValid(World) || !IsValid(ProjectileClass))
	{
		return;
	}

	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
	SpawnParameters.Owner = this;
	const FVector& SpawnPosition = GetActorLocation();
	const FRotator& SpawnRotation = GetActorRotation();
	for (unsigned short i = 0; i < MagazineCapacity; i++)
	{
		ABaseProjectile* Projectile = World->SpawnActor<ABaseProjectile>(ProjectileClass, SpawnPosition, SpawnRotation, SpawnParameters);
		Projectile->AttachToComponent(ProjectilesContainerComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		Projectile->SetOwner(this);
		ProjectilesContainer.Insert(Projectile, ProjectilesContainer.Num());
	}
}

/**
 * Handles the firing logic for the weapon.
 * Activates the next available projectile from the pool and fires it from the muzzle location.
 * @return True if the weapon fired successfully.
 */
bool AProjectileWeapon::HandleFire_Implementation()
{
	int Index = CurrentIndex;
	bool bSuccess = !ProjectilesContainer.IsEmpty() && Super::HandleFire_Implementation();
	if (!bSuccess)
	{
		return bSuccess;
	}
	else if (++Index >= ProjectilesContainer.Num())
	{
		Index = 0;
	}

	if (CurrentIndex != Index)
	{
		CurrentIndex = Index;
	}

	if (ProjectilesContainer.IsValidIndex(CurrentIndex) && IsValid(ProjectilesContainer[CurrentIndex]))
	{
		ProjectilesContainer[CurrentIndex]->Multicast_ProjectileOut(
			IsValid(MuzzleComponent) ? MuzzleComponent->GetComponentLocation() : GetActorLocation(),
			IsValid(MuzzleComponent) ? MuzzleComponent->GetComponentRotation() : GetActorRotation());
	}

	return bSuccess;
}

/**
 * Handles interaction with the weapon (e.g., when picked up by a player).
 * Sets the owner for all pooled projectiles to match the weapon's owner.
 * @param Caller The actor that initiated the interaction.
 */
void AProjectileWeapon::OnInteract_Implementation(AActor* Caller)
{
	Super::OnInteract_Implementation(Caller);

	for (ABaseProjectile* Projectile : ProjectilesContainer)
	{
		if (IsValid(Projectile))
		{
			Projectile->SetOwner(GetOwner());
		}
	}
}