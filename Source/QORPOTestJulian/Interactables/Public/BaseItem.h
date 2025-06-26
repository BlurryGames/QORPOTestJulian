#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../Interfaces/Public/InteractableInterface.h"
#include "../../Interfaces/Public/ReusableInterface.h"

#include "BaseItem.generated.h"

/**
 * ABaseItem
 *
 * Abstract base class for all interactable items in the game.
 * Handles core logic for item animation, interaction, respawn, and reusable state.
 * Designed to be extended for specific item types and supports both C++ and Blueprint customization.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API ABaseItem : public AActor, public IInteractableInterface, public IReusableInterface
{
	GENERATED_BODY()

public:
	/** Default constructor. Initializes components and default values. */
	ABaseItem();

protected:
	/** The static mesh component representing the item's visual appearance. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent = nullptr;

	/** Timer handle used for managing item respawn. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation|Respawn")
	FTimerHandle RespawnTimerHandle = FTimerHandle();

	/** Direction vector for item displacement animation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	FVector DisplacementDirection = FVector::UpVector;

	/** Direction vector for item rotation animation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	FVector RotationDirection = FVector::RightVector;

	/** Time in seconds before the item respawns after being collected or disabled. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation|Respawn", meta = (ClampMin = 0.0f, ClampMax = 1000.0f))
	float RespawnTime = 120.0;

	/** Maximum distance the item can be displaced from its original position during animation. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (ClampMin = 0.0f, ClampMax = 100.0f))
	float DisplacementDistanceLimit = 10.0f;

	/** Speed at which the item is displaced during animation. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (ClampMin = 0.0f, ClampMax = 100.0f))
	float DisplacementSpeed = 20.0f;

	/** Speed at which the item rotates during animation. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (ClampMin = 0.0f, ClampMax = 100.0f))
	float RotationSpeed = 4.0f;

	/** Whether the item's animation is currently active. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bActiveAnimation = true;

	/**
	 * Called when the game starts or when spawned.
	 * Initializes animation and stores the original position and rotation.
	 */
	virtual void BeginPlay() override;

	/**
	 * Called every frame.
	 * Handles item animation logic.
	 * @param DeltaTime Time elapsed since the last tick.
	 */
	virtual void Tick(float DeltaTime) override;

	/**
	 * Called when the item is removed from the world.
	 * Cleans up timers and resources.
	 * @param EndPlayReason The reason for removal.
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * Enables or disables the item and its animation.
	 * If disabled, starts the respawn countdown.
	 * @param bEnabled Whether the item should be enabled.
	 */
	virtual void OnTurnEnabled_Implementation(const bool bEnabled) override;

	/**
	 * Handles the item's interaction animation each tick.
	 * Rotates and moves the item for visual feedback.
	 * @param DeltaTime Time elapsed since the last tick.
	 */
	virtual void OnInteractionAnimation_Implementation(const float DeltaTime) override;

	/**
	 * Starts the respawn countdown timer for the item.
	 * When the timer expires, the item will be re-enabled.
	 */
	UFUNCTION(BlueprintCallable, Category = "Respawn")
	void StartRespawnCountdown();

private:
	/** Delegate used internally to trigger item respawn. */
	FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUFunction(this, GET_FUNCTION_NAME_CHECKED(ABaseItem, OnTurnEnabled), true);
};
