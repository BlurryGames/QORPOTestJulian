#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerWidget.h"

#include "ShooterPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSurviveTimeUpdated, const float, Seconds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreUpdated, const int, Score);
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSurviveTimeUpdated OnSurviveTimeUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnScoreUpdated OnScoreUpdated;

	AShooterPlayerController();

	UFUNCTION(BlueprintCallable, Category = "Options")
	const bool GetInvertPitch() const;

	UFUNCTION(BlueprintCallable, Category = "Data")
	void UpdateScore(const float Points);

	UFUNCTION(Server, Unreliable)
	void Server_DoDamage(AActor* DamageReceiver, float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data|UI")
	UPlayerWidget* PlayerWidget = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|UI")
	TSubclassOf<UPlayerWidget> PlayerWidgetClass = nullptr;

	UPROPERTY(ReplicatedUsing = OnReplicateSurviveTime, VisibleAnywhere, BlueprintReadWrite, Category = "Data", meta = (ClampMin = 0.0f, ClampMax = 9999999999.0f))
	float SurviveTime = 0.0f;

	UPROPERTY(ReplicatedUsing = OnReplicatedCurrentScore, VisibleAnywhere, BlueprintReadWrite, Category = "Data", meta = (ClampMin = 0.0f, ClampMax = 9999999999.0f))
	float CurrentScore = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data|Options")
	bool bInvertPitch = false;

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Data")
	void UpdateSurviveTime();

	UFUNCTION(BlueprintCallable, Category = "Data|UI")
	void CreatePlayerWidget();

	UFUNCTION(BlueprintCallable, Category = "Data")
	void OnReplicateSurviveTime();

	UFUNCTION(BlueprintCallable, Category = "Data")
	void OnReplicatedCurrentScore();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Data|UI")
	void Multicast_HandleHealthUpdated(const float CurrentHealth, const float MaxHealth);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Data|UI")
	void Multicast_HandleRoundUpdated(const int CurrentRound);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Data|UI")
	void Multicast_HandlePlayerAmmunitionUpdated(const int CurrentAmmunition);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Data|UI")
	void Multicast_HandleWeaponMagazineUpdated(const int Magazine);
};
