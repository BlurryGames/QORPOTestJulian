#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "InteractableInterface.h"
#include "ReusableInterface.h"

#include "Door.generated.h"

UCLASS(Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API ADoor : public AActor, public IInteractableInterface, public IReusableInterface
{
	GENERATED_BODY()
	
public:	
	ADoor();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* BoxComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	FVector ClosePositionOffset = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	FVector OpenPositionOffset = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	FVector DesiredPosition = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	FRotator CloseRotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	FRotator OpenRotationOffset = FRotator(0.0f, 90.0f, 0.0f);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	FRotator DesiredRotation = FRotator::ZeroRotator;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (ClampMin = 0.0f, ClampMax = 1000.0f))
	float PositionSpeed = 140.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (ClampMin = 0.0f, ClampMax = 100.0f))
	float RotationSpeed = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (ClampMin = 1.0f, ClampMax = 10.0f))
	float PositionToleranceOffset = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (ClampMin = 0.1f, ClampMax = 1.0f))
	float RotationToleranceOffset = 0.1f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bActiveAnimation = false;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnInteract_Implementation(AActor* Caller) override;

	virtual void OnInteractionAnimation_Implementation(const float DeltaTime) override;

};
