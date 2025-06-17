#include "BaseWeapon.h"
#include "ShooterPlayer.h"

ABaseWeapon::ABaseWeapon() : Super()
{
	MuzzleComponent = CreateDefaultSubobject<USceneComponent>(FName("MuzzleComponent"));
	MuzzleComponent->SetupAttachment(GetRootComponent());

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(FName("AudioComponent"));
	AudioComponent->SetAutoActivate(false);
	AudioComponent->SetupAttachment(MuzzleComponent);
}

void ABaseWeapon::SetOwner(AActor* NewOwner)
{
	SetEvents(GetOwner<AShooterPlayer>(), false);
	SetEvents(Cast<AShooterPlayer>(NewOwner));

	Super::SetOwner(NewOwner);
}

const FVector ABaseWeapon::GetAimPosition() const
{
	const FVector& Pivot = IsValid(MuzzleComponent) ? MuzzleComponent->GetComponentLocation() : GetActorLocation();
	const FVector& Direction = IsValid(MuzzleComponent) ? MuzzleComponent->GetForwardVector() : GetActorForwardVector();
	return Pivot + Direction * MaxRange;
}

const int ABaseWeapon::GetMagazine() const
{
	return Magazine;
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(CadencyTimerHandle, CadencyDelegate, CadencyTime, true);
}

void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(Cast<AShooterPlayer>(GetOwner())))
	{
		DrawDebugLine(GetWorld(), IsValid(MuzzleComponent) ? MuzzleComponent->GetComponentLocation() : GetActorLocation(), 
			GetAimPosition(), FColor::Red, false, -1.0f, 0, 1.0f);
	}
}

void ABaseWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	SetEvents(GetOwner<AShooterPlayer>());
}

void ABaseWeapon::OnInteract_Implementation(AActor* Caller)
{
	SetOwner(Caller);
	if (!IsValid(Caller))
	{
		GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
		bActiveTrigger = false;
		Execute_OnTurnEnabled(this, false);
		SetInstigator(nullptr);
	}
	else
	{
		bActiveAnimation = false;
		SetInstigator(Cast<APawn>(Caller));
	}
}

bool ABaseWeapon::SetEvents(AShooterPlayer* Player, const bool bConnect)
{
	bool success = IsValid(Player);
	if (success)
	{
		bConnect ? Player->OnShootHeld.AddUniqueDynamic(this, &ABaseWeapon::HandleShootHeld) : Player->OnShootHeld.RemoveAll(this);
		bConnect ? Player->OnReloadSpent.AddUniqueDynamic(this, &ABaseWeapon::HandleReloadSpent) : Player->OnReloadSpent.RemoveAll(this);
	}

	return success;
}

void ABaseWeapon::HandleReloadSpent(const int BulletsAmount)
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
	Magazine += ResultAmount;
	OnReloaded.Broadcast(ResultAmount);
	if (bActiveTrigger)
	{
		GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
		HandleShootHeld(true);
	}
}

bool ABaseWeapon::HandleFire_Implementation()
{
	FTimerManager& TimerManager = GetWorldTimerManager();
	const bool bSuccess = (bActiveTrigger || TimerManager.IsTimerActive(IntervalTimerHandle)) && Magazine > 0;
	if (!bSuccess)
	{
		TimerManager.PauseTimer(CadencyTimerHandle);
		TimerManager.ClearTimer(IntervalTimerHandle);
		IntervalCount = 0;

		return bSuccess;
	}

	const bool bInterval = IntervalProportionTime > 0.0f && ShotCost > 1;
	Magazine = FMath::Max(Magazine - (bInterval ? 1 : ShotCost), 0);
	OnReloaded.Broadcast(0);
	if (Magazine <= 0 || (bInterval && ++IntervalCount >= ShotCost))
	{
		TimerManager.ClearTimer(IntervalTimerHandle);
		IntervalCount = 0;
	}
	else if (bInterval && !TimerManager.IsTimerActive(IntervalTimerHandle))
	{
		TimerManager.SetTimer(IntervalTimerHandle, CadencyDelegate, (CadencyTime / ShotCost) * IntervalProportionTime, true);
	}

	if (IsValid(AudioComponent))
	{
		AudioComponent->Play();
	}

	return bSuccess;
}
