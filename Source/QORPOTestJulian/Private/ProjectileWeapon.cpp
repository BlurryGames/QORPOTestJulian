#include "ProjectileWeapon.h"

AProjectileWeapon::AProjectileWeapon() : Super()
{
	ProjectilesContainerComponent = CreateDefaultSubobject<USceneComponent>(FName("ProjectilesContainerComponent"));
	ProjectilesContainerComponent->SetAbsolute(true, true, true);
	ProjectilesContainerComponent->SetupAttachment(GetRootComponent());
}

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
			Projectile->AttachToComponent(ProjectilesContainerComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			Projectile->SetOwner(this);
		}
	}
}

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

bool AProjectileWeapon::HandleFire_Implementation()
{
	const bool bSucces = Super::HandleFire_Implementation() && !ProjectilesContainer.IsEmpty();
	if (!bSucces)
	{
		return bSucces;
	}
	else if (++CurrentIndex >= ProjectilesContainer.Num())
	{
		CurrentIndex = 0;
	}

	ABaseProjectile* Projectile = ProjectilesContainer[CurrentIndex];
	Projectile->SetActorLocationAndRotation(IsValid(MuzzleComponent) ? MuzzleComponent->GetComponentLocation() : GetActorLocation(),
		IsValid(MuzzleComponent) ? MuzzleComponent->GetComponentRotation() : GetActorRotation());
	Projectile->EnableProjectile(true);

	return bSucces;
}
