#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "ShooterPlayerController.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AShooterPlayerController();

	UFUNCTION(BlueprintCallable, Category = "Options")
	const bool GetInvertPitch() const;

	UFUNCTION(BlueprintCallable, Category = "Data")
	void UpdateScore(const float Points);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data", meta = (ClampMin = 0.0f, ClampMax = 9999999999.0f))
	float Score = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data", meta = (ClampMin = 0.0f, ClampMax = 9999999999.0f))
	float SurviveTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Options")
	bool bInvertPitch = false;

	virtual void SetupInputComponent() override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
};
