#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "AttributesComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, const float, HealthResult, const float, TotalHealth);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class QORPOTESTJULIAN_API UAttributesComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChanged OnHealthChanged;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	const float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	const float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool HealthReaction(const float Amount);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void ResetHealth();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes", meta = (ClampMin = 1.0f, ClampMax = 1000.0f))
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", meta = (ClampMin = 1.0f, ClampMax = 1000.0f))
	float CurrentHealth = MaxHealth;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
