#include "ShooterPlayer.h"
#include "BaseWeapon.h"
#include "Door.h"

AShooterPlayer::AShooterPlayer() : Super()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = true;

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
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Black, FString::FromInt(RadialEvent.Params.GetMaxRadius()));
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

void AShooterPlayer::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(AttributesComponent))
	{
		AttributesComponent->OnHealthChanged.AddUniqueDynamic(this, &AShooterPlayer::HandleHealthChange);
	}
}

void AShooterPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AddMovementInput(GetMovementDirection());

	FVector PivotPosition = IsValid(WeaponSocketComponent) ? WeaponSocketComponent->GetComponentLocation() : GetActorLocation();
	DrawDebugLine(GetWorld(), PivotPosition,
		PivotPosition + (IsValid(WeaponSocketComponent) ? WeaponSocketComponent->GetForwardVector() : GetActorForwardVector()) * InteractionRange, 
		FColor::Black);
}

void AShooterPlayer::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	EquipWeapon(Cast<ABaseWeapon>(OtherActor));
}

void AShooterPlayer::AddControllerPitchInput(float Value)
{
	const AShooterPlayerController* PlayerController = GetController<AShooterPlayerController>();
	Super::AddControllerPitchInput(IsValid(PlayerController) && PlayerController->GetInvertPitch() ? Value : -Value);
	if (IsValid(WeaponSocketComponent))
	{
		WeaponSocketComponent->SetRelativeRotation(FRotator(GetControlRotation().Pitch, 0.0f, 0.0f));
	}
}

void AShooterPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UnequipWeapon();
	AttributesComponent->OnHealthChanged.RemoveDynamic(this, &AShooterPlayer::HandleHealthChange);
}

void AShooterPlayer::AddAmmunition(const int Amount)
{
	Ammunition = FMath::Clamp(Ammunition + abs(Amount), 0, INT_MAX);
}

FVector AShooterPlayer::GetMovementDirection() const
{
	return GetActorForwardVector() * MovementDirection.X + GetActorRightVector() * MovementDirection.Y;
}

void AShooterPlayer::EquipWeapon_Implementation(ABaseWeapon* Weapon)
{
	if (IsValid(Weapon) && Weapon != CurrentWeapon)
	{
		UnequipWeapon();
		Weapon->AttachToComponent(IsValid(WeaponSocketComponent) ? WeaponSocketComponent : GetRootComponent(), 
			FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		Weapon->Execute_OnInteract(Weapon, this);
		Weapon->OnReloadSpent.AddUniqueDynamic(this, &AShooterPlayer::ReloadSpent);
		CurrentWeapon = Weapon;
		LineTraceParams.AddIgnoredActor(Weapon);
	}
}

void AShooterPlayer::ReloadSpent(const int Amount)
{
	Ammunition = FMath::Max(Ammunition - abs(Amount), 0);
}

void AShooterPlayer::UnequipWeapon_Implementation()
{
	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentWeapon->Execute_OnInteract(CurrentWeapon, nullptr);
		CurrentWeapon->OnReloadSpent.RemoveDynamic(this, &AShooterPlayer::ReloadSpent);
		CurrentWeapon = nullptr;
		LineTraceParams.ClearIgnoredSourceObjects();
		LineTraceParams.AddIgnoredActor(this);
	}
}

void AShooterPlayer::HandleHealthChange(const float HealthResult)
{
	if (HealthResult <= 0.0f)
	{
		Destroy();
	}
}

void AShooterPlayer::HandleMoveForward()
{
	MovementDirection.X += 1.0f;
}

void AShooterPlayer::HandleMoveBackward()
{
	MovementDirection.X -= 1.0f;
}

void AShooterPlayer::HandleMoveRight()
{
	MovementDirection.Y += 1.0f;
}

void AShooterPlayer::HandleMoveLeft()
{
	MovementDirection.Y -= 1.0f;
}

void AShooterPlayer::HandleSprint()
{
	float& MaxSpeed = GetCharacterMovement()->MaxWalkSpeed;
	MaxSpeed = FMath::IsNearlyEqual(MaxSpeed, DefaultSpeed) ? DefaultSpeed * SprintMultiplier : DefaultSpeed;
}

void AShooterPlayer::HandleCrouch()
{
	bIsCrouched ? UnCrouch() : Crouch();
}

void AShooterPlayer::HandleJump()
{
	Jump();
}

void AShooterPlayer::HandleStartShoot()
{
	OnShootHeld.Broadcast(true);
}

void AShooterPlayer::HandleStopShoot()
{
	OnShootHeld.Broadcast(false);
}

void AShooterPlayer::HandleReload()
{
	OnReloaded.Broadcast(Ammunition);
}

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

	ADoor* HitDoor = Cast<ADoor>(LineTraceHitInteraction.GetActor());
	if (IsValid(HitDoor))
	{
		HitDoor->Execute_OnInteract(HitDoor, this);
	}
}
