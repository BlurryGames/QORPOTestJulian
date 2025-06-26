// Copyright (c) Julián López Barañano. All Rights Reserved.

/**
 * @file ShooterPlayer.cpp
 * @brief Implements the logic for the AShooterPlayer class, representing the main player character in the game.
 *
 * This class handles player input, movement, health, weapon management, interaction, and network replication.
 * It is designed to be extended and supports both C++ and Blueprint customization.
 */

#include "../Public/ShooterPlayer.h"
#include "../../Core/Public/ShooterPlayerController.h"
#include "../../Weapons/Public/BaseWeapon.h"
#include "../../Interactables/Public/Door.h"

/**
 * Default constructor.
 * Initializes components, sets up movement and replication, and configures collision and trace parameters.
 */
AShooterPlayer::AShooterPlayer() : Super()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = true;
	SetReplicates(true);
	SetReplicateMovement(true);

	UCharacterMovementComponent& MovementComponent = *GetCharacterMovement();
	DefaultSpeed = MovementComponent.MaxWalkSpeed;
	MovementComponent.bOrientRotationToMovement = false;
	MovementComponent.GetNavAgentPropertiesRef().bCanCrouch = true;

	WeaponSocketComponent = CreateDefaultSubobject<USceneComponent>(FName("WeaponSocketComponent"));
	WeaponSocketComponent->SetupAttachment(GetRootComponent());

	AttributesComponent = CreateDefaultSubobject<UAttributesComponent>(FName("AttributesComponent"));

	ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectParams.AddObjectTypesToQuery(ECC_PhysicsBody);
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
	LineTraceParams.AddIgnoredActor(this);
}

/**
 * Handles incoming damage or healing events for the player.
 * Updates health and applies radial impulses if necessary.
 *
 * @param DamageAmount The amount of damage or healing.
 * @param DamageEvent The event describing the type of damage or healing.
 * @param EventInstigator The controller responsible for the event.
 * @param DamageCauser The actor that caused the event.
 * @return The final amount of damage or healing applied.
 */
float AShooterPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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
		DamageResult = abs(RadialEvent.Params.BaseDamage * RadialEvent.Params.GetDamageScale(FVector::Distance(CurrentPosition, RadialPosition)));
		AttributesComponent->HealthReaction(-DamageResult);
		UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
		if (IsValid(MovementComponent))
		{
			MovementComponent->AddRadialImpulse(RadialPosition, RadialEvent.Params.GetMaxRadius(), DamageResult * DamageAmount, RIF_Linear, true);
		}
		break;
	}
	case FHealEvent::ClassID:
	{
		const FHealEvent& HealEvent = static_cast<const FHealEvent&>(DamageEvent);
		DamageResult = AttributesComponent->HealthReaction(abs(DamageAmount)) ? HealEvent.HealSuccess : DamageAmount;
		break;
	}
	default:
		break;
	}

	return DamageResult;
}

/**
 * Sets up player input bindings for movement, actions, and interactions.
 *
 * @param PlayerInputComponent The input component to bind actions and axes to.
 */
void AShooterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxisKey(EKeys::MouseX, this, &AShooterPlayer::AddControllerYawInput);
	PlayerInputComponent->BindAxisKey(EKeys::MouseY, this, &AShooterPlayer::AddControllerPitchInput);

	PlayerInputComponent->BindKey(EKeys::W, IE_Pressed, this, &AShooterPlayer::HandleMoveForward);
	PlayerInputComponent->BindKey(EKeys::S, IE_Pressed, this, &AShooterPlayer::HandleMoveBackward);
	PlayerInputComponent->BindKey(EKeys::D, IE_Pressed, this, &AShooterPlayer::HandleMoveRight);
	PlayerInputComponent->BindKey(EKeys::A, IE_Pressed, this, &AShooterPlayer::HandleMoveLeft);
	PlayerInputComponent->BindKey(EKeys::LeftShift, IE_Pressed, this, &AShooterPlayer::HandleSprint);
	PlayerInputComponent->BindKey(EKeys::C, IE_Pressed, this, &AShooterPlayer::HandleCrouch);
	PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &AShooterPlayer::HandleJump);
	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AShooterPlayer::HandleStartShoot);
	PlayerInputComponent->BindKey(EKeys::R, IE_Pressed, this, &AShooterPlayer::HandleReload);
	PlayerInputComponent->BindKey(EKeys::I, IE_Pressed, this, &AShooterPlayer::HandleInteraction);

	PlayerInputComponent->BindKey(EKeys::I, IE_Repeat, this, &AShooterPlayer::HandleInteraction);

	PlayerInputComponent->BindKey(EKeys::W, IE_Released, this, &AShooterPlayer::HandleMoveBackward);
	PlayerInputComponent->BindKey(EKeys::S, IE_Released, this, &AShooterPlayer::HandleMoveForward);
	PlayerInputComponent->BindKey(EKeys::D, IE_Released, this, &AShooterPlayer::HandleMoveLeft);
	PlayerInputComponent->BindKey(EKeys::A, IE_Released, this, &AShooterPlayer::HandleMoveRight);
	PlayerInputComponent->BindKey(EKeys::LeftShift, IE_Released, this, &AShooterPlayer::HandleSprint);
	PlayerInputComponent->BindKey(EKeys::C, IE_Released, this, &AShooterPlayer::HandleCrouch);
	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &AShooterPlayer::HandleStopShoot);
}

