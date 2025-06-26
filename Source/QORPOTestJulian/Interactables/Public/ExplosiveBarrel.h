#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "../../Core/Public/HealEvent.h"
#include "../../Interfaces/Public/ReusableInterface.h"
#include "../../Components/Public/AttributesComponent.h"

#include "ExplosiveBarrel.generated.h"

/**
 * AExplosiveBarrel
 *
 * Represents an interactable explosive barrel actor in the game world.
 * Handles damage processing, explosion effects, radial damage application, and networked state.
 * When destroyed, applies radial damage and impulse to nearby actors, plays visual and audio effects, and disables itself.
 * Designed to be extended in C++ or Blueprints for custom explosive behavior.
 */
UCLASS(Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API AExplosiveBarrel : public AActor, public IReusableInterface
{
	GENERATED_BODY()
	
public:
	/** Default constructor. Initializes components and default values. */
	AExplosiveBarrel();

	/**
	 * Processes incoming damage events for this barrel.
	 * Handles different types of damage events (point, radial, generic) and updates health accordingly.
	 * Applies physics impulses on radial damage.
	 *
	 * @param DamageAmount The amount of damage.
	 * @param DamageEvent The event describing the type of damage.
	 * @param EventInstigator The controller responsible for the event.
	 * @param DamageCauser The actor that caused the event.
	 * @return The final amount of damage applied.
	 */
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	/** The static mesh component representing the barrel's visual appearance and physics. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent = nullptr;

	/** Sphere component used to determine the explosion's area of effect. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* ExplosionSphereComponent = nullptr;

	/** Capsule component used for additional collision or navigation blocking. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCapsuleComponent* CapsuleComponent = nullptr;

	/** Particle system component for explosion visual effects. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UParticleSystemComponent* ParticleComponent = nullptr;

	/** Audio component for explosion sound effects. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAudioComponent* AudioComponent = nullptr;

	/** Attributes component for managing health and damage reactions. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAttributesComponent* AttributesComponent = nullptr;

	/** List of actors affected by the explosion. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Explosion")
	TArray<AActor*> ExplosionOverlaps = TArray<AActor*>();

	/** Timer handle used for managing barrel disappearance after explosion. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Explosion")
	FTimerHandle DissapearTimerHandle = FTimerHandle();

	/** Radial damage event struct used for applying explosion damage. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	FRadialDamageEvent RadialDamageEvent = FRadialDamageEvent();

	/** Parameters for configuring the explosion's damage and radius. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	FRadialDamageParams RadialDamageParameters = FRadialDamageParams(60.0f, 30.0f, 100.0f, 200.0f, 1.0f);

	/** Multiplier for the impulse applied to affected actors during explosion. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion", meta = (ClampMin = 0.0f, ClampMax = 100.0f))
	float ImpulseMultiplier = 50.0f;

	/** Time in seconds before the barrel disappears after exploding. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion", meta = (ClampMin = 0.0f, ClampMax = 10.0f))
	float DissapearTime = 4.0f;

	/**
	 * Called when the game starts or when spawned.
	 * Initializes enabled types, sets up explosion parameters, and binds health change events.
	 */
	virtual void BeginPlay() override;

	/**
	 * Called when the barrel is removed from the world.
	 * Clears all timers associated with this barrel.
	 * @param EndPlayReason The reason for removal.
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * Handles changes in the barrel's health.
	 * Triggers explosion effects, applies radial damage to overlapping actors, and schedules barrel disappearance.
	 *
	 * @param HealthResult The new health value.
	 * @param TotalHealth The maximum health value.
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void HandleHealthChanged(const float HealthResult, const float TotalHealth);

	/**
	 * Multicast function to handle the barrel's disappearance after explosion.
	 * Disables the barrel for all clients.
	 */
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Explosion")
	void Multicast_HandleDissapear();
};
