#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "ShooterPlayer.h"

#include "AmmunitionPackage.generated.h"

/**
 * 
 */
UCLASS()
class QORPOTESTJULIAN_API AAmmunitionPackage : public ABaseItem
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (ClampMin = 1, ClampMax = 1000))
	int AmmunitionPack = 30;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void OnInteract_Implementation(AActor* Caller) override;
};
