#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "EngineUtils.h"
#include "RoundSpawnable.h"
#include "ShooterPlayerController.h"
#include "ShooterPlayer.h"
#include "BaseEnemy.h"

#include "ShooterGameModeBase.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API AShooterGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AShooterGameModeBase();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map|Round")
	TMap<TSubclassOf<ABaseEnemy>, FRoundSpawnable> RoundSpawnableParameters = TMap<TSubclassOf<ABaseEnemy>, FRoundSpawnable>();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map|Round")
	TArray<ABaseEnemy*> RoundEnemies = TArray<ABaseEnemy*>();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map")
	TArray<TSubclassOf<ABaseEnemy>> EnemyClassKeys = TArray<TSubclassOf<ABaseEnemy>>();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map")
	TArray<ANavMeshBoundsVolume*> NavMeshBoundsContainer = TArray<ANavMeshBoundsVolume*>();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map|Round")
	FTimerHandle BetweenRoundsTimerHandle = FTimerHandle();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map")
	FVector EnemyHiddenPosition = FVector(-10000.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Map|Round", meta = (ClampMin = 0.0f, ClampMax = 60.0f))
	float BetweenRoundsTime = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Map|Round", meta = (ClampMin = 1.01f, ClampMax = 10.0f))
	float RoundIncreaseMultiplier = 1.2f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map|Round")
	int CurrentRound = 0;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Map")
	void SpawnAllEnemies(TSubclassOf<ABaseEnemy> EnemyClass);

	UFUNCTION(BlueprintCallable, Category = "Map|Round")
	void ActivateRoundEnemies(TSubclassOf<ABaseEnemy> EnemyClass);

	UFUNCTION(BlueprintCallable, Category = "Map|Round")
	void HandleEnemyOut(ABaseEnemy* OutEnemy);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Map|Round")
	void HandleNextRound();

private:
	FTimerDelegate BetweenRoundsTimerDelegate = FTimerDelegate::CreateUObject(this, &AShooterGameModeBase::HandleNextRound);
};
