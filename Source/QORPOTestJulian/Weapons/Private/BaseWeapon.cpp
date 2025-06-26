// Copyright (c) Julián López Barañano. All Rights Reserved.

/**
 * @file BaseWeapon.cpp
 * @brief Implements the logic for the ABaseWeapon class, which represents a networked, reusable weapon actor in the game world.
 *
 * This class handles weapon initialization, firing, reloading, magazine management, owner assignment, and network replication.
 * It is designed to be extended for custom weapon behavior and supports both C++ and Blueprint customization.
 */

#include "../Public/BaseWeapon.h"
#include "../../Characters/Public/ShooterPlayer.h"

/**
 * Default constructor.
 * Initializes components, sets up the muzzle and audio for the weapon.
 */
ABaseWeapon::ABaseWeapon() : Super()
{
	MuzzleComponent = CreateDefaultSubobject<USceneComponent>(FName("MuzzleComponent"));
	MuzzleComponent->SetupAttachment(GetRootComponent());

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(FName("AudioComponent"));
	AudioComponent->SetAutoActivate(false);
	AudioComponent->SetupAttachment(MuzzleComponent);
}

/**
 * Sets the owner of the weapon and manages event bindings for the previous and new owner.
 * @param NewOwner The new owner actor.
 */
void ABaseWeapon::SetOwner(AActor* NewOwner)
{
	SetEvents(GetOwner<AShooterPlayer>(), false);
	SetEvents(Cast<AShooterPlayer>(NewOwner));

	Super::SetOwner(NewOwner);
}

/**
 * Called when the weapon is spawned or the game starts.
 * Initializes the firing cadence timer.
 */
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(CadencyTimerHandle, CadencyDelegate, CadencyTime, true);
}

/**
 * Registers properties for network replication.
 * @param OutLifetimeProps The array to add replicated properties to.
 */
void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseWeapon, Magazine);
	DOREPLIFETIME(ABaseWeapon, IntervalCount);
	DOREPLIFETIME(ABaseWeapon, bActiveTrigger);
}

/**
 * Called every frame.
 * Draws debug lines for aiming if the owner is a valid player.
 * @param DeltaTime Time elapsed since the last tick.
 */
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(Cast<AShooterPlayer>(GetOwner())))
	{
		DrawDebugLine(GetWorld(), IsValid(MuzzleComponent) ? MuzzleComponent->GetComponentLocation() : GetActorLocation(), 
			GetAimPosition(), FColor::Red, false, -1.0f, 0, 1.0f);
	}
}

/**
 * Called when the weapon is removed from the world.
 * Unbinds events from the owner.
 * @param EndPlayReason The reason for removal.
 */
void ABaseWeapon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	SetEvents(GetOwner<AShooterPlayer>());
}

/**
 * Returns the world position the weapon is aiming at, based on the muzzle's forward vector and max range.
 * @return The aim position as an FVector.
 */
const FVector ABaseWeapon::GetAimPosition() const
{
	const FVector& Pivot = IsValid(MuzzleComponent) ? MuzzleComponent->GetComponentLocation() : GetActorLocation();
	const FVector& Direction = IsValid(MuzzleComponent) ? MuzzleComponent->GetForwardVector() : GetActorForwardVector();
	return Pivot + Direction * MaxRange;
}

/**
 * Returns the current number of bullets in the magazine.
 * @return The magazine count.
 */
const int ABaseWeapon::GetMagazine() const
{
	return Magazine;
}

/**
 * Handles interaction with the weapon (e.g., when picked up by a player).
 * Sets the owner, disables collision, and resets relevant states.
 * @param Caller The actor that initiated the interaction.
 */
void ABaseWeapon::OnInteract_Implementation(AActor* Caller)
{
	SetOwner(Caller);
	if (!IsValid(Caller))
	{
		GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
		bActiveTrigger = false;
		Execute_OnTurnEnabled(this, false);
		SetInstigator(nullptr);

		return;
	}

	bActiveAnimation = false;
	SetInstigator(Cast<APawn>(Caller));
	SetActorEnableCollision(false);
	if (IsValid(MeshComponent))
	{
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

/**
 * Binds or unbinds weapon events to the specified player.
 * @param Player The player to bind/unbind events to.
 * @param bConnect True to bind, false to unbind.
 * @return True if the player is valid and events were set.
 */
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

/**
 * Handles the firing logic for the weapon.
 * Manages magazine count, firing cadence, and triggers the fire mechanism.
 * @return True if the weapon fired successfully.
 */
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

	Multicast_FireMechanism();

	return bSuccess;
}

/**
 * Handles the reload logic when the player requests a reload.
 * Starts the reload timer and prepares the reload delegate.
 * @param BulletsAmount The number of bullets to reload.
 */
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

/**
 * Handles the shoot held event, managing trigger state and firing cadence.
 * @param bHold True if the shoot button is held, false otherwise.
 */
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

/**
 * Completes the reload process, updating the magazine count and resuming firing if needed.
 * @param BulletsAmount The number of bullets to reload.
 */
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

/**
 * Multicast function to play the fire mechanism (e.g., sound) across the network.
 */
void ABaseWeapon::Multicast_FireMechanism_Implementation()
{
	if (IsValid(AudioComponent))
	{
		AudioComponent->Play();
	}
}