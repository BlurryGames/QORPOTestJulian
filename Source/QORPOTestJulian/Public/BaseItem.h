#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"

#include "BaseItem.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API ABaseItem : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	ABaseItem();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation|Respawn")
	FTimerHandle RespawnTimerHandle = FTimerHandle();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	FVector DisplacementDirection = FVector::UpVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	FVector RotationDirection = FVector::RightVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation|Respawn", meta = (ClampMin = 0.0f, ClampMax = 1000.0f))
	float RespawnTime = 120.0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (ClampMin = 0.0f, ClampMax = 100.0f))
	float DisplacementDistanceLimit = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (ClampMin = 0.0f, ClampMax = 100.0f))
	float DisplacementSpeed = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (ClampMin = 0.0f, ClampMax = 100.0f))
	float RotationSpeed = 4.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bActiveAnimation = true;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnInteractionAnimation_Implementation(const float DeltaTime) override;

	virtual void OnTurnEnabled_Implementation(const bool bEnabled) override;

	UFUNCTION(BlueprintCallable, Category = "Respawn")
	void StartRespawnCountdown();

private:
	FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUFunction(this, GET_FUNCTION_NAME_CHECKED(ABaseItem, OnTurnEnabled), true);
};
