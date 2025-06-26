#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "../../Interfaces/Public/InteractableInterface.h"
#include "../../Interfaces/Public/ReusableInterface.h"

#include "Door.generated.h"

/**
 * ADoor
 *
 * Represents an interactable and animated door actor in the game world.
 * Handles opening and closing logic, animation, collision, and network replication.
 * Supports both position and rotation transitions for smooth door movement.
 * Designed to be extended in C++ or Blueprints for custom door behavior.
 */
UCLASS(Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API ADoor : public AActor, public IInteractableInterface, public IReusableInterface
{
	GENERATED_BODY()
	
public:	
	/** Default constructor. Initializes components and default values. */
	ADoor();

protected:
	/** The static mesh component representing the door's visual appearance. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent = nullptr;

	/** The box collision component used for interaction and navigation blocking. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* BoxComponent = nullptr;

	/** Local offset from the original position when the door is closed. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	FVector ClosePositionOffset = FVector::ZeroVector;

	/** Local offset from the original position when the door is open. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	FVector OpenPositionOffset = FVector::ZeroVector;

	/** The current target world position for the door, replicated for networked animation. */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	FVector DesiredPosition = FVector::ZeroVector;

	/** Local rotation offset when the door is closed. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	FRotator CloseRotationOffset = FRotator::ZeroRotator;

	/** Local rotation offset when the door is open. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	FRotator OpenRotationOffset = FRotator(0.0f, 90.0f, 0.0f);

	/** The current target world rotation for the door, replicated for networked animation. */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	FRotator DesiredRotation = FRotator::ZeroRotator;

	/** Speed at which the door moves between positions (units per second). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (ClampMin = 0.0f, ClampMax = 1000.0f))
	float PositionSpeed = 140.0f;

	/** Speed at which the door rotates (degrees per second). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (ClampMin = 0.0f, ClampMax = 100.0f))
	float RotationSpeed = 2.0f;

	/** Tolerance for considering the door's position as reached (units). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (ClampMin = 1.0f, ClampMax = 10.0f))
	float PositionToleranceOffset = 1.0f;

	/** Tolerance for considering the door's rotation as reached (degrees). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (ClampMin = 0.1f, ClampMax = 1.0f))
	float RotationToleranceOffset = 0.1f;

	/** Whether the door is currently animating (opening or closing), replicated for networked state. */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bActiveAnimation = false;

	/**
	 * Called when the game starts or when spawned.
	 * Initializes door state and animation targets.
	 */
	virtual void BeginPlay() override;

	/**
	 * Registers properties for network replication.
	 * @param OutLifetimeProps The array to add replicated properties to.
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * Called every frame.
	 * Handles door animation logic.
	 * @param DeltaTime Time elapsed since the last tick.
	 */
	virtual void Tick(float DeltaTime) override;

	/**
	 * Called when this door is interacted with (e.g., by a player).
	 * Toggles the door's open/close state and starts the animation.
	 * @param Caller The actor that initiated the interaction.
	 */
	virtual void OnInteract_Implementation(AActor* Caller) override;

	/**
	 * Handles the door's animation each tick, moving and rotating the door towards its desired state.
	 * Animation completes when both position and rotation reach their targets.
	 * @param DeltaTime Time elapsed since the last tick.
	 */
	virtual void OnInteractionAnimation_Implementation(const float DeltaTime) override;
};
