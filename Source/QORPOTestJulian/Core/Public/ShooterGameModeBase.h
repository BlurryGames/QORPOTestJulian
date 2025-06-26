#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "EngineUtils.h"
#include "RoundSpawnable.h"
#include "ShooterPlayerController.h"
#include "../../Characters/Public/ShooterPlayer.h"
#include "../../Characters/Public/BaseEnemy.h"

#include "ShooterGameModeBase.generated.h"

/**
 * Delegate broadcast when a new round starts.
 * @param Round The round number that has started.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoundStarted, const int, Round);

/**
 * AShooterGameModeBase
 *
 * Main game mode class for the shooter game.
 * Manages round logic, enemy spawning, round progression, and navigation mesh bounds.
 * Handles the lifecycle of enemies and rounds, and provides Blueprint and C++ hooks for round events.
 *
 * This class is designed to be extended and supports both C++ and Blueprint customization.
 */
UCLASS(Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API AShooterGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	/** Event triggered when a new round starts. */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRoundStarted OnRoundStarted;

	/** Default constructor. Initializes default values and sets up round management. */
	AShooterGameModeBase();

protected:
	/**
	 * Map of enemy class types to their round spawnable parameters.
	 * Used to configure how each enemy type is spawned and managed per round.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map|Round")
	TMap<TSubclassOf<ABaseEnemy>, FRoundSpawnable> RoundSpawnableParameters = TMap<TSubclassOf<ABaseEnemy>, FRoundSpawnable>();

	/**
	 * Array containing all enemy instances spawned for the current round.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map|Round")
	TArray<ABaseEnemy*> RoundEnemies = TArray<ABaseEnemy*>();

	/**
	 * Array of enemy class keys used for round management and spawning.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map")
	TArray<TSubclassOf<ABaseEnemy>> EnemyClassKeys = TArray<TSubclassOf<ABaseEnemy>>();

	/**
	 * Array of navigation mesh bounds volumes present in the level.
	 * Used for AI navigation and enemy spawning.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map")
	TArray<ANavMeshBoundsVolume*> NavMeshBoundsContainer = TArray<ANavMeshBoundsVolume*>();

	/**
	 * Timer handle used to manage the delay between rounds.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map|Round")
	FTimerHandle BetweenRoundsTimerHandle = FTimerHandle();

	/**
	 * Position used to hide enemies when not active (e.g., between rounds).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map")
	FVector EnemyHiddenPosition = FVector(-10000.0f);

	/**
	 * Time in seconds to wait between rounds before starting the next round.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Map|Round", meta = (ClampMin = 0.0f, ClampMax = 60.0f))
	float BetweenRoundsTime = 10.0f;

	/**
	 * Multiplier applied to increase the number of enemies or difficulty each round.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map|Round", meta = (ClampMin = 1.01f, ClampMax = 10.0f))
	float RoundIncreaseMultiplier = 1.2f;

	/**
	 * The current round number.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map|Round")
	int CurrentRound = 0;

	/**
	 * Called when the game starts or when spawned.
	 * Initializes round and enemy management.
	 */
	virtual void BeginPlay() override;

	/**
	 * Called when the game mode is removed from the world.
	 * Cleans up round and enemy management.
	 * @param EndPlayReason The reason for removal.
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * Spawns all enemies of the specified class for the current round.
	 * @param EnemyClass The class of enemy to spawn.
	 */
	UFUNCTION(BlueprintCallable, Category = "Map")
	void SpawnAllEnemies(TSubclassOf<ABaseEnemy> EnemyClass);

	/**
	 * Activates all enemies of the specified class for the current round.
	 * @param EnemyClass The class of enemy to activate.
	 */
	UFUNCTION(BlueprintCallable, Category = "Map|Round")
	void ActivateRoundEnemies(TSubclassOf<ABaseEnemy> EnemyClass);

	/**
	 * Handles logic when an enemy leaves the round (e.g., defeated or removed).
	 * @param OutEnemy The enemy that left the round.
	 */
	UFUNCTION(BlueprintCallable, Category = "Map|Round")
	void HandleEnemyOut(ABaseEnemy* OutEnemy);

	/**
	 * Handles the transition to the next round.
	 * Can be overridden in Blueprints for custom round progression logic.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Map|Round")
	void HandleNextRound();

private:
	/** Delegate used internally to trigger the next round after a delay. */
	FTimerDelegate BetweenRoundsTimerDelegate = FTimerDelegate::CreateUObject(this, &AShooterGameModeBase::HandleNextRound);
};
