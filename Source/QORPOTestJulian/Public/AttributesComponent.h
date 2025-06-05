#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributesComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, const float, HealthResult);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class QORPOTESTJULIAN_API UAttributesComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributesComponent();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChanged OnHealthChanged;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void HealthReaction(const float Amount, bool bDamage = true);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes", meta = (ClampMin = 1.0f, ClampMax = 1000.0f))
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attributes", meta = (ClampMin = 1.0f, ClampMax = 1000.0f))
	float CurrentHealth = MaxHealth;
};
