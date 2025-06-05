#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ShooterPlayerController.h"
#include "AttributesComponent.h"

#include "ShooterPlayer.generated.h"

class ABaseWeapon;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloaded, const int, Amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShootHeld, const bool, bHold);

UCLASS(Abstract, Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API AShooterPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnReloaded OnReloaded;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnShootHeld OnShootHeld;

	AShooterPlayer();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAttributesComponent* AttributesComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* WeaponSocketComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Equipment")
	ABaseWeapon* CurrentWeapon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Movement")
	FVector MovementDirection = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Movement")
	float DefaultSpeed = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|Movement", meta = (ClampMin = 1.1f, ClampMax = 10.0f))
	float SprintMultiplier = 2.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|Equipment", meta = (ClampMin = 0, ClampMax = 10000))
	int Ammunition = 120;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void AddControllerPitchInput(float Value) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetMovementDirection() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void EquipWeapon(ABaseWeapon* Weapon);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void ReloadSpent(int Amount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void UnequipWeapon();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void HandleHealthChange(const float HealthResult);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void HandleMoveForward();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void HandleMoveBackward();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void HandleMoveRight();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void HandleMoveLeft();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void HandleSprint();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void HandleCrouch();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void HandleJump();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void HandleStartShoot();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void HandleStopShoot();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void HandleReload();
};
