#include "ShooterPlayerController.h"

AShooterPlayerController::AShooterPlayerController()
{
	SetShowMouseCursor(false);
}

const bool AShooterPlayerController::GetInvertPitch() const
{
	return bInvertPitch;
}

void AShooterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();
}