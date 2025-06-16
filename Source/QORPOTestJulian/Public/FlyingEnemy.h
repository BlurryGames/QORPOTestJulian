#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "FlyingEnemy.generated.h"

/**
 * 
 */
UCLASS()
class QORPOTESTJULIAN_API AFlyingEnemy : public ABaseEnemy
{
	GENERATED_BODY()
	
protected:
	virtual void OnUpdateTarget_Implementation() override;
};
