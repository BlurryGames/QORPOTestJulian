#include "BaseEnemy.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "ShooterPlayer.h"

ABaseEnemy::ABaseEnemy()
{
    PrimaryActorTick.bCanEverTick = true;
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    SetRootComponent(CreateDefaultSubobject<USceneComponent>(FName("RootComponent")));
    USceneComponent* MainSceneComponent = GetRootComponent();

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeshComponent"));
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    MeshComponent->SetCollisionObjectType(ECC_Pawn);
    MeshComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
    MeshComponent->SetupAttachment(MainSceneComponent);

    BoxComponent = CreateDefaultSubobject<UBoxComponent>(FName("BoxComponent"));
    BoxComponent->bDynamicObstacle = true;
    BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BoxComponent->SetCollisionObjectType(ECC_Pawn);
    BoxComponent->SetCollisionResponseToAllChannels(ECR_Block);
    BoxComponent->SetupAttachment(MeshComponent);

    ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(FName("ParticleComponent"));
    ParticleComponent->SetAutoActivate(false);
    ParticleComponent->bAllowRecycling = true;
    ParticleComponent->SetupAttachment(MainSceneComponent);

    AudioComponent = CreateDefaultSubobject<UAudioComponent>(FName("AudioComponent"));
    AudioComponent->SetAutoActivate(false);
    AudioComponent->SetupAttachment(MainSceneComponent);

    AttributesComponent = CreateDefaultSubobject<UAttributesComponent>(FName("AttributesComponent"));

    FloatingMovement = CreateDefaultSubobject<UFloatingPawnMovement>(FName("FloatingMovementComponent"));
    FloatingMovement->MaxSpeed = 400.0f;
    FloatingMovement->Acceleration = 1000.0f;
    FloatingMovement->Deceleration = 800.0f;
    FloatingMovement->TurningBoost = 2.0f;
}

float ABaseEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float DamageResult = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    if (!IsValid(AttributesComponent))
    {
        return DamageResult;
    }

    bool bSuccessReaction = false;
    switch (DamageEvent.GetTypeID())
    {
    case FDamageEvent::ClassID:
    {
        DamageResult = -abs(DamageAmount);
        bSuccessReaction = AttributesComponent->HealthReaction(-abs(DamageAmount));

        break;
    }
    case FPointDamageEvent::ClassID:
    {
        DamageResult = -abs(DamageAmount);
        bSuccessReaction = AttributesComponent->HealthReaction(-abs(DamageAmount));

        break;
    }
    case FRadialDamageEvent::ClassID:
    {
        const FRadialDamageEvent& RadialEvent = static_cast<const FRadialDamageEvent&>(DamageEvent);
        const FVector& CurrentPosition = GetActorLocation();
        const FVector& RadialPosition = RadialEvent.Origin;
        DamageResult = abs(RadialEvent.Params.BaseDamage * RadialEvent.Params.GetDamageScale(FVector::Distance(CurrentPosition, RadialPosition)));
        bSuccessReaction = AttributesComponent->HealthReaction(-DamageResult);
        if (IsValid(FloatingMovement))
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Black, FString::FromInt(RadialEvent.Params.GetMaxRadius()));
            FloatingMovement->AddRadialImpulse(RadialPosition, RadialEvent.Params.GetMaxRadius(), DamageResult * DamageAmount, RIF_Linear, true);
        }

        break;
    }
    case FHealEvent::ClassID:
    {
        const FHealEvent& HealEvent = static_cast<const FHealEvent&>(DamageEvent);
        bSuccessReaction = AttributesComponent->HealthReaction(abs(DamageAmount));
        DamageResult = bSuccessReaction ? HealEvent.HealSuccess : DamageAmount;

        break;
    }
    default:
        break;
    }

    AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(EventInstigator);
    const float HealthResult = AttributesComponent->GetHealth();
    if (IsValid(PlayerController) && bSuccessReaction && HealthResult <= 0.0f)
    {
        PlayerController->UpdateScore(Points);
    }

    return DamageResult;
}