/**
 * Called when the game starts or when spawned.
 * Binds health change events.
 */
void AShooterPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(AttributesComponent))
	{
		AttributesComponent->OnHealthChanged.AddUniqueDynamic(this, &AShooterPlayer::HandleHealthChange);
	}
}

/**
 * Registers properties for network replication.
 *
 * @param OutLifetimeProps The array to add replicated properties to.
 */
void AShooterPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterPlayer, Ammunition);
}

/**
 * Called every frame.
 * Handles movement input and draws debug lines for interaction.
 *
 * @param DeltaTime Time elapsed since the last tick.
 */
void AShooterPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddMovementInput(GetMovementDirection());

	FVector PivotPosition = IsValid(WeaponSocketComponent) ? WeaponSocketComponent->GetComponentLocation() : GetActorLocation();
	DrawDebugLine(GetWorld(), PivotPosition,
		PivotPosition + (IsValid(WeaponSocketComponent) ? WeaponSocketComponent->GetForwardVector() : GetActorForwardVector()) * InteractionRange, 
		FColor::Black);
}

/**
 * Called when another actor begins to overlap with this player.
 * Attempts to equip a weapon if the overlapping actor is a weapon.
 *
 * @param OtherActor The actor that started overlapping.
 */
void AShooterPlayer::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	OnEquipWeapon(Cast<ABaseWeapon>(OtherActor));
}

/**
 * Adds pitch input to the controller, considering player settings for inverted pitch.
 *
 * @param Value The pitch input value.
 */
void AShooterPlayer::AddControllerPitchInput(float Value)
{
	const AShooterPlayerController* PlayerController = GetController<AShooterPlayerController>();
	Super::AddControllerPitchInput(IsValid(PlayerController) && PlayerController->GetInvertPitch() ? Value : -Value);

	Server_UpdatePitchView(GetControlRotation().Pitch);
}

/**
 * Called when the player is removed from the world.
 * Handles weapon unequip logic.
 *
 * @param EndPlayReason The reason for removal.
 */
void AShooterPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	OnUnequipWeapon();
}

/**
 * Returns the attributes component for this player.
 *
 * @return The attributes component.
 */
UAttributesComponent* AShooterPlayer::GetAttributesComponent() const
{
	return AttributesComponent;
}

/**
 * Returns the current amount of ammunition.
 *
 * @return The ammunition count.
 */
const int AShooterPlayer::GetAmmunition() const
{
	return Ammunition;
}

/**
 * Adds ammunition to the player and broadcasts the update.
 *
 * @param Amount The amount of ammunition to add.
 */
void AShooterPlayer::AddAmmunition(const int Amount)
{
	const int CurrentAmmunition = Ammunition;
	Ammunition = FMath::Clamp(Ammunition + abs(Amount), 0, INT_MAX);
	OnPlayerAmmunitionUpdated.Broadcast(Ammunition);
}

/**
 * Calculates the current movement direction based on input.
 *
 * @return The movement direction vector.
 */
FVector AShooterPlayer::GetMovementDirection() const
{
	return GetActorForwardVector() * MovementDirection.X + GetActorRightVector() * MovementDirection.Y;
}

/**
 * Equips the specified weapon, attaching it to the player and binding events.
 *
 * @param Weapon The weapon to equip.
 */
void AShooterPlayer::OnEquipWeapon_Implementation(ABaseWeapon* Weapon)
{
	if (IsValid(Weapon) && Weapon != CurrentWeapon)
	{
		OnUnequipWeapon();
		Weapon->AttachToComponent(IsValid(WeaponSocketComponent) ? WeaponSocketComponent : GetRootComponent(), 
			FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		Weapon->Execute_OnInteract(Weapon, this);
		Weapon->OnReloaded.AddUniqueDynamic(this, &AShooterPlayer::HandleReloaded);
		CurrentWeapon = Weapon;
		LineTraceParams.AddIgnoredActor(Weapon);
		HandleReloaded(0);
	}
}

/**
 * Unequips the current weapon, detaching it and unbinding events.
 */
void AShooterPlayer::OnUnequipWeapon_Implementation()
{
	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentWeapon->Execute_OnInteract(CurrentWeapon, nullptr);
		CurrentWeapon->OnReloaded.RemoveAll(this);
		CurrentWeapon = nullptr;
		LineTraceParams.ClearIgnoredSourceObjects();
		LineTraceParams.AddIgnoredActor(this);
		HandleReloaded(0);
	}
}

