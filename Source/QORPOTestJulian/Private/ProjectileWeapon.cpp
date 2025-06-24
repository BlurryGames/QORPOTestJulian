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