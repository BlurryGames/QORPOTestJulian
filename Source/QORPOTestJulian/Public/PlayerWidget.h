#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

#include "PlayerWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerUI")
	void HandleHealthProgressBarUpdated(const float CurrentHealth, const float MaxHealth);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerUI")
	void HandleSurviveTimeTextUpdated(const float Seconds);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerUI")
	void HandleWeaponMagazineTextUpdated(const int Magazine);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerUI")
	void HandlePlayerAmmunitionTextUpdated(const int CurrentAmmunition);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerUI")
	void HandleRoundTextUpdated(const int CurrentRound);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerUI")
	void HandleScoreTextUpdated(const int Score);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerUI", meta = (BindWidget))
	UProgressBar* HealthProgressBar = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerUI", meta = (BindWidget))
	UTextBlock* SurviveTimeText = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerUI", meta = (BindWidget))
	UTextBlock* WeaponMagazineText = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerUI", meta = (BindWidget))
	UTextBlock* PlayerAmmunitionText = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerUI", meta = (BindWidget))
	UTextBlock* RoundText = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerUI", meta = (BindWidget))
	UTextBlock* ScoreText = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerUI|Data")
	FString SurviveTimeTextTitle = FString("Survive Time: ");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerUI|Data")
	FString WeaponMagazineTextSymbol = FString("|");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerUI|Data")
	FString PlayerAmmunitionTextTitle = FString("Ammo: ");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerUI|Data")
	FString RoundTextTitle = FString("Round: ");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerUI|Data")
	FString ScoreTextTitle = FString("Score: ");

	UFUNCTION(BlueprintCallable, Category = "PlayerUI")
	void SetTextBlock(UTextBlock* TextBlock, FString Text);
};
