#pragma once  

#include "CoreMinimal.h"  
#include "GameFramework/Actor.h"  
#include "Components/CapsuleComponent.h"  
#include "Components/SphereComponent.h"  
#include "Components/AudioComponent.h"  
#include "Particles/ParticleSystemComponent.h"
#include "HealEvent.h"  
#include "AttributesComponent.h"  

#include "ExplosiveBarrel.generated.h"  

UCLASS()  
class QORPOTESTJULIAN_API AExplosiveBarrel : public AActor  
{  
	GENERATED_BODY()  
	
public:	  
	AExplosiveBarrel();  

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;  

protected:  
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")  
	UStaticMeshComponent* MeshComponent = nullptr;  

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")  
	UCapsuleComponent* CapsuleComponent = nullptr;  

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")  
	USphereComponent* ExplosionSphereComponent = nullptr;  

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")  
	UParticleSystemComponent* ParticleComponent = nullptr;  

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")  
	UAudioComponent* AudioComponent = nullptr;  

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")  
	UAttributesComponent* AttributesComponent = nullptr;  

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Explosion")  
	TArray<AActor*> ExplosionOverlaps = TArray<AActor*>();  

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")  
	FRadialDamageEvent RadialDamageEvent = FRadialDamageEvent();  

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")  
	FRadialDamageParams RadialDamageParameters = FRadialDamageParams(60.0f, 30.0f, 100.0f, 200.0f, 1.0f);  

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion", meta = (ClampMin = 0.0f, ClampMax = 100.0f))  
	float ImpulseMultiplier = 50.0f;  

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Explosion")  
	void HandleHealthChange(const float HealthResult);  
};
