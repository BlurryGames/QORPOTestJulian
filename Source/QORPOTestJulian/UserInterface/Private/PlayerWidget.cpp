// Copyright (c) Julián López Barañano. All Rights Reserved.

/**
 * @file PlayerWidget.cpp
 * @brief Implements the logic for the UPlayerWidget class, which manages the player's HUD and UI updates.
 *
 * This class handles updating health, score, round, ammunition, and survive time UI elements.
 * It provides BlueprintNativeEvent methods for updating UI widgets in response to gameplay events.
 * Designed to be extended in both C++ and Blueprints for custom UI behavior.
 */

#include "../Public/PlayerWidget.h"

/**
 * Called when the widget is constructed.
 * Initializes UI elements to their default state.
 */
void UPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HandleSurviveTimeTextUpdated(0.0f);
	HandleRoundTextUpdated(0);
	HandleScoreTextUpdated(0);
}

/**
 * Updates the health progress bar in the UI.
 * @param CurrentHealth The player's current health value.
 * @param MaxHealth The player's maximum health value.
 */
void UPlayerWidget::HandleHealthProgressBarUpdated_Implementation(const float CurrentHealth, const float MaxHealth)
{
	if (IsValid(HealthProgressBar))
	{
		HealthProgressBar->SetPercent(CurrentHealth / MaxHealth);
	}
}

/**
 * Updates the survive time text in the UI.
 * Formats the time as HH:MM:SS.
 * @param Seconds The total survive time in seconds.
 */
void UPlayerWidget::HandleSurviveTimeTextUpdated_Implementation(const float Seconds)
{
	const unsigned int CalculatedHours = FMath::FloorToInt(Seconds / 3600);
	const unsigned int CalculatedMinutes = FMath::FloorToInt(FMath::Fmod(Seconds / 60, 60));
	const unsigned int CalculatedSeconds = FMath::FloorToInt(FMath::Fmod(Seconds, 60));
	SetTextBlock(SurviveTimeText, SurviveTimeTextTitle + 
		FString::Printf(TEXT("%02d:%02d:%02d"), CalculatedHours, CalculatedMinutes, CalculatedSeconds));
}

/**
 * Updates the player's ammunition text in the UI.
 * @param CurrentAmmunition The player's current ammunition count.
 */
void UPlayerWidget::HandlePlayerAmmunitionTextUpdated_Implementation(const int CurrentAmmunition)
{
	SetTextBlock(PlayerAmmunitionText, PlayerAmmunitionTextTitle + FString::FromInt(CurrentAmmunition));
}

/**
 * Updates the weapon magazine text in the UI.
 * Displays a symbol for each bullet in the magazine.
 * @param Magazine The current number of bullets in the magazine.
 */
void UPlayerWidget::HandleWeaponMagazineTextUpdated_Implementation(const int Magazine)
{
	FString Text = FString();
	for (int i = 0; i < Magazine; i++)
	{
		Text += WeaponMagazineTextSymbol;
	}

	SetTextBlock(WeaponMagazineText, Text);
}

/**
 * Updates the round text in the UI.
 * @param CurrentRound The current round number.
 */
void UPlayerWidget::HandleRoundTextUpdated_Implementation(const int CurrentRound)
{
	SetTextBlock(RoundText, RoundTextTitle + FString::FromInt(CurrentRound));
}

/**
 * Updates the score text in the UI.
 * @param Score The player's current score.
 */
void UPlayerWidget::HandleScoreTextUpdated_Implementation(const int Score)
{
	SetTextBlock(ScoreText, ScoreTextTitle + FString::FromInt(Score));
}

/**
 * Sets the text of a UTextBlock widget.
 * @param TextBlock The text block to update.
 * @param Text The new text to display.
 */
void UPlayerWidget::SetTextBlock(UTextBlock* TextBlock, FString Text)
{
	if (IsValid(TextBlock))
	{
		TextBlock->SetText(FText::FromString(Text));
	}
}