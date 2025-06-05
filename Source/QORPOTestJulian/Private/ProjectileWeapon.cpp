#include "ProjectileWeapon.h"

void AProjectileWeapon::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!IsValid(World) || !IsValid(ProjectileClass))
	{
		return;
	}
	else if (ProjectilesContainer.IsEmpty())
	{
		ProjectilesContainer.Init(nullptr, MagazineCapacity);
	}

	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
	SpawnParameters.Owner = this;
	const FVector& SpawnPosition = GetActorLocation();
	const FRotator& SpawnRotation = GetActorRotation();
	for (unsigned short i = 0; i < ProjectilesContainer.Num(); i++)
	{
		if (!IsValid(ProjectilesContainer[i]))
		{
			ABaseProjectile* Projectile = World->SpawnActor<ABaseProjectile>(ProjectileClass, SpawnPosition, SpawnRotation, SpawnParameters);
			ProjectilesContainer[i] = Projectile;
			Projectile->SetOwner(this);
		}
	}
}

void AProjectileWeapon::OnPickup_Implementation(AShooterPlayer* Caller)
{
	Super::OnPickup_Implementation(Caller);
	for (ABaseProjectile* Projectile : ProjectilesContainer)
	{
		if (IsValid(Projectile))
		{
			Projectile->SetOwner(GetOwner());
		}
	}
}

void AProjectileWeapon::HandleFire_Implementation()
{
	if (bActiveTrigger && Magazine > 0 && !ProjectilesContainer.IsEmpty())
	{
		if (++CurrentIndex >= ProjectilesContainer.Num())
		{
			CurrentIndex = 0;
		}

		ABaseProjectile* Projectile = ProjectilesContainer[CurrentIndex];
		Projectile->SetActorLocationAndRotation(IsValid(MuzzleComponent) ? MuzzleComponent->GetComponentLocation() : GetActorLocation(),
			IsValid(MuzzleComponent) ? MuzzleComponent->GetComponentRotation() : GetActorRotation());
		Projectile->EnableProjectile(true);
	}

	Super::HandleFire_Implementation();
}
