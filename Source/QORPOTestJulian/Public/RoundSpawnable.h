#pragma once

#include "CoreMinimal.h"

#include "RoundSpawnable.generated.h"

class ABaseEnemy;
/**
 * 
 */
USTRUCT(BlueprintType)
struct FRoundSpawnable
{
	GENERATED_BODY()

public:
	FRoundSpawnable() {}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	TArray<ABaseEnemy*> EnemiesContainer = TArray<ABaseEnemy*>();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data", meta = (ClampMin = -1.0f, ClampMax = 10000.0f))
	float SpawnAltitude = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data", meta = (ClampMin = 0.01f, ClampMax = 1.0f))
	float EnemiesAmountMultiplier = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data", meta = (ClampMin = 1, ClampMax = 100.0f))
	int TotalEnemies = 100;
};
