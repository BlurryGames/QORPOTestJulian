// Copyright (c) Julián López Barañano. All Rights Reserved.

/**
 * @file ShooterPlayerController.cpp
 * @brief Implements the logic for the AShooterPlayerController class, which manages player input, UI, score, and survive time.
 *
 * This class handles player-specific logic such as UI widget creation, score and survive time tracking, event binding,
 * and networked updates for health, ammunition, and round information. It is designed to be extended and supports
 * both C++ and Blueprint customization.
 */

#include "../Public/ShooterPlayerController.h"
#include "../Public/ShooterGameModeBase.h"
#include "../../Characters/Public/ShooterPlayer.h"

/**
 * Default constructor.
 * Sets up replication and initial controller state.
 */
AShooterPlayerController::AShooterPlayerController()
{
	SetReplicates(true);
	SetReplicateMovement(true);
	SetShowMouseCursor(false);
}

/**
 * Called when the game starts or when spawned.
 * Initializes the player UI widget and binds to round events.
 */
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

/**
 * Registers properties for network replication.
 * @param OutLifetimeProps The array to add replicated properties to.
 */
void AShooterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterPlayerController, SurviveTime);
	DOREPLIFETIME(AShooterPlayerController, CurrentScore);
}

/**
 * Called every frame.
 * Updates the survive time for the player.
 * @param DeltaTime Time elapsed since the last tick.
 */
void AShooterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSurviveTime();
}

/**
 * Called when this controller possesses a new pawn.
 * Binds to health and ammunition events from the possessed player.
 * @param InPawn The pawn being possessed.
 */
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

/**
 * Called when the controller is removed from the world.
 * Clears all event bindings.
 * @param EndPlayReason The reason for removal.
 */
void AShooterPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	OnSurviveTimeUpdated.Clear();
	OnScoreUpdated.Clear();
}

/**
 * Returns whether the pitch input is inverted for this player.
 * @return True if pitch is inverted, false otherwise.
 */
const bool AShooterPlayerController::GetInvertPitch() const
{
	return bInvertPitch;
}

/**
 * Updates the player's score by adding the specified amount of points.
 * Broadcasts the OnScoreUpdated event if the score changes.
 * @param Points The amount of points to add to the score.
 */
void AShooterPlayerController::UpdateScore(const float Points)
{
	const float Score = CurrentScore;
	CurrentScore = FMath::Clamp(Score + Points, 0.0f, float(INT_MAX));
	if (CurrentScore != Score)
	{
		OnScoreUpdated.Broadcast(CurrentScore);
	}
}

/**
 * Server-side function to apply damage to a target actor.
 * @param DamageReceiver The actor receiving damage.
 * @param DamageAmount The amount of damage to apply.
 * @param DamageEvent The damage event struct.
 * @param EventInstigator The controller responsible for the damage.
 * @param DamageCauser The actor causing the damage.
 */
void AShooterPlayerController::Server_DoDamage_Implementation(AActor* DamageReceiver, float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (IsValid(DamageReceiver))
	{
		DamageReceiver->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	}
}

/**
 * Updates the survive time for the player and broadcasts the event if it changes.
 */
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

/**
 * Creates and initializes the player's UI widget.
 * Binds UI events and updates initial ammunition display.
 */
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

/**
 * Called when the SurviveTime property is replicated.
 * Broadcasts the OnSurviveTimeUpdated event.
 */
void AShooterPlayerController::OnReplicateSurviveTime()
{
	OnSurviveTimeUpdated.Broadcast(SurviveTime);
}

/**
 * Called when the CurrentScore property is replicated.
 * Broadcasts the OnScoreUpdated event.
 */
void AShooterPlayerController::OnReplicatedCurrentScore()
{
	OnScoreUpdated.Broadcast(CurrentScore);
}

/**
 * Multicast function to update the player's health in the UI on all clients.
 * @param CurrentHealth The current health value.
 * @param MaxHealth The maximum health value.
 */
void AShooterPlayerController::Multicast_HandleHealthUpdated_Implementation(const float CurrentHealth, const float MaxHealth)
{
	if (IsValid(PlayerWidget))
	{
		PlayerWidget->HandleHealthProgressBarUpdated(CurrentHealth, MaxHealth);
	}
}

/**
 * Multicast function to update the current round in the UI on all clients.
 * @param CurrentRound The current round number.
 */
void AShooterPlayerController::Multicast_HandleRoundUpdated_Implementation(const int CurrentRound)
{
	if (IsValid(PlayerWidget))
	{
		PlayerWidget->HandleRoundTextUpdated(CurrentRound);
	}
}

/**
 * Multicast function to update the weapon's magazine in the UI on all clients.
 * @param Magazine The current magazine value.
 */
void AShooterPlayerController::Multicast_HandleWeaponMagazineUpdated_Implementation(const int Magazine)
{
	if (IsValid(PlayerWidget))
	{
		PlayerWidget->HandleWeaponMagazineTextUpdated(Magazine);
	}
}

/**
 * Multicast function to update the player's ammunition in the UI on all clients.
 * @param CurrentAmmunition The current ammunition value.
 */
void AShooterPlayerController::Multicast_HandlePlayerAmmunitionUpdated_Implementation(const int CurrentAmmunition)
{
	if (IsValid(PlayerWidget))
	{
		PlayerWidget->HandlePlayerAmmunitionTextUpdated(CurrentAmmunition);
	}
}