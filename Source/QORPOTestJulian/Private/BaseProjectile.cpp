#include "BaseProjectile.h"
#include "BaseWeapon.h"

ABaseProjectile::ABaseProjectile()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeshComponent"));
	MeshComponent->SetCollisionProfileName(FName("OverlapAllDynamic"));
	MeshComponent->SetNotifyRigidBodyCollision(true);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	MeshComponent->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
	SetRootComponent(MeshComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovementComponent"));
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	const float& Speed = ProjectileMovementComponent->MaxSpeed = 1000.0f;
	ProjectileMovementComponent->InitialSpeed = Speed;
	ProjectileMovementComponent->SetVelocityInLocalSpace(FVector::ForwardVector);
}

void ABaseProjectile::SetOwner(AActor* NewOwner)
{
	AActor* OldOwner = GetOwner();
	Super::SetOwner(NewOwner);
	if (!IsValid(MeshComponent))
	{
		return;
	}

	if (!IsValid(Cast<ABaseWeapon>(OldOwner)))
	{
		MeshComponent->IgnoreActorWhenMoving(OldOwner, false);
	}

	MeshComponent->IgnoreActorWhenMoving(NewOwner, true);
}

void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();
	EnableProjectile(false);
}

void ABaseProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	ImpactBody(OtherActor);
}

void ABaseProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, OtherActor, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	ImpactBody(OtherActor);
}

void ABaseProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearTimer(LifeTimeHandle);
}

void ABaseProjectile::ImpactBody(AActor* Actor)
{
	if (Actor == GetOwner() || IsValid(Cast<ABaseProjectile>(Actor)) || IsValid(Cast<ABaseWeapon>(Actor)))
	{
		return;
	}

	UAttributesComponent* AttributesComponent = IsValid(Actor) ? Actor->FindComponentByClass<UAttributesComponent>() : nullptr;
	if (IsValid(AttributesComponent))
	{
		AttributesComponent->HealthReaction(Damage);
	}

	EnableProjectile(false);
}

void ABaseProjectile::EnableProjectile(const bool bEnable)
{
	SetActorEnableCollision(bEnable);
	SetHidden(!bEnable);
	if (IsValid(ProjectileMovementComponent))
	{
		ProjectileMovementComponent->StopMovementImmediately();
		ProjectileMovementComponent->SetVelocityInLocalSpace(FVector(ProjectileMovementComponent->InitialSpeed, 0.0f, 0.0f));
		ProjectileMovementComponent->SetActive(bEnable);
	}

	if (IsValid(MeshComponent))
	{
		MeshComponent->SetVisibility(bEnable);
		MeshComponent->SetCollisionEnabled(bEnable ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	}
	
	FTimerManager& TimerManager = GetWorldTimerManager();
	if (bEnable && !TimerManager.IsTimerActive(LifeTimeHandle))
	{
		TimerManager.SetTimer(LifeTimeHandle, LifeTimeDelegate, LifeTime, false);
	}
	else if (!bEnable && TimerManager.IsTimerActive(LifeTimeHandle))
	{
		TimerManager.ClearTimer(LifeTimeHandle);
	}
}