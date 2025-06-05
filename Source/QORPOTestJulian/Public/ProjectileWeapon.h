#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "BaseProjectile.h"

#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API AProjectileWeapon : public ABaseWeapon
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Projectile")
	TSubclassOf<ABaseProjectile> ProjectileClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Projectile")
	TArray<ABaseProjectile*> ProjectilesContainer = TArray<ABaseProjectile*>();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Projectile")
	int CurrentIndex = -1;

	virtual void BeginPlay() override;

	virtual void OnPickup_Implementation(AShooterPlayer* Caller) override;

	virtual void HandleFire_Implementation() override;
};
