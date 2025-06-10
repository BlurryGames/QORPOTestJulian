#include "ExplosiveBarrel.h"

AExplosiveBarrel::AExplosiveBarrel()
{
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(FName("RootComponent")));
	USceneComponent* MainSceneComponent = GetRootComponent();

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeshComponent"));
	MeshComponent->SetupAttachment(MainSceneComponent);

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(FName("CapsuleComponent"));
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CapsuleComponent->SetSimulatePhysics(true);
	CapsuleComponent->SetupAttachment(MeshComponent);

	ExplosionSphereComponent = CreateDefaultSubobject<USphereComponent>(FName("ExplosionSphereComponent"));
	ExplosionSphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ExplosionSphereComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	ExplosionSphereComponent->SetupAttachment(MeshComponent);

	ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(FName("ParticleComponent"));
	ParticleComponent->SetAutoActivate(false);
	ParticleComponent->bAllowRecycling = true;
	ParticleComponent->SetupAttachment(MainSceneComponent);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(FName("AudioComponent"));
	AudioComponent->SetAutoActivate(false);
	AudioComponent->SetupAttachment(MainSceneComponent);

	AttributesComponent = CreateDefaultSubobject<UAttributesComponent>(FName("AtrributesComponent"));

	RadialDamageEvent.Params = RadialDamageParameters;
}

float AExplosiveBarrel::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageResult = -1.0f;
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
		if (IsValid(CapsuleComponent))
		{
			CapsuleComponent->AddImpulseAtLocation((CurrentPosition - RadialPosition).GetSafeNormal() * DamageResult * DamageAmount, RadialPosition);
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
	if (IsValid(ExplosionSphereComponent))
	{
		RadialDamageParameters.OuterRadius = ExplosionSphereComponent->GetUnscaledSphereRadius();
		RadialDamageParameters.InnerRadius = RadialDamageParameters.OuterRadius * 0.5f;
		RadialDamageEvent.Params = RadialDamageParameters;
	}

	if (IsValid(AttributesComponent))
	{
		AttributesComponent->OnHealthChanged.AddUniqueDynamic(this, &AExplosiveBarrel::HandleHealthChange);
	}
}

void AExplosiveBarrel::HandleHealthChange(const float HealthResult)
{
	if (HealthResult > 0.0f)
	{
		return;
	}
	else if (IsValid(AudioComponent))
	{
		AudioComponent->Play();
	}

	RadialDamageEvent.Origin = IsValid(ExplosionSphereComponent) ? ExplosionSphereComponent->GetComponentLocation() : GetActorLocation();
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
	if (IsValid(ParticleComponent))
	{
		ParticleComponent->ActivateSystem(true);
	}
}