#include "BaseProjectile.h"
#include "BaseWeapon.h"

ABaseProjectile::ABaseProjectile()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeshComponent"));
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

void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();

	Execute_AddEnabledType(this, MeshComponent);

	Execute_OnTurnEnabled(this, false);
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

void ABaseProjectile::OnTurnEnabled_Implementation(const bool bEnabled)
{
	IReusableInterface::OnTurnEnabled_Implementation(bEnabled);

	if (IsValid(ProjectileMovementComponent))
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

void ABaseProjectile::ImpactBody(AActor* Actor)
{
	if (Actor == GetOwner() || IsValid(Cast<ABaseProjectile>(Actor)) || IsValid(Cast<ABaseItem>(Actor)))
	{
		return;
	}
	else if (IsValid(Actor))
	{
		Actor->TakeDamage(Damage, FDamageEvent(), GetInstigatorController(), this);
	}

	Execute_OnTurnEnabled(this, false);
}