#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameframework/ProjectileMovementComponent.h"
#include "../../Core/Public/HealEvent.h"
#include "../../Interfaces/Public/ReusableInterface.h"

#include "BaseProjectile.generated.h"

/**
 * ABaseProjectile
 *
 * Abstract base class for reusable projectile actors in the game world.
 * Handles projectile initialization, movement, collision, damage application, and networked state.
 * Designed to be extended for custom projectile behavior and supports both C++ and Blueprint customization.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API ABaseProjectile : public AActor, public IReusableInterface
{
	GENERATED_BODY()
	
public:	
	/** Default constructor. Initializes components and default values. */
	ABaseProjectile();

	/**
	 * Sets the owner of the projectile and updates collision ignore settings.
	 * @param NewOwner The new owner actor.
	 */
	virtual void SetOwner(AActor* NewOwner) override;

	/**
	 * Multicast function to update the projectile's position, rotation, and enabled state across the network.
	 * @param Position The new world position.
	 * @param Rotation The new world rotation.
	 * @param bEnable Whether the projectile should be enabled.
	 */
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Movement")
	void Multicast_ProjectileOut(const FVector& Position, const FRotator& Rotation, const bool bEnable = true);

protected:
	/** Static mesh component representing the projectile's visual appearance and collision. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent = nullptr;

	/** Projectile movement component for handling movement and physics. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovementComponent = nullptr;

	/** Timer handle used to manage the projectile's lifetime. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Limit")
	FTimerHandle LifeTimeHandle = FTimerHandle();

	/** Maximum lifetime of the projectile in seconds before it is disabled. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Limit", meta = (ClampMin = 1.0f, ClampMax = 100.0f))
	float LifeTime = 6.0f;

	/** Amount of damage dealt by the projectile on impact (should be negative for damage). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (ClampMin = -1000.0f, ClampMax = -0.1f))
	float Damage = -20.0f;

	/**
	 * Called when the game starts or when spawned.
	 * Initializes enabled types and disables the projectile by default.
	 */
	virtual void BeginPlay() override;

	/**
	 * Called when another actor begins to overlap with this projectile.
	 * Triggers the impact logic.
	 * @param OtherActor The actor that started overlapping.
	 */
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	/**
	 * Called when the projectile hits another actor or component.
	 * Triggers the impact logic.
	 * @param MyComp The component that was hit.
	 * @param OtherActor The actor that was hit.
	 * @param OtherComp The component that was hit.
	 * @param bSelfMoved Whether the hit was caused by this actor's movement.
	 * @param HitLocation The location of the hit.
	 * @param HitNormal The normal at the hit location.
	 * @param NormalImpulse The impulse applied at the hit.
	 * @param Hit The hit result data.
	 */
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	/**
	 * Called when the projectile is removed from the world.
	 * Clears the lifetime timer.
	 * @param EndPlayReason The reason for removal.
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * Enables or disables the projectile and its movement.
	 * Starts or stops the lifetime timer as appropriate.
	 * @param bEnabled Whether the projectile should be enabled.
	 */
	virtual void OnTurnEnabled_Implementation(const bool bEnabled) override;

	/**
	 * Handles the logic when the projectile impacts another actor.
	 * Applies damage and disables the projectile if appropriate.
	 * @param Actor The actor that was impacted.
	 */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ImpactBody(AActor* Actor);

private:
	/** Delegate used internally to manage the projectile's lifetime. */
	FTimerDelegate LifeTimeDelegate = FTimerDelegate::CreateUFunction(this, GET_FUNCTION_NAME_CHECKED(ABaseProjectile, OnTurnEnabled) ,false);
};
