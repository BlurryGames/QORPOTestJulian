#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/AudioComponent.h"  
#include "Particles/ParticleSystemComponent.h"
#include "EngineUtils.h"
#include "AIController.h"
#include "HealEvent.h"
#include "ReusableInterface.h"
#include "AttributesComponent.h"

#include "BaseEnemy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyOut, ABaseEnemy*, Enemy);

UCLASS(Abstract, Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API ABaseEnemy : public APawn, public IReusableInterface
{
	GENERATED_BODY()

public:
	ABaseEnemy();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEnemyOut OnEnemyOut;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* BoxComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UParticleSystemComponent* ParticleComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAudioComponent* AudioComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAttributesComponent* AttributesComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UFloatingPawnMovement* FloatingMovement = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Target")
	TArray<AActor*> Targets = TArray<AActor*>();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Target")
	AActor* CurrentTarget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	FAIMoveRequest MoveRequest = FAIMoveRequest();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (ClampMin = 0.0f, ClampMax = 1000.0f))
	float Points = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (ClampMin = -1000.0f, ClampMax = 0.0f))
	float Damage = -4.0f;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnTurnEnabled_Implementation(const bool bEnabled) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Movement")
	void OnUpdateTarget();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Movement")
	void HandleSystemFinished(UParticleSystemComponent* ParticleSystem);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void HandleHealthChanged(const float HealthResult, const float TotalHealth);
};
