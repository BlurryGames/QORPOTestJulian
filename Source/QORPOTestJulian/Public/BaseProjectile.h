#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameframework/ProjectileMovementComponent.h"
#include "HealEvent.h"
#include "ReusableInterface.h"

#include "BaseProjectile.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API ABaseProjectile : public AActor, public IReusableInterface
{
	GENERATED_BODY()
	
public:	
	ABaseProjectile();

	virtual void SetOwner(AActor* NewOwner) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovementComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Limit")
	FTimerHandle LifeTimeHandle = FTimerHandle();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Limit", meta = (ClampMin = 1.0f, ClampMax = 100.0f))
	float LifeTime = 6.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (ClampMin = -1000.0f, ClampMax = -0.1f))
	float Damage = -20.0f;

	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnTurnEnabled_Implementation(const bool bEnabled) override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ImpactBody(AActor* Actor);

private:
	FTimerDelegate LifeTimeDelegate = FTimerDelegate::CreateUFunction(this, GET_FUNCTION_NAME_CHECKED(ABaseProjectile, OnTurnEnabled) ,false);
};
