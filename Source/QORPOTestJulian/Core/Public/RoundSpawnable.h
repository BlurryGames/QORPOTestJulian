#pragma once

#include "CoreMinimal.h"

#include "RoundSpawnable.generated.h"

class ABaseEnemy;

/**
 * FRoundSpawnable
 *
 * Data structure used to define the configuration for a round's enemy spawning in the game.
 * Contains information about the enemies to spawn, their spawn altitude, the multiplier for the number of enemies,
 * and the total number of enemies for the round.
 *
 * This struct is designed to be used in both C++ and Blueprints for flexible round setup and enemy management.
 */
USTRUCT(BlueprintType)
struct FRoundSpawnable
{
	GENERATED_BODY()

public:
	/** Default constructor. Initializes default values for the round spawnable configuration. */
	FRoundSpawnable() {}

	/**
	 * Container holding references to all enemy instances spawned in this round.
	 * This array is typically filled at runtime as enemies are spawned and managed.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	TArray<ABaseEnemy*> EnemiesContainer = TArray<ABaseEnemy*>();

	/**
	 * The altitude (Z coordinate) at which enemies should be spawned for this round.
	 * Can be used to control vertical placement of enemies in the level.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data", meta = (ClampMin = -1.0f, ClampMax = 10000.0f))
	float SpawnAltitude = 0.0f;

	/**
	 * Multiplier applied to the base number of enemies to determine how many should spawn in this round.
	 * Useful for scaling difficulty or adapting to player progress.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data", meta = (ClampMin = 0.01f, ClampMax = 1.0f))
	float EnemiesAmountMultiplier = 0.1f;

	/**
	 * The total number of enemies to spawn in this round.
	 * This value can be used as a cap or target for enemy spawning logic.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data", meta = (ClampMin = 1, ClampMax = 100.0f))
	int TotalEnemies = 100;
};
