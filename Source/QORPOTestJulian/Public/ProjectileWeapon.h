#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "BaseProjectile.h"

#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API AProjectileWeapon : public ABaseWeapon
{
	GENERATED_BODY()

	AProjectileWeapon();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Projectile")
	USceneComponent* ProjectilesContainerComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Projectile")
	TArray<ABaseProjectile*> ProjectilesContainer = TArray<ABaseProjectile*>();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Projectile")
	TSubclassOf<ABaseProjectile> ProjectileClass = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Projectile")
	int CurrentIndex = -1;

	virtual void BeginPlay() override;

	virtual bool HandleFire_Implementation() override;

	virtual void OnInteract_Implementation(AActor* Caller) override;
};
