#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"

#include "RayWeapon.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API ARayWeapon : public ABaseWeapon
{
	GENERATED_BODY()

	ARayWeapon();
	
protected:

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	FHitResult LineTraceHitResult = FHitResult();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Stats", meta = (ClampMin = -1000.0f, ClampMax = -0.1f))
	float Damage = -10.0f;

	FCollisionObjectQueryParams ObjectParams = FCollisionObjectQueryParams(ECC_Pawn);

	FCollisionQueryParams LineTraceParams = FCollisionQueryParams();

	virtual bool HandleFire_Implementation() override;
};
