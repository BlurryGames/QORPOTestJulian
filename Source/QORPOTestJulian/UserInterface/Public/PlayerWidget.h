#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

#include "PlayerWidget.generated.h"

/**
 * UPlayerWidget
 *
 * Abstract base class for the player's HUD widget.
 * Handles the display and updating of player-related UI elements such as health, ammunition, score, round, and survive time.
 * Provides BlueprintNativeEvent functions for updating UI elements in response to gameplay events.
 * Designed to be extended in both C++ and Blueprints for custom UI behavior.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/** Called when the widget is constructed. Initializes UI elements to their default state. */
	virtual void NativeConstruct() override;

	/**
	 * Updates the health progress bar in the UI.
	 * @param CurrentHealth The player's current health value.
	 * @param MaxHealth The player's maximum health value.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerUI")
	void HandleHealthProgressBarUpdated(const float CurrentHealth, const float MaxHealth);

	/**
	 * Updates the survive time text in the UI.
	 * Formats the time as HH:MM:SS.
	 * @param Seconds The total survive time in seconds.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerUI")
	void HandleSurviveTimeTextUpdated(const float Seconds);

	/**
	 * Updates the weapon magazine text in the UI.
	 * Displays a symbol for each bullet in the magazine.
	 * @param Magazine The current number of bullets in the magazine.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerUI")
	void HandleWeaponMagazineTextUpdated(const int Magazine);

	/**
	 * Updates the player's ammunition text in the UI.
	 * @param CurrentAmmunition The player's current ammunition count.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerUI")
	void HandlePlayerAmmunitionTextUpdated(const int CurrentAmmunition);

	/**
	 * Updates the round text in the UI.
	 * @param CurrentRound The current round number.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerUI")
	void HandleRoundTextUpdated(const int CurrentRound);

	/**
	 * Updates the score text in the UI.
	 * @param Score The player's current score.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerUI")
	void HandleScoreTextUpdated(const int Score);

protected:
	/** Progress bar widget for displaying the player's health. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerUI", meta = (BindWidget))
	UProgressBar* HealthProgressBar = nullptr;
	
	/** Text block widget for displaying the survive time. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerUI", meta = (BindWidget))
	UTextBlock* SurviveTimeText = nullptr;

	/** Text block widget for displaying the weapon magazine. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerUI", meta = (BindWidget))
	UTextBlock* WeaponMagazineText = nullptr;

	/** Text block widget for displaying the player's ammunition. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerUI", meta = (BindWidget))
	UTextBlock* PlayerAmmunitionText = nullptr;

	/** Text block widget for displaying the current round. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerUI", meta = (BindWidget))
	UTextBlock* RoundText = nullptr;

	/** Text block widget for displaying the player's score. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerUI", meta = (BindWidget))
	UTextBlock* ScoreText = nullptr;

	/** Title prefix for the survive time text. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerUI|Data")
	FString SurviveTimeTextTitle = FString("Survive Time: ");

	/** Symbol used to represent each bullet in the weapon magazine. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerUI|Data")
	FString WeaponMagazineTextSymbol = FString("|");

	/** Title prefix for the player's ammunition text. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerUI|Data")
	FString PlayerAmmunitionTextTitle = FString("Ammo: ");

	/** Title prefix for the round text. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerUI|Data")
	FString RoundTextTitle = FString("Round: ");

	/** Title prefix for the score text. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerUI|Data")
	FString ScoreTextTitle = FString("Score: ");

	/**
	 * Sets the text of a UTextBlock widget.
	 * @param TextBlock The text block to update.
	 * @param Text The new text to display.
	 */
	UFUNCTION(BlueprintCallable, Category = "PlayerUI")
	void SetTextBlock(UTextBlock* TextBlock, FString Text);
};
