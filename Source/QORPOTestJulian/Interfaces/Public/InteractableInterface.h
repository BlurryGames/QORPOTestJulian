#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "InteractableInterface.generated.h"

/**
 * UInteractableInterface
 *
 * Unreal Engine interface class for interactable actors.
 * Allows actors to implement custom interaction and animation logic, accessible from both C++ and Blueprints.
 */
UINTERFACE(Blueprintable, BlueprintType)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IInteractableInterface
 *
 * Interface for actors that can be interacted with in the game world.
 * Provides methods for handling interaction events and per-frame interaction animations.
 * Designed to be implemented by actors that require custom interaction logic, such as pickups, doors, or switches.
 */
class QORPOTESTJULIAN_API IInteractableInterface
{
	GENERATED_BODY()

public:
	/**
	 * Called when an interaction with this actor is requested.
	 * Can be implemented in C++ or Blueprints to define custom interaction behavior.
	 *
	 * @param Caller The actor that initiated the interaction (e.g., a player or another actor).
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteract(AActor* Caller);

protected:
	/**
	 * Called every frame to handle custom interaction animation logic.
	 * Can be implemented in C++ or Blueprints to animate the actor while it is interactable or being interacted with.
	 *
	 * @param DeltaTime Time elapsed since the last tick.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
	void OnInteractionAnimation(const float DeltaTime);

	/**
	 * Default C++ implementation for OnInteract.
	 * Override this method to provide custom interaction logic in C++.
	 *
	 * @param Caller The actor that initiated the interaction.
	 */
	virtual void OnInteract_Implementation(AActor* Caller);

	/**
	 * Default C++ implementation for OnInteractionAnimation.
	 * Override this method to provide custom animation logic in C++.
	 *
	 * @param DeltaTime Time elapsed since the last tick.
	 */
	virtual void OnInteractionAnimation_Implementation(const float DeltaTime);
};