void ABaseEnemy::BeginPlay()
{
    Super::BeginPlay();

    Execute_AddEnabledType(this, MeshComponent);
    Execute_AddEnabledType(this, BoxComponent);

    if (IsValid(AttributesComponent))
    {
        AttributesComponent->OnHealthChanged.AddUniqueDynamic(this, &ABaseEnemy::HandleHealthChange);
    }

    if (IsValid(ParticleComponent))
    {
        ParticleComponent->OnSystemFinished.AddUniqueDynamic(this, &ABaseEnemy::HandleSystemFinished);
    }

    for (TActorIterator<AShooterPlayer> I(GetWorld()); I; ++I)
    {
        if (IsValid(*I))
        {
            Targets.AddUnique(*I);
        }
    }

    Execute_OnTurnEnabled(this, false);
}

void ABaseEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    OnUpdateTarget();
}

void ABaseEnemy::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    if (bEnableStatus && OtherActor != this && IsValid(Cast<AShooterPlayer>(OtherActor)))
    {
        OtherActor->TakeDamage(Damage, FDamageEvent(), GetInstigatorController(), this);
        HandleHealthChange(0.0f);
    }
}

void ABaseEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    OnEnemyOut.Clear();
    if (IsValid(ParticleComponent))
    {
        ParticleComponent->OnSystemFinished.RemoveAll(this);
    }
}

void ABaseEnemy::OnTurnEnabled_Implementation(const bool bEnabled)
{
    IReusableInterface::OnTurnEnabled_Implementation(bEnabled);

    if (!bEnabled)
    {
        OnEnemyOut.Broadcast(this);
        MoveRequest = FAIMoveRequest();
        CurrentTarget = nullptr;

        return;
    }
    else if (IsValid(AttributesComponent))
    {
        AttributesComponent->ResetHealth();
    }
}

void ABaseEnemy::OnUpdateTarget_Implementation()
{
    if (!bEnableStatus)
    {
        return;
    }

    const FVector& CurrentPosition = GetActorLocation();
    float MinDistance = IsValid(CurrentTarget) ? FVector::Distance(CurrentPosition, CurrentTarget->GetActorLocation()) : float(INT_MAX);
    for (AActor* T : Targets)
    {
        if (T == CurrentTarget || !IsValid(T))
        {
            continue;
        }

        const FVector& TargetPosition = T->GetActorLocation();
        const float CurrentDistance = FVector::Distance(CurrentPosition, TargetPosition);
        if (CurrentDistance < MinDistance)
        {
            MinDistance = CurrentDistance;
            CurrentTarget = T;
        }
    }

    AAIController* AIController = GetController<AAIController>();
    AActor* GoalActor = MoveRequest.GetGoalActor();
    if (IsValid(AIController) && GoalActor != CurrentTarget && IsValid(CurrentTarget))
    {
        MoveRequest = FAIMoveRequest();
        MoveRequest.SetReachTestIncludesGoalRadius(false);
        MoveRequest.SetGoalActor(CurrentTarget);
        AIController->MoveTo(MoveRequest);
    }
}

void ABaseEnemy::HandleSystemFinished_Implementation(UParticleSystemComponent* ParticleSystem)
{
    Execute_OnTurnEnabled(this, false);
}

void ABaseEnemy::HandleHealthChange(const float HealthResult)
{
    if (HealthResult > 0.0f)
    {
        return;
    }
    else if (IsValid(AudioComponent))
    {
        AudioComponent->Play();
    }

    SetActorTickEnabled(false);
    IsValid(ParticleComponent) && IsValid(ParticleComponent->Template) ? ParticleComponent->ActivateSystem(true) : Execute_OnTurnEnabled(this, false);
    AAIController* AIController = GetController<AAIController>();
    if (IsValid(AIController))
    {
        AIController->StopMovement();
    }

    bEnableStatus = false;
}