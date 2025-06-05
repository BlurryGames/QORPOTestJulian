#include "BaseWeapon.h"

ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(FName("RootComponent")));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeshComponent"));
	MeshComponent->SetCollisionProfileName(FName("OverlapAllDynamic"));
	MeshComponent->SetupAttachment(RootComponent);

	MuzzleComponent = CreateDefaultSubobject<USceneComponent>(FName("MuzzleComponent"));
	MuzzleComponent->SetupAttachment(RootComponent);
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	FTimerManager& TimerManager = GetWorldTimerManager();
	TimerManager.SetTimer(CadencyTimerHandle, CadencyDelegate, Cadency, true);
}

void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsValid(MuzzleComponent))
	{
		const FVector& PivotPosition = MuzzleComponent->GetComponentLocation();
		DrawDebugLine(GetWorld(), PivotPosition, PivotPosition + MuzzleComponent->GetForwardVector() * MaxRange, FColor::Red, false, -1.0f, 0, 1.0f);
	}
}

void ABaseWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorldTimerManager().ClearAllTimersForObject(this);
	SetEvents(GetOwner<AShooterPlayer>());
}

void ABaseWeapon::OnPickup_Implementation(AShooterPlayer* Caller)
{
	if (SetEvents(Caller))
	{
		SetOwner(Caller);
	}
}

void ABaseWeapon::OnDrop_Implementation()
{
	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
	bActiveTrigger = false;
	SetEvents(GetOwner<AShooterPlayer>(), false);
	SetOwner(nullptr);
}

bool ABaseWeapon::SetEvents(AShooterPlayer* Player, const bool bConnect)
{
	bool success = IsValid(Player);
	if (success)
	{
		bConnect ? Player->OnShootHeld.AddUniqueDynamic(this, &ABaseWeapon::HandleShootHeld) : Player->OnShootHeld.RemoveDynamic(this, &ABaseWeapon::HandleShootHeld);
		bConnect ? Player->OnReloaded.RemoveDynamic(this, &ABaseWeapon::HandleRealoaded) : Player->OnReloaded.RemoveDynamic(this, &ABaseWeapon::HandleRealoaded);
	}

	return success;
}

void ABaseWeapon::HandleRealoaded(const int BulletsAmount)
{
	FTimerManager& TimerManager = GetWorldTimerManager();
	if (BulletsAmount < 1 || Magazine >= MagazineCapacity || TimerManager.IsTimerActive(ReloadTimerHandle))
	{
		return;
	}

	ReloadDelegate.Unbind();
	ReloadDelegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(ABaseWeapon, HandleReloadCompleted), BulletsAmount);
	TimerManager.SetTimer(ReloadTimerHandle, ReloadDelegate, ReloadTime, false);
	bActiveTrigger = false;
}

void ABaseWeapon::HandleShootHeld(const bool bHold)
{
	FTimerManager& TimerManager = GetWorldTimerManager();
	if (!bHold || Magazine < 1)
	{
		bActiveTrigger = false;
		return;
	}
	else if (TimerManager.IsTimerActive(ReloadTimerHandle))
	{
		bActiveTrigger = true;
		return;
	}

	bActiveTrigger = true;
	if (TimerManager.IsTimerPaused(CadencyTimerHandle))
	{
		TimerManager.UnPauseTimer(CadencyTimerHandle);
	}
}

void ABaseWeapon::HandleReloadCompleted_Implementation(const int BullettsAmount)
{
	const int ResultAmount = FMath::Min(BullettsAmount, MagazineCapacity - Magazine);
	OnReloadSpent.Broadcast(ResultAmount);
	Magazine += ResultAmount;
	if (bActiveTrigger)
	{
		GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
		HandleShootHeld(true);
	}
}

void ABaseWeapon::HandleFire_Implementation()
{
	FTimerManager& TimerManager = GetWorldTimerManager();
	if (!bActiveTrigger)
	{
		TimerManager.PauseTimer(CadencyTimerHandle);
	}
	else if ((Magazine = FMath::Max(Magazine - ShotCost, 0)) <= 0)
	{
		TimerManager.PauseTimer(CadencyTimerHandle);
		bActiveTrigger = false;
	}
}
