#include "ShooterPlayerController.h"

AShooterPlayerController::AShooterPlayerController()
{
	SetShowMouseCursor(false);
}

const bool AShooterPlayerController::GetInvertPitch() const
{
	return bInvertPitch;
}

void AShooterPlayerController::UpdateScore(const float Points)
{
	Score = FMath::Clamp(Score + Points, 0.0f, 9999999999.0f);
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::FromInt(Score));
}

void AShooterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AShooterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APawn* CurrentPawn = GetPawn();
	if (IsValid(CurrentPawn))
	{
		SurviveTime = CurrentPawn->GetGameTimeSinceCreation();
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Black, FString::FromInt(SurviveTime));
	}
}
