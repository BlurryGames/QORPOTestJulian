#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "ShooterPlayerController.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class QORPOTESTJULIAN_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AShooterPlayerController();

	UFUNCTION(BlueprintCallable, Category = "Options")
	const bool GetInvertPitch() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Options")
	bool bInvertPitch = false;

	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
};
