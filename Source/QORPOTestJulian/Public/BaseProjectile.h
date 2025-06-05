#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameframework/ProjectileMovementComponent.h"

#include "BaseProjectile.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API ABaseProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseProjectile();

	virtual void SetOwner(AActor* NewOwner) override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void EnableProjectile(const bool bEnable);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovementComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Limit")
	FTimerHandle LifeTimeHandle = FTimerHandle();

	UPROPERTY(EditDefaultsOnly, Category = "Limit", meta = (ClampMin = 1.0f, ClampMax = 100.0f))
	float LifeTime = 6.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Stats", meta = (ClampMin = 0.1f, ClampMax = 1000.0f))
	float Damage = 20.0f;

	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ImpactBody(AActor* Actor);

private:
	FTimerDelegate LifeTimeDelegate = FTimerDelegate::CreateUObject(this, &ABaseProjectile::EnableProjectile, false);
};