/**
 * Called when the ammunition value is replicated.
 * Broadcasts the updated ammunition value.
 */
void AShooterPlayer::OnReplicateAmmunition()
{
	OnPlayerAmmunitionUpdated.Broadcast(Ammunition);
}

/**
 * Handles changes in the player's health.
 * Destroys the player if health reaches zero.
 *
 * @param HealthResult The new health value.
 * @param TotalHealth The maximum health value.
 */
void AShooterPlayer::HandleHealthChange(const float HealthResult, const float TotalHealth)
{
	if (HealthResult <= 0.0f)
	{
		Destroy();
	}
}

/**
 * Handles weapon reload events, updating ammunition and broadcasting changes.
 *
 * @param AmmunitionSpent The amount of ammunition spent during reload.
 */
void AShooterPlayer::HandleReloaded(const int AmmunitionSpent)
{
	const int CurrentAmmunition = FMath::Max(Ammunition - abs(AmmunitionSpent), 0);
	if (CurrentAmmunition != Ammunition)
	{
		Ammunition = CurrentAmmunition;
		OnPlayerAmmunitionUpdated.Broadcast(CurrentAmmunition);
	}

	if (IsValid(CurrentWeapon))
	{
		OnWeaponMagazineUpdated.Broadcast(CurrentWeapon->GetMagazine());
	}
}

/**
 * Handles forward movement input.
 */
void AShooterPlayer::HandleMoveForward()
{
	MovementDirection.X += 1.0f;
}

/**
 * Handles backward movement input.
 */
void AShooterPlayer::HandleMoveBackward()
{
	MovementDirection.X -= 1.0f;
}

/**
 * Handles right movement input.
 */
void AShooterPlayer::HandleMoveRight()
{
	MovementDirection.Y += 1.0f;
}

/**
 * Handles left movement input.
 */
void AShooterPlayer::HandleMoveLeft()
{
	MovementDirection.Y -= 1.0f;
}

/**
 * Handles sprint input, toggling between default and sprint speed.
 */
void AShooterPlayer::HandleSprint()
{
	float& MaxSpeed = GetCharacterMovement()->MaxWalkSpeed;
	MaxSpeed = FMath::IsNearlyEqual(MaxSpeed, DefaultSpeed) ? DefaultSpeed * SprintMultiplier : DefaultSpeed;
}

/**
 * Handles crouch input, toggling crouch state.
 */
void AShooterPlayer::HandleCrouch()
{
	bIsCrouched ? UnCrouch() : Crouch();
}

/**
 * Handles jump input.
 */
void AShooterPlayer::HandleJump()
{
	Jump();
}

/**
 * Handles the start of shooting input.
 * Broadcasts the shoot held event.
 */
void AShooterPlayer::HandleStartShoot_Implementation()
{
	OnShootHeld.Broadcast(true);
}

/**
 * Handles the stop of shooting input.
 * Broadcasts the shoot held event.
 */
void AShooterPlayer::HandleStopShoot_Implementation()
{
	OnShootHeld.Broadcast(false);
}

/**
 * Handles reload input.
 * Broadcasts the reload spent event.
 */
void AShooterPlayer::HandleReload_Implementation()
{
	OnReloadSpent.Broadcast(Ammunition);
}

/**
 * Handles interaction input, performing a line trace and interacting with doors if hit.
 */
void AShooterPlayer::HandleInteraction()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	FVector PivotPosition = IsValid(WeaponSocketComponent) ? WeaponSocketComponent->GetComponentLocation() : GetActorLocation();
	bool bHit = World->LineTraceSingleByObjectType(LineTraceHitInteraction, PivotPosition, 
		PivotPosition + (IsValid(WeaponSocketComponent) ? WeaponSocketComponent->GetForwardVector() : GetActorForwardVector()) * InteractionRange, 
		ObjectParams, LineTraceParams);
	if (bHit)
	{
		Server_DoorInteraction(Cast<ADoor>(LineTraceHitInteraction.GetActor()));
	}
}

/**
 * Server-side implementation for interacting with a door.
 *
 * @param Door The door to interact with.
 */
void AShooterPlayer::Server_DoorInteraction_Implementation(ADoor* Door)
{
	if (IsValid(Door))
	{
		Door->Execute_OnInteract(Door, this);
	}
}

/**
 * Server-side implementation for updating the pitch view.
 *
 * @param PitchInput The pitch value to update.
 */
void AShooterPlayer::Server_UpdatePitchView_Implementation(const float PitchInput)
{
	Multicast_UpdatePitchView(PitchInput);
}

/**
 * Multicast implementation for updating the pitch view on all clients.
 *
 * @param PitchInput The pitch value to update.
 */
void AShooterPlayer::Multicast_UpdatePitchView_Implementation(const float PitchInput)
{
	if (IsValid(WeaponSocketComponent))
	{
		WeaponSocketComponent->SetRelativeRotation(FRotator(PitchInput, 0.0f, 0.0f));
	}
}