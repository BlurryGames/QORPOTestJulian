#include "ShooterGameModeBase.h"

AShooterGameModeBase::AShooterGameModeBase()
{
	PlayerControllerClass = AShooterPlayerController::StaticClass();
	DefaultPawnClass = AShooterPlayer::StaticClass();
}

void AShooterGameModeBase::BeginPlay()
{
	Super::BeginPlay();

    for (TActorIterator<ANavMeshBoundsVolume> I(GetWorld()); I; ++I)
    {
        if (IsValid(*I))
        {
            NavMeshBoundsContainer.AddUnique(*I);
        }
    }

    RoundSpawnableParameters.GetKeys(EnemyClassKeys);
    for (TSubclassOf<ABaseEnemy> K : EnemyClassKeys)
    {
        SpawnAllEnemies(K);
    }

    GetWorldTimerManager().SetTimer(BetweenRoundsTimerHandle, BetweenRoundsTimerDelegate, BetweenRoundsTime, false);
}

void AShooterGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    GetWorldTimerManager().ClearAllTimersForObject(this);
}

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

        Enemy->Execute_SetOriginalPosition(Enemy, DesiredPosition);
        Enemy->Execute_OnTurnEnabled(Enemy, true);
        RoundEnemies.AddUnique(Enemy);
    }

    EnemiesAmountMultiplier = FMath::Clamp(EnemiesAmountMultiplier * RoundIncreaseMultiplier, 0.0f, 1.0f);
}

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
}