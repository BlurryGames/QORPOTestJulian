#pragma once

#include "CoreMinimal.h"
#include "Engine/DamageEvents.h"

#include "HealEvent.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FHealEvent : public FDamageEvent
{
	GENERATED_BODY()

	FHealEvent() {}

	FHealEvent(float Amount) : HealSuccess(Amount) {}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Heal", meta = (ClampMin = -1000.0f, ClampMax = 1000.0f))
	float HealSuccess = 911.0f;

	static const int32 ClassID = 3;

	virtual int32 GetTypeID() const override { return ClassID; };
	virtual bool IsOfType(int32 InID) const override { return (ClassID == InID) || FDamageEvent::IsOfType(InID); };
};
