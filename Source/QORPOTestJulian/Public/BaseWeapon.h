#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "BaseItem.h"

#include "BaseWeapon.generated.h"

class AShooterPlayer;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadSpent, const int, Amount);

UCLASS(Abstract, Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API ABaseWeapon : public ABaseItem
{
	GENERATED_BODY()

public:
	ABaseWeapon();

	virtual void SetOwner(AActor* NewOwner) override;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnReloadSpent OnReloadSpent;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* MuzzleComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAudioComponent* AudioComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	FTimerHandle ReloadTimerHandle = FTimerHandle();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	FTimerHandle CadencyTimerHandle = FTimerHandle();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	FTimerHandle IntervalTimerHandle = FTimerHandle();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stats", meta = (ClampMin = 10.0f, ClampMax = 10000.0f))
	float MaxRange = 10000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stats", meta = (ClampMin = 0.4f, ClampMax = 10.0f))
	float ReloadTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stats", meta = (ClampMin = 0.1f, ClampMax = 10.0f))
	float CadencyTime = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stats", meta = (ClampMin = 0.0f, ClampMax = 1.0f))
	float IntervalProportionTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stats", meta = (ClampMin = 1, ClampMax = 100))
	int ShotCost = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stats", meta = (ClampMin = 1, ClampMax = 100))
	int MagazineCapacity = 30;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Stats")
	int Magazine = MagazineCapacity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Stats")
	int IntervalCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|State")
	bool bActiveTrigger = false;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnInteract_Implementation(AActor* Caller) override;

	UFUNCTION(BlueprintCallable, Category = "Weapon|State")
	bool SetEvents(AShooterPlayer* Player, const bool bConnect = true);

	UFUNCTION(BlueprintCallable, Category = "Weapon|State")
	void HandleRealoaded(const int BulletsAmount);

	UFUNCTION(BlueprintCallable, Category = "Weapon|State")
	void HandleShootHeld(const bool bHold);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|State")
	void HandleReloadCompleted(const int BullettsAmount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|State")
	bool HandleFire();

private:
	const FTimerDelegate CadencyDelegate = FTimerDelegate::CreateUFunction(this, GET_FUNCTION_NAME_CHECKED(ABaseWeapon, HandleFire));
	FTimerDelegate ReloadDelegate = FTimerDelegate();
};
