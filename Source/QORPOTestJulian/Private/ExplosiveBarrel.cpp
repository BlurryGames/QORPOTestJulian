#include "ExplosiveBarrel.h"

AExplosiveBarrel::AExplosiveBarrel()
{
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
	ParticleComponent->SetAutoActivate(false);
	ParticleComponent->bAllowRecycling = true;
	ParticleComponent->SetupAttachment(MeshComponent);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(FName("AudioComponent"));
	AudioComponent->SetAutoActivate(false);
	AudioComponent->SetupAttachment(MeshComponent);

	AttributesComponent = CreateDefaultSubobject<UAttributesComponent>(FName("AtrributesComponent"));

	RadialDamageEvent.Params = RadialDamageParameters;
}

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

	if (IsValid(ParticleComponent))
	{
		ParticleComponent->OnSystemFinished.AddUniqueDynamic(this, &AExplosiveBarrel::HandleSystemFinished);
	}

	if (IsValid(AttributesComponent))
	{
		AttributesComponent->OnHealthChanged.AddUniqueDynamic(this, &AExplosiveBarrel::HandleHealthChanged);
	}
}

void AExplosiveBarrel::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (IsValid(ParticleComponent))
	{
		ParticleComponent->OnSystemFinished.RemoveAll(this);
	}
}

void AExplosiveBarrel::HandleSystemFinished_Implementation(UParticleSystemComponent* ParticleSystem)
{
	Execute_OnTurnEnabled(this, false);
}

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

	IsValid(ParticleComponent) && IsValid(ParticleComponent->Template) ? ParticleComponent->ActivateSystem(true) : Execute_OnTurnEnabled(this, false);
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