#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PickupInterface.generated.h"

class AShooterPlayer;

UINTERFACE(BlueprintType)
class UPickupInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class QORPOTESTJULIAN_API IPickupInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	void OnPickup(AShooterPlayer* Caller);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	void OnDrop();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	void OnTurnEnabled(const bool bEnabled);

protected:
	virtual void OnPickup_Implementation(AShooterPlayer* Caller);
	virtual void OnDrop_Implementation();
	virtual void OnTurnEnabled_Implementation(const bool bEnabled);
};
