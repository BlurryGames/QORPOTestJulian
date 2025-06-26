#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../../UserInterface/Public/PlayerWidget.h"

#include "ShooterPlayerController.generated.h"

/**
 * Delegate broadcast when the survive time is updated.
 * @param Seconds The new survive time in seconds.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSurviveTimeUpdated, const float, Seconds);

/**
 * Delegate broadcast when the score is updated.
 * @param Score The new score value.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreUpdated, const int, Score);

/**
 * AShooterPlayerController
 *
 * Custom player controller for the shooter game.
 * Manages player input, UI widget creation, score, survive time tracking, and networked events.
 * Handles replication of key player data and provides Blueprint and C++ hooks for UI and gameplay events.
 */
UCLASS(Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/** Event triggered when the survive time is updated. */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSurviveTimeUpdated OnSurviveTimeUpdated;

	/** Event triggered when the score is updated. */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnScoreUpdated OnScoreUpdated;

	/** Default constructor. Initializes default values and sets up controller state. */
	AShooterPlayerController();

	/**
	 * Returns whether the pitch input is inverted for this player.
	 * @return True if pitch is inverted, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category = "Options")
	const bool GetInvertPitch() const;

	/**
	 * Updates the player's score by adding the specified amount of points.
	 * Broadcasts the OnScoreUpdated event and replicates the new score.
	 * @param Points The amount of points to add to the score.
	 */
	UFUNCTION(BlueprintCallable, Category = "Data")
	void UpdateScore(const float Points);

	/**
	 * Server-side function to apply damage to a target actor.
	 * @param DamageReceiver The actor receiving damage.
	 * @param DamageAmount The amount of damage to apply.
	 * @param DamageEvent The damage event struct.
	 * @param EventInstigator The controller responsible for the damage.
	 * @param DamageCauser The actor causing the damage.
	 */
	UFUNCTION(Server, Unreliable, BlueprintCallable, Category = "Net")
	void Server_DoDamage(AActor* DamageReceiver, float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

protected:
	/** Reference to the player's UI widget instance. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data|UI")
	UPlayerWidget* PlayerWidget = nullptr;

	/** The class used to create the player's UI widget. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data|UI")
	TSubclassOf<UPlayerWidget> PlayerWidgetClass = nullptr;

	/** The total time the player has survived, replicated to clients. */
	UPROPERTY(ReplicatedUsing = OnReplicateSurviveTime, VisibleAnywhere, BlueprintReadWrite, Category = "Data", meta = (ClampMin = 0.0f, ClampMax = 9999999999.0f))
	float SurviveTime = 0.0f;

	/** The current score of the player, replicated to clients. */
	UPROPERTY(ReplicatedUsing = OnReplicatedCurrentScore, VisibleAnywhere, BlueprintReadWrite, Category = "Data", meta = (ClampMin = 0.0f, ClampMax = 9999999999.0f))
	float CurrentScore = 0.0f;

	/** Whether the pitch input is inverted for this player. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data|Options")
	bool bInvertPitch = false;

	/** Called when the game starts or when spawned. Initializes UI and player data. */
	virtual void BeginPlay() override;

	/**
	 * Registers properties for network replication.
	 * @param OutLifetimeProps The array to add replicated properties to.
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * Called every frame. Handles per-tick logic such as survive time updates.
	 * @param DeltaTime Time elapsed since the last tick.
	 */
	virtual void Tick(float DeltaTime) override;

	/**
	 * Called when this controller possesses a new pawn.
	 * @param InPawn The pawn being possessed.
	 */
	virtual void OnPossess(APawn* InPawn) override;

	/**
	 * Called when the controller is removed from the world.
	 * Cleans up UI and event bindings.
	 * @param EndPlayReason The reason for removal.
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * Updates the survive time for the player and broadcasts the event.
	 */
	UFUNCTION(BlueprintCallable, Category = "Data")
	void UpdateSurviveTime();

	/**
	 * Creates and initializes the player's UI widget.
	 */
	UFUNCTION(BlueprintCallable, Category = "Data|UI")
	void CreatePlayerWidget();

	/**
	 * Called when the SurviveTime property is replicated.
	 * Broadcasts the OnSurviveTimeUpdated event.
	 */
	UFUNCTION(BlueprintCallable, Category = "Data")
	void OnReplicateSurviveTime();

	/**
	 * Called when the CurrentScore property is replicated.
	 * Broadcasts the OnScoreUpdated event.
	 */
	UFUNCTION(BlueprintCallable, Category = "Data")
	void OnReplicatedCurrentScore();

	/**
	 * Multicast function to update the player's health in the UI on all clients.
	 * @param CurrentHealth The current health value.
	 * @param MaxHealth The maximum health value.
	 */
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Data|UI")
	void Multicast_HandleHealthUpdated(const float CurrentHealth, const float MaxHealth);

	/**
	 * Multicast function to update the current round in the UI on all clients.
	 * @param CurrentRound The current round number.
	 */
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Data|UI")
	void Multicast_HandleRoundUpdated(const int CurrentRound);

	/**
	 * Multicast function to update the player's ammunition in the UI on all clients.
	 * @param CurrentAmmunition The current ammunition value.
	 */
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Data|UI")
	void Multicast_HandlePlayerAmmunitionUpdated(const int CurrentAmmunition);

	/**
	 * Multicast function to update the weapon's magazine in the UI on all clients.
	 * @param Magazine The current magazine value.
	 */
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Data|UI")
	void Multicast_HandleWeaponMagazineUpdated(const int Magazine);
};
