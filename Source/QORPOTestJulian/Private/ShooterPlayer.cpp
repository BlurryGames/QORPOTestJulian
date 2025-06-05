#include "ShooterPlayer.h"
#include "BaseWeapon.h"

AShooterPlayer::AShooterPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = true;

	UCharacterMovementComponent& MovementComponent = *GetCharacterMovement();
	DefaultSpeed = MovementComponent.MaxWalkSpeed;
	MovementComponent.bOrientRotationToMovement = false;
	MovementComponent.GetNavAgentPropertiesRef().bCanCrouch = true;

	WeaponSocketComponent = CreateDefaultSubobject<USceneComponent>(FName("WeaponSocketComponent"));
	WeaponSocketComponent->SetupAttachment(RootComponent);
	AttributesComponent = CreateDefaultSubobject<UAttributesComponent>(FName("AttributesComponent"));
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
	AttributesComponent->OnHealthChanged.AddUniqueDynamic(this, &AShooterPlayer::HandleHealthChange);
}

void AShooterPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AddMovementInput(GetMovementDirection());
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
	WeaponSocketComponent->SetRelativeRotation(FRotator(GetControlRotation().Pitch, 0.0f, 0.0f));
}

void AShooterPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UnequipWeapon();
	AttributesComponent->OnHealthChanged.RemoveDynamic(this, &AShooterPlayer::HandleHealthChange);
}

FVector AShooterPlayer::GetMovementDirection() const
{
	return GetActorForwardVector() * MovementDirection.X + GetActorRightVector() * MovementDirection.Y;
}

void AShooterPlayer::EquipWeapon_Implementation(ABaseWeapon* Weapon)
{
	if (!IsValid(Weapon) || Weapon == CurrentWeapon)
	{
		return;
	}

	UnequipWeapon();
	Weapon->AttachToComponent(WeaponSocketComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Weapon->Execute_OnPickup(Weapon, this);
	Weapon->OnReloadSpent.AddUniqueDynamic(this, &AShooterPlayer::ReloadSpent);
	CurrentWeapon = Weapon;
}

void AShooterPlayer::ReloadSpent(int Amount)
{
	Ammunition = FMath::Max(Ammunition - abs(Amount), 0);
}

void AShooterPlayer::UnequipWeapon_Implementation()
{
	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->Execute_OnDrop(CurrentWeapon);
		CurrentWeapon->OnReloadSpent.RemoveDynamic(this, &AShooterPlayer::ReloadSpent);
		CurrentWeapon = nullptr;
	}
}

void AShooterPlayer::HandleHealthChange(const float Amount)
{
	if (Amount <= 0.0f)
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