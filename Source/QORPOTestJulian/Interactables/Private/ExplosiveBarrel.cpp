// Copyright (c) Julián López Barañano. All Rights Reserved.

/**
 * @file ExplosiveBarrel.cpp
 * @brief Implements the logic for the AExplosiveBarrel class, which represents an interactable explosive barrel in the game world.
 *
 * This class handles damage processing, explosion effects, radial damage application, and networked state.
 * It is designed to be extended and supports both C++ and Blueprint customization.
 */

#include "../Public/ExplosiveBarrel.h"

/**
 * Default constructor.
 * Initializes all components, sets up collision, physics, and replication properties for the explosive barrel.
 */
AExplosiveBarrel::AExplosiveBarrel()
{
	SetReplicates(true);
	SetReplicateMovement(true);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeshComponent"));
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetCanEverAffectNavigation(false);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
	SetRootComponent(MeshComponent);

	ExplosionSphereComponent = CreateDefaultSubobject<USphereComponent>(FName("ExplosionSphereComponent"));
	ExplosionSphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ExplosionSphereComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	ExplosionSphereComponent->SetupAttachment(MeshComponent);

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(FName("CapsuleComponent"));
	CapsuleComponent->bDynamicObstacle = true;
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CapsuleComponent->SetupAttachment(MeshComponent);

	ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(FName("ParticleComponent"));
	ParticleComponent->SetIsReplicated(true);
	ParticleComponent->SetAutoActivate(false);
	ParticleComponent->bAllowRecycling = false;
	ParticleComponent->SetupAttachment(MeshComponent);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(FName("AudioComponent"));
	AudioComponent->SetAutoActivate(false);
	AudioComponent->SetupAttachment(MeshComponent);

	AttributesComponent = CreateDefaultSubobject<UAttributesComponent>(FName("AtrributesComponent"));

	RadialDamageEvent.Params = RadialDamageParameters;
}

/**
 * Processes incoming damage events for this barrel.
 * Handles different types of damage events (point, radial, generic) and updates health accordingly.
 * Applies physics impulses on radial damage.
 *
 * @param DamageAmount The amount of damage.
 * @param DamageEvent The event describing the type of damage.
 * @param EventInstigator The controller responsible for the event.
 * @param DamageCauser The actor that caused the event.
 * @return The final amount of damage applied.
 */
float AExplosiveBarrel::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageResult = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (!IsValid(AttributesComponent))
	{
		return DamageResult;
	}

	switch (DamageEvent.GetTypeID())
	{
	case FDamageEvent::ClassID:
	{
		DamageResult = -abs(DamageAmount);
		AttributesComponent->HealthReaction(-abs(DamageAmount));
		break;
	}
	case FPointDamageEvent::ClassID:
	{
		DamageResult = -abs(DamageAmount);
		AttributesComponent->HealthReaction(-abs(DamageAmount));
		break;
	}
	case FRadialDamageEvent::ClassID:
	{
		const FRadialDamageEvent& RadialEvent = static_cast<const FRadialDamageEvent&>(DamageEvent);
		const FVector& CurrentPosition = GetActorLocation();
		const FVector& RadialPosition = RadialEvent.Origin;
		DamageResult = -abs(RadialEvent.Params.GetDamageScale(FVector::Distance(CurrentPosition, RadialPosition)));
		AttributesComponent->HealthReaction(DamageResult);
		if (IsValid(MeshComponent))
		{
			MeshComponent->AddImpulseAtLocation((CurrentPosition - RadialPosition).GetSafeNormal() * DamageResult * DamageAmount, RadialPosition);
		}
		break;
	}
	default:
		break;
	}

	return DamageResult;
}

/**
 * Called when the game starts or when spawned.
 * Initializes enabled types, sets up explosion parameters, and binds health change events.
 */
void AExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();

	Execute_AddEnabledType(this, MeshComponent);
	Execute_AddEnabledType(this, CapsuleComponent);

	Execute_SetOriginalPositionAndRotation(this, GetActorLocation(), GetActorRotation());
	if (IsValid(ExplosionSphereComponent))
	{
		RadialDamageParameters.OuterRadius = ExplosionSphereComponent->GetUnscaledSphereRadius();
		RadialDamageParameters.InnerRadius = RadialDamageParameters.OuterRadius * 0.5f;
		RadialDamageEvent.Params = RadialDamageParameters;
	}

	if (IsValid(AttributesComponent))
	{
		AttributesComponent->OnHealthChanged.AddUniqueDynamic(this, &AExplosiveBarrel::HandleHealthChanged);
	}
}

/**
 * Called when the barrel is removed from the world.
 * Clears all timers associated with this barrel.
 *
 * @param EndPlayReason The reason for removal.
 */
void AExplosiveBarrel::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearAllTimersForObject(this);
}

/**
 * Handles changes in the barrel's health.
 * Triggers explosion effects, applies radial damage to overlapping actors, and schedules barrel disappearance.
 *
 * @param HealthResult The new health value.
 * @param TotalHealth The maximum health value.
 */
void AExplosiveBarrel::HandleHealthChanged(const float HealthResult, const float TotalHealth)
{
	if (HealthResult > 0.0f)
	{
		return;
	}
	else if (IsValid(AudioComponent))
	{
		AudioComponent->Play();
	}

	if (IsValid(ParticleComponent))
	{
		ParticleComponent->ActivateSystem(true);
	}

	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(DissapearTimerHandle, this, &AExplosiveBarrel::Multicast_HandleDissapear, DissapearTime, false);
	}

	if (!IsValid(ExplosionSphereComponent))
	{
		return;
	}

	RadialDamageEvent.Origin = ExplosionSphereComponent->GetComponentLocation();
	ExplosionSphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ExplosionSphereComponent->GetOverlappingActors(ExplosionOverlaps);
	for (AActor* A : ExplosionOverlaps)
	{
		if (A != this && IsValid(A))
		{
			A->TakeDamage(ImpulseMultiplier, RadialDamageEvent, GetInstigatorController(), this);
		}
	}

	ExplosionSphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

/**
 * Multicast function to handle the barrel's disappearance after explosion.
 * Disables the barrel for all clients.
 */
void AExplosiveBarrel::Multicast_HandleDissapear_Implementation()
{
	Execute_OnTurnEnabled(this, false);
}
