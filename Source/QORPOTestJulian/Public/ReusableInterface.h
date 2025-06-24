#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "ReusableInterface.generated.h"

UINTERFACE(Blueprintable, BlueprintType)
class UReusableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class QORPOTESTJULIAN_API IReusableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status")
	FVector GetOriginalPosition();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status")
	FRotator GetOriginalRotation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status")
	void SetOriginalPositionAndRotation(FVector Position, FRotator Rotation);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status")
	void SetOriginalPosition(FVector Position);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status")
	void SetOriginalRotation(FRotator Rotation);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status")
	void OnTurnEnabled(const bool bEnabled = true);

protected:
	TMap<UPrimitiveComponent*, TEnumAsByte<ECollisionEnabled::Type>> CollisionEnabledTypes = TMap<UPrimitiveComponent*, TEnumAsByte<ECollisionEnabled::Type>>();
	
	FVector OriginalPosition = FVector::ZeroVector;

	FRotator OriginalRotation = FRotator::ZeroRotator;

	bool bEnableStatus = true;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Status")
	void AddEnabledType(UPrimitiveComponent* PrimitiveComponent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void DoDamage(AActor* DamageReceiver, float DamageAmount, FDamageEvent const& DamageEvent);

	virtual FVector GetOriginalPosition_Implementation();

	virtual FRotator GetOriginalRotation_Implementation();

	virtual void SetOriginalPositionAndRotation_Implementation(FVector Position, FRotator Rotation);

	virtual void SetOriginalPosition_Implementation(FVector Position);

	virtual void SetOriginalRotation_Implementation(FRotator Rotation);

	virtual void OnTurnEnabled_Implementation(const bool bEnabled = true);

	virtual void AddEnabledType_Implementation(UPrimitiveComponent* PrimitiveComponent);

	virtual void DoDamage_Implementation(AActor* DamageReceiver, float DamageAmount, FDamageEvent const& DamageEvent);
};
