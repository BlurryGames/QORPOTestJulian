#include "ShooterPlayerController.h"
#include "ShooterGameModeBase.h"
#include "ShooterPlayer.h"

AShooterPlayerController::AShooterPlayerController()
{
	SetShowMouseCursor(false);
}

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	CreatePlayerWidget();
	AShooterGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AShooterGameModeBase>();
	if (IsValid(GameMode) && IsValid(PlayerWidget))
	{
		GameMode->OnRoundStarted.AddUniqueDynamic(PlayerWidget, &UPlayerWidget::HandleRoundTextUpdated);
	}
}

void AShooterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSurviveTime();
}

void AShooterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	CreatePlayerWidget();
	AShooterPlayer* ShooterPlayer = Cast<AShooterPlayer>(InPawn);
	if (!IsValid(PlayerWidget) || !IsValid(ShooterPlayer))
	{
		return;
	}
	else if (IsValid(ShooterPlayer->GetAttributesComponent()))
	{
		ShooterPlayer->GetAttributesComponent()->OnHealthChanged.AddUniqueDynamic(PlayerWidget, &UPlayerWidget::HandleHealthProgressBarUpdated);
	}

	ShooterPlayer->OnPlayerAmmunitionUpdated.AddUniqueDynamic(PlayerWidget, &UPlayerWidget::HandlePlayerAmmunitionTextUpdated);
	ShooterPlayer->OnWeaponMagazineUpdated.AddUniqueDynamic(PlayerWidget, &UPlayerWidget::HandleWeaponMagazineTextUpdated);
	ShooterPlayer->AddAmmunition(0);
}

void AShooterPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	OnSurviveTimeUpdated.Clear();
	OnScoreUpdated.Clear();
}

const bool AShooterPlayerController::GetInvertPitch() const
{
	return bInvertPitch;
}

void AShooterPlayerController::UpdateScore(const float Points)
{
	const float Score = CurrentScore;
	CurrentScore = FMath::Clamp(Score + Points, 0.0f, float(INT_MAX));
	if (CurrentScore != Score)
	{
		OnScoreUpdated.Broadcast(CurrentScore);
	}
}

void AShooterPlayerController::UpdateSurviveTime()
{
	APawn* CurrentPawn = GetPawn();
	if (!IsValid(CurrentPawn))
	{
		return;
	}

	const float CurrentTime = CurrentPawn->GetGameTimeSinceCreation();
	if (!FMath::IsNearlyEqual(CurrentTime, SurviveTime, 1.0f))
	{
		SurviveTime = CurrentTime;
		OnSurviveTimeUpdated.Broadcast(CurrentTime);
	}
}

void AShooterPlayerController::CreatePlayerWidget()
{
	if (IsValid(PlayerWidgetClass) && !IsValid(PlayerWidget))
	{
		PlayerWidget = CreateWidget<UPlayerWidget>(this, PlayerWidgetClass);
		PlayerWidget->AddToPlayerScreen();

		OnSurviveTimeUpdated.AddUniqueDynamic(PlayerWidget, &UPlayerWidget::HandleSurviveTimeTextUpdated);
		OnScoreUpdated.AddUniqueDynamic(PlayerWidget, &UPlayerWidget::HandleScoreTextUpdated);
	}
}