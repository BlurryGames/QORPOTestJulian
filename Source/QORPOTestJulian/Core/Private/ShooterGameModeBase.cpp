// Copyright (c) Julián López Barañano. All Rights Reserved.

/**
 * @file ShooterGameModeBase.cpp
 * @brief Implements the logic for the AShooterGameModeBase class, which manages round progression, enemy spawning, and game flow.
 *
 * This class is responsible for initializing the game mode, spawning and activating enemies for each round,
 * handling round transitions, and managing navigation mesh bounds for AI movement.
 * It is designed to be extended and supports both C++ and Blueprint customization.
 */

#include "../Public/ShooterGameModeBase.h"

/**
 * Default constructor.
 * Sets the default player controller and pawn classes for the game mode.
 */
AShooterGameModeBase::AShooterGameModeBase()
{
	PlayerControllerClass = AShooterPlayerController::StaticClass();
	DefaultPawnClass = AShooterPlayer::StaticClass();
}

/**
 * Called when the game starts or when spawned.
 * Initializes navigation mesh bounds, spawns all enemies for each class, and sets up the timer for the first round.
 */
void AShooterGameModeBase::BeginPlay()
{
	Super::BeginPlay();

    // Gather all navigation mesh bounds volumes in the world
    for (TActorIterator<ANavMeshBoundsVolume> I(GetWorld()); I; ++I)
    {
        if (IsValid(*I))
        {
            NavMeshBoundsContainer.AddUnique(*I);
        }
    }

    // Get all enemy class keys and spawn their enemies
    RoundSpawnableParameters.GetKeys(EnemyClassKeys);
    for (TSubclassOf<ABaseEnemy> K : EnemyClassKeys)
    {
        SpawnAllEnemies(K);
    }

    // Set timer to start the first round after a delay
    GetWorldTimerManager().SetTimer(BetweenRoundsTimerHandle, BetweenRoundsTimerDelegate, BetweenRoundsTime, false);
}

/**
 * Called when the game mode is removed from the world.
 * Clears round events and all timers associated with this object.
 *
 * @param EndPlayReason The reason for removal.
 */
void AShooterGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    OnRoundStarted.Clear();
    GetWorldTimerManager().ClearAllTimersForObject(this);
}

/**
 * Spawns all enemies of the specified class for the current round.
 * Each spawned enemy is hidden at EnemyHiddenPosition and bound to the HandleEnemyOut event.
 *
 * @param EnemyClass The class of enemy to spawn.
 */
void AShooterGameModeBase::SpawnAllEnemies(TSubclassOf<ABaseEnemy> EnemyClass)
{
    UWorld* World = GetWorld();
    if (!IsValid(World) || !IsValid(EnemyClass) || !RoundSpawnableParameters.Contains(EnemyClass))
    {
        return;
    }

    FRoundSpawnable& SpawnableParameters = RoundSpawnableParameters[EnemyClass];
    for (unsigned short i = 0; i < SpawnableParameters.TotalEnemies; i++)
    {
        ABaseEnemy* Enemy = World->SpawnActor<ABaseEnemy>(EnemyClass, EnemyHiddenPosition, FRotator::ZeroRotator);
        SpawnableParameters.EnemiesContainer.AddUnique(Enemy);
        Enemy->OnEnemyOut.AddUniqueDynamic(this, &AShooterGameModeBase::HandleEnemyOut);
    }
}

/**
 * Activates a subset of enemies of the specified class for the current round.
 * Randomly selects enemies and spawn locations within navigation mesh bounds, and calls their Multicast_Spawn method.
 * Increases the EnemiesAmountMultiplier for the next round.
 *
 * @param EnemyClass The class of enemy to activate.
 */
void AShooterGameModeBase::ActivateRoundEnemies(TSubclassOf<ABaseEnemy> EnemyClass)
{
    if (!IsValid(EnemyClass) || !RoundSpawnableParameters.Contains(EnemyClass))
    {
        return;
    }

    FRoundSpawnable& SpawnableParameters = RoundSpawnableParameters[EnemyClass];
    TArray<ABaseEnemy*> EnemiesContainer = TArray<ABaseEnemy*>(SpawnableParameters.EnemiesContainer);
    float& EnemiesAmountMultiplier = SpawnableParameters.EnemiesAmountMultiplier;
    unsigned short EnemiesAmount = FMath::Clamp(FMath::RoundToInt(
        SpawnableParameters.TotalEnemies * EnemiesAmountMultiplier), 0, SpawnableParameters.TotalEnemies);
    unsigned short NavMeshBoundsCount = NavMeshBoundsContainer.Num();
    for (unsigned short i = 0; i < EnemiesAmount; i++)
    {
        unsigned short EnemyIndex = FMath::RandHelper(EnemiesContainer.Num() - 1);
        unsigned short BoxIndex = FMath::RandHelper(NavMeshBoundsCount - 1);
        ABaseEnemy* Enemy = EnemiesContainer[EnemyIndex];
        ANavMeshBoundsVolume* BoundsVolume = NavMeshBoundsContainer[BoxIndex];
        EnemiesContainer.RemoveAt(EnemyIndex);
        if (!IsValid(Enemy) || !IsValid(BoundsVolume))
        {
            continue;
        }

        FVector DesiredPosition = FMath::RandPointInBox(BoundsVolume->GetComponentsBoundingBox(true));
        if (SpawnableParameters.SpawnAltitude >= 0.0f)
        {
            DesiredPosition.Z = SpawnableParameters.SpawnAltitude;
        }

        Enemy->Multicast_Spawn(DesiredPosition);
        RoundEnemies.AddUnique(Enemy);
    }

    EnemiesAmountMultiplier = FMath::Clamp(EnemiesAmountMultiplier * RoundIncreaseMultiplier, 0.0f, 1.0f);
}

/**
 * Handles logic when an enemy leaves the round (e.g., is defeated or removed).
 * Removes the enemy from the active round list and, if all enemies are out, starts the timer for the next round.
 *
 * @param OutEnemy The enemy that left the round.
 */
void AShooterGameModeBase::HandleEnemyOut(ABaseEnemy* OutEnemy)
{
    FTimerManager& TimerManager = GetWorldTimerManager();
    if (TimerManager.IsTimerActive(BetweenRoundsTimerHandle))
    {
        return;
    }

    RoundEnemies.Remove(OutEnemy);
    if (RoundEnemies.IsEmpty())
    {
        TimerManager.SetTimer(BetweenRoundsTimerHandle, BetweenRoundsTimerDelegate, BetweenRoundsTime, false);
    }
}

/**
 * Handles the transition to the next round.
 * Activates enemies for all enemy classes, increments the round counter, and broadcasts the OnRoundStarted event.
 * Can be overridden in Blueprints for custom round progression logic.
 */
void AShooterGameModeBase::HandleNextRound_Implementation()
{
    if (RoundSpawnableParameters.IsEmpty())
    {
        return;
    }

    for (TSubclassOf<ABaseEnemy> K : EnemyClassKeys)
    {
        ActivateRoundEnemies(K);
    }

    CurrentRound = FMath::Clamp(CurrentRound + 1, 0, INT_MAX);
    OnRoundStarted.Broadcast(CurrentRound);
}