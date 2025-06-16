#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "HealEvent.h"

#include "HealingPackage.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API AHealingPackage : public ABaseItem
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Heal")
	FHealEvent HealEvent = FHealEvent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Heal", meta = (ClampMin = 1.0f, ClampMax = 1000.0f))
	float HealingPack = 20.0f;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void OnInteract_Implementation(AActor* Caller) override;
};
