// Copyright (c) Julián López Barañano. All Rights Reserved.

/**
 * @file BaseProjectile.cpp
 * @brief Implements the logic for the ABaseProjectile class, which represents a reusable projectile actor in the game world.
 *
 * This class handles projectile initialization, movement, collision, damage application, and networked state.
 * It is designed to be extended for custom projectile behavior and supports both C++ and Blueprint customization.
 */

#include "../Public/BaseProjectile.h"
#include "../../Core/Public/ShooterPlayerController.h"
#include "../Public/BaseWeapon.h"
#include "../../Interactables/Public/ExplosiveBarrel.h"

/**
 * Default constructor.
 * Initializes components, sets up collision, movement, and replication properties for the projectile.
 */
ABaseProjectile::ABaseProjectile()
{
	SetReplicates(true);
	SetReplicateMovement(true);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeshComponent"));
	MeshComponent->SetIsReplicated(true);
	MeshComponent->SetNotifyRigidBodyCollision(true);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	MeshComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	MeshComponent->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
	SetRootComponent(MeshComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovementComponent"));
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	const float& Speed = ProjectileMovementComponent->MaxSpeed = 1000.0f;
	ProjectileMovementComponent->InitialSpeed = Speed;
}

/**
 * Sets the owner of the projectile and updates collision ignore settings.
 * @param NewOwner The new owner actor.
 */
void ABaseProjectile::SetOwner(AActor* NewOwner)
{
	AActor* OldOwner = GetOwner();
	if (IsValid(MeshComponent))
	{
		if (!IsValid(Cast<ABaseWeapon>(OldOwner)))
		{
			MeshComponent->IgnoreActorWhenMoving(OldOwner, false);
		}

		MeshComponent->IgnoreActorWhenMoving(NewOwner, true);
	}

	Super::SetOwner(NewOwner);

	SetInstigator(Cast<APawn>(NewOwner));
}

/**
 * Called when the game starts or when spawned.
 * Initializes enabled types and disables the projectile by default.
 * Deactivates movement on clients without authority.
 */
void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();

	Execute_AddEnabledType(this, MeshComponent);
	Execute_OnTurnEnabled(this, false);
	if (!HasAuthority() && IsValid(ProjectileMovementComponent))
	{
		ProjectileMovementComponent->Deactivate();
	}
}

/**
 * Called when another actor begins to overlap with this projectile.
 * Triggers the impact logic.
 * @param OtherActor The actor that started overlapping.
 */
void ABaseProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	ImpactBody(OtherActor);
}

/**
 * Called when the projectile hits another actor or component.
 * Triggers the impact logic.
 * @param MyComp The component that was hit.
 * @param OtherActor The actor that was hit.
 * @param OtherComp The component that was hit.
 * @param bSelfMoved Whether the hit was caused by this actor's movement.
 * @param HitLocation The location of the hit.
 * @param HitNormal The normal at the hit location.
 * @param NormalImpulse The impulse applied at the hit.
 * @param Hit The hit result data.
 */
void ABaseProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, OtherActor, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	ImpactBody(OtherActor);
}

/**
 * Called when the projectile is removed from the world.
 * Clears the lifetime timer.
 * @param EndPlayReason The reason for removal.
 */
void ABaseProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearTimer(LifeTimeHandle);
}

/**
 * Multicast function to update the projectile's position, rotation, and enabled state across the network.
 * @param Position The new world position.
 * @param Rotation The new world rotation.
 * @param bEnable Whether the projectile should be enabled.
 */
void ABaseProjectile::Multicast_ProjectileOut_Implementation(const FVector& Position, const FRotator& Rotation, const bool bEnable)
{
	Execute_SetOriginalPositionAndRotation(this, Position, Rotation);
	Execute_OnTurnEnabled(this, bEnable);
}

/**
 * Enables or disables the projectile and its movement.
 * Starts or stops the lifetime timer as appropriate.
 * @param bEnabled Whether the projectile should be enabled.
 */
void ABaseProjectile::OnTurnEnabled_Implementation(const bool bEnabled)
{
	IReusableInterface::OnTurnEnabled_Implementation(bEnabled);

	if (!HasAuthority())
	{
		return;
	}
	else if (IsValid(ProjectileMovementComponent))
	{
		ProjectileMovementComponent->StopMovementImmediately();
		ProjectileMovementComponent->SetVelocityInLocalSpace(FVector(ProjectileMovementComponent->InitialSpeed, 0.0f, 0.0f));
		ProjectileMovementComponent->SetActive(bEnabled);
	}

	FTimerManager& TimerManager = GetWorldTimerManager();
	if (bEnabled && !TimerManager.IsTimerActive(LifeTimeHandle))
	{
		TimerManager.SetTimer(LifeTimeHandle, LifeTimeDelegate, LifeTime, false);
	}
	else if (!bEnabled && TimerManager.IsTimerActive(LifeTimeHandle))
	{
		TimerManager.ClearTimer(LifeTimeHandle);
	}
}

/**
 * Handles the logic when the projectile impacts another actor.
 * Applies damage and disables the projectile if appropriate.
 * @param Actor The actor that was impacted.
 */
void ABaseProjectile::ImpactBody(AActor* Actor)
{
	if (!HasAuthority() || Actor == GetOwner() || IsValid(Cast<ABaseProjectile>(Actor)) || IsValid(Cast<ABaseItem>(Actor)))
	{
		return;
	}
	
	Execute_DoDamage(this, Actor, Damage, FDamageEvent());
	Multicast_ProjectileOut(GetActorLocation(), GetActorRotation(), false);
}