#include "PlayerWidget.h"
#include "Components/CanvasPanelSlot.h"

void UPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HandleSurviveTimeTextUpdated(0.0f);
	HandleRoundTextUpdated(0);
	HandleScoreTextUpdated(0);
}

void UPlayerWidget::HandleHealthProgressBarUpdated_Implementation(const float CurrentHealth, const float MaxHealth)
{
	if (IsValid(HealthProgressBar))
	{
		HealthProgressBar->SetPercent(CurrentHealth / MaxHealth);
	}
}

void UPlayerWidget::HandleSurviveTimeTextUpdated_Implementation(const float Seconds)
{
	const unsigned int CalculatedHours = FMath::FloorToInt(Seconds / 3600);
	const unsigned int CalculatedMinutes = FMath::FloorToInt(FMath::Fmod(Seconds / 60, 60));
	const unsigned int CalculatedSeconds = FMath::FloorToInt(FMath::Fmod(Seconds, 60));
	SetTextBlock(SurviveTimeText, SurviveTimeTextTitle + 
		FString::Printf(TEXT("%02d:%02d:%02d"), CalculatedHours, CalculatedMinutes, CalculatedSeconds));
}

void UPlayerWidget::HandlePlayerAmmunitionTextUpdated_Implementation(const int CurrentAmmunition)
{
	SetTextBlock(PlayerAmmunitionText, PlayerAmmunitionTextTitle + FString::FromInt(CurrentAmmunition));
}

void UPlayerWidget::HandleWeaponMagazineTextUpdated_Implementation(const int Magazine)
{
	FString Text = FString();
	for (int i = 0; i < Magazine; i++)
	{
		Text += WeaponMagazineTextSymbol;
	}

	SetTextBlock(WeaponMagazineText, Text);
}

void UPlayerWidget::HandleRoundTextUpdated_Implementation(const int CurrentRound)
{
	SetTextBlock(RoundText, RoundTextTitle + FString::FromInt(CurrentRound));
}

void UPlayerWidget::HandleScoreTextUpdated_Implementation(const int Score)
{
	SetTextBlock(ScoreText, ScoreTextTitle + FString::FromInt(Score));
}

void UPlayerWidget::SetTextBlock(UTextBlock* TextBlock, FString Text)
{
	if (IsValid(TextBlock))
	{
		TextBlock->SetText(FText::FromString(Text));
	}
}