#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "HealEvent.h"
#include "ReusableInterface.h"
#include "AttributesComponent.h"

#include "ExplosiveBarrel.generated.h"

UCLASS(Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API AExplosiveBarrel : public AActor, public IReusableInterface
{
	GENERATED_BODY()
	
public:
	AExplosiveBarrel();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* ExplosionSphereComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCapsuleComponent* CapsuleComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UParticleSystemComponent* ParticleComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAudioComponent* AudioComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAttributesComponent* AttributesComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Explosion")
	TArray<AActor*> ExplosionOverlaps = TArray<AActor*>();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Explosion")
	FTimerHandle DissapearTimerHandle = FTimerHandle();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	FRadialDamageEvent RadialDamageEvent = FRadialDamageEvent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	FRadialDamageParams RadialDamageParameters = FRadialDamageParams(60.0f, 30.0f, 100.0f, 200.0f, 1.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion", meta = (ClampMin = 0.0f, ClampMax = 100.0f))
	float ImpulseMultiplier = 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion", meta = (ClampMin = 0.0f, ClampMax = 10.0f))
	float DissapearTime = 4.0f;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void HandleHealthChanged(const float HealthResult, const float TotalHealth);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Explosion")
	void Multicast_HandleDissapear();
};
