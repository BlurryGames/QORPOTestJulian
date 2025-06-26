#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/AudioComponent.h"  
#include "Particles/ParticleSystemComponent.h"
#include "EngineUtils.h"
#include "AIController.h"
#include "../../Core/Public/HealEvent.h"
#include "../../Interfaces/Public/ReusableInterface.h"
#include "../../Components/Public/AttributesComponent.h"

#include "BaseEnemy.generated.h"

/**
 * Delegate for broadcasting when an enemy dissapear.
 * @param Enemy The enemy instance that is leaving.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyOut, ABaseEnemy*, Enemy);

/**
 * ABaseEnemy is an abstract base class for all enemy pawns in the game.
 * 
 * This class provides core functionality for enemy actors, including:
 * - Health and damage handling
 * - AI movement and targeting
 * - Overlap and interaction logic
 * - Networked spawning and state management
 * - Integration with reusable object systems
 * 
 * It is designed to be extended for specific enemy types and supports Blueprint extension.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API ABaseEnemy : public APawn, public IReusableInterface
{
	GENERATED_BODY()

public:
	/** 
	 * Event triggered when the enemy dissapear (e.g., on death or deactivation).
	 */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEnemyOut OnEnemyOut;

	/**
	 * Default constructor. Initializes all components and sets up default values.
	 */
	ABaseEnemy();

	/**
	 * Handles incoming damage or healing events for this enemy.
	 * @param DamageAmount The amount of damage or heal.
	 * @param DamageEvent The event describing the type of damage or healing.
	 * @param EventInstigator The controller responsible for the event.
	 * @param DamageCauser The actor that caused the event.
	 * @return The final amount of damage or healing applied.
	 */
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/**
	 * Multicast function to spawn or reset the enemy at a given position and state.
	 * @param Position The world position to spawn at.
	 * @param bEnable Whether the enemy should be enabled after spawning or dissable if it is already spawned.
	 */
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Spawn")
	void Multicast_Spawn(const FVector& Position, const bool bEnable = true);

protected:
    /** Static mesh component representing the enemy's visual appearance. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent = nullptr;

	/** Box collision component for physical interactions and blocking. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* BoxComponent = nullptr;

	/** Particle system component for visual effects (e.g., death, hit). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UParticleSystemComponent* ParticleComponent = nullptr;

	/** Audio component for playing sound effects. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAudioComponent* AudioComponent = nullptr;

	/** Attributes component managing health and other stats. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAttributesComponent* AttributesComponent = nullptr;

	/** Floating pawn movement component for AI navigation and movement. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UFloatingPawnMovement* FloatingMovement = nullptr;

	/** List of potential target actors for this enemy (e.g., players). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Target")
	TArray<AActor*> Targets = TArray<AActor*>();

	/** The current target actor the enemy is focusing on. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Target")
	AActor* CurrentTarget = nullptr;

	/** Timer handle for managing the enemy's disappearance after death. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn")
	FTimerHandle DissapearTimerHandle = FTimerHandle();

	/** AI move request used for pathfinding and movement. */
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	FAIMoveRequest MoveRequest = FAIMoveRequest();

	/** Points awarded to the player for defeating this enemy. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (ClampMin = 0.0f, ClampMax = 1000.0f))
	float Points = 10.0f;

	/** Amount of damage this enemy deals on overlap. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (ClampMin = -1000.0f, ClampMax = 0.0f))
	float Damage = -4.0f;

	/** Time in seconds before the enemy disappears after being defeated. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn", meta = (ClampMin = 0.0f, ClampMax = 10.0f))
	float DissapearTime = 2.6f;

	/**
	 * Called when the game starts or when spawned.
	 * Initializes components and sets up event bindings.
	 */
	virtual void BeginPlay() override;

	/**
	 * Called every frame.
	 * Handles per-tick logic such as target updates.
	 * @param DeltaTime Time elapsed since the last tick.
	 */
	virtual void Tick(float DeltaTime) override;

	/**
	 * Called when another actor begins to overlap with this enemy.
	 * @param OtherActor The actor that started overlapping.
	 */
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	/**
	 * Called when the enemy is removed from the world.
	 * @param EndPlayReason The reason for removal.
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * Implementation of the reusable interface to enable or disable the enemy.
	 * @param bEnabled Whether the enemy should be enabled.
	 */
	virtual void OnTurnEnabled_Implementation(const bool bEnabled) override;

	/**
	 * Starts the timer that will make the enemy disappear after a delay.
	 */
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void StartDissapearTimer();

	/**
	 * Updates the current target for the enemy.
	 * Can be overridden in Blueprints for custom targeting logic.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Movement")
	void OnUpdateTarget();

	/**
	 * Handles changes in the enemy's health.
	 * @param HealthResult The new health value.
	 * @param TotalHealth The maximum health value.
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void HandleHealthChanged(const float HealthResult, const float TotalHealth);

private:
	/** Delegate used internally for the disappear timer. */
	const FTimerDelegate DissapearDelegate = FTimerDelegate::CreateUFunction(this, GET_FUNCTION_NAME_CHECKED(ABaseEnemy, Multicast_Spawn), FVector::ZeroVector, false);
};
