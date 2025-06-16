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
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteract(AActor* Caller);

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Animation")
	void OnInteractionAnimation(const float DeltaTime);

	virtual void OnInteract_Implementation(AActor* Caller);

	virtual void OnInteractionAnimation_Implementation(const float DeltaTime);
};
