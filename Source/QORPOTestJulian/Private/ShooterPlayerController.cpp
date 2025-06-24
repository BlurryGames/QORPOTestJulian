#include "ShooterPlayerController.h"
#include "ShooterGameModeBase.h"
#include "ShooterPlayer.h"

AShooterPlayerController::AShooterPlayerController()
{
	SetReplicates(true);
	SetReplicateMovement(true);
	SetShowMouseCursor(false);
}

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	CreatePlayerWidget();
	UWorld* World = GetWorld();
	AShooterGameModeBase* GameMode = IsValid(World) ? World->GetAuthGameMode<AShooterGameModeBase>() : nullptr;
	if (IsValid(GameMode))
	{
		GameMode->OnRoundStarted.AddUniqueDynamic(this, &AShooterPlayerController::Multicast_HandleRoundUpdated);
	}
}

void AShooterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterPlayerController, SurviveTime);
	DOREPLIFETIME(AShooterPlayerController, CurrentScore);
}

void AShooterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSurviveTime();
}

void AShooterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AShooterPlayer* ShooterPlayer = Cast<AShooterPlayer>(InPawn);
	if (!IsValid(ShooterPlayer))
	{
		return;
	}
	else if (IsValid(ShooterPlayer->GetAttributesComponent()))
	{
		ShooterPlayer->GetAttributesComponent()->OnHealthChanged.AddUniqueDynamic(this, &AShooterPlayerController::Multicast_HandleHealthUpdated);
	}

	ShooterPlayer->OnPlayerAmmunitionUpdated.AddUniqueDynamic(this, &AShooterPlayerController::Multicast_HandlePlayerAmmunitionUpdated);
	ShooterPlayer->OnWeaponMagazineUpdated.AddUniqueDynamic(this, &AShooterPlayerController::Multicast_HandleWeaponMagazineUpdated);
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

void AShooterPlayerController::Server_DoDamage_Implementation(AActor* DamageReceiver, float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (IsValid(DamageReceiver))
	{
		DamageReceiver->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
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
	bool bCantSetUI = false;
	switch (GetNetMode())
	{
	case NM_DedicatedServer:
		bCantSetUI = HasAuthority();
		break;
	case NM_ListenServer:
		bCantSetUI = HasAuthority() && !IsLocalPlayerController();
	}

	if (bCantSetUI || !IsValid(PlayerWidgetClass) || IsValid(PlayerWidget))
	{
		return;
	}

	PlayerWidget = CreateWidget<UPlayerWidget>(this, PlayerWidgetClass);
	PlayerWidget->AddToPlayerScreen();

	OnSurviveTimeUpdated.AddUniqueDynamic(PlayerWidget, &UPlayerWidget::HandleSurviveTimeTextUpdated);
	OnScoreUpdated.AddUniqueDynamic(PlayerWidget, &UPlayerWidget::HandleScoreTextUpdated);

	AShooterPlayer* ShooterPlayer = GetPawn<AShooterPlayer>();
	if (IsValid(ShooterPlayer))
	{
		Multicast_HandlePlayerAmmunitionUpdated(ShooterPlayer->GetAmmunition());
	}
}

void AShooterPlayerController::OnReplicateSurviveTime()
{
	OnSurviveTimeUpdated.Broadcast(SurviveTime);
}

void AShooterPlayerController::OnReplicatedCurrentScore()
{
	OnScoreUpdated.Broadcast(CurrentScore);
}

void AShooterPlayerController::Multicast_HandleHealthUpdated_Implementation(const float CurrentHealth, const float MaxHealth)
{
	if (IsValid(PlayerWidget))
	{
		PlayerWidget->HandleHealthProgressBarUpdated(CurrentHealth, MaxHealth);
	}
}

void AShooterPlayerController::Multicast_HandleRoundUpdated_Implementation(const int CurrentRound)
{
	if (IsValid(PlayerWidget))
	{
		PlayerWidget->HandleRoundTextUpdated(CurrentRound);
	}
}

void AShooterPlayerController::Multicast_HandleWeaponMagazineUpdated_Implementation(const int Magazine)
{
	if (IsValid(PlayerWidget))
	{
		PlayerWidget->HandleWeaponMagazineTextUpdated(Magazine);
	}
}

void AShooterPlayerController::Multicast_HandlePlayerAmmunitionUpdated_Implementation(const int CurrentAmmunition)
{
	if (IsValid(PlayerWidget))
	{
		PlayerWidget->HandlePlayerAmmunitionTextUpdated(CurrentAmmunition);
	}
}