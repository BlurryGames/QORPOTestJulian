#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "InteractableInterface.generated.h"

UINTERFACE(Blueprintable, BlueprintType)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class QORPOTESTJULIAN_API IInteractableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	void OnInteract(AActor* Caller);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnTurnEnabled(const bool bEnabled);

protected:
	FVector OriginalPosition = FVector::ZeroVector;

	FRotator OriginalRotation = FRotator::ZeroRotator;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
	void OnInteractionAnimation(const float DeltaTime);

	virtual void OnInteract_Implementation(AActor* Caller);

	virtual void OnTurnEnabled_Implementation(const bool bEnabled);

	virtual void OnInteractionAnimation_Implementation(const float DeltaTime);
};
