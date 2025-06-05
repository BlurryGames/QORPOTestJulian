#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupInterface.h"
#include "ShooterPlayer.h"

#include "BaseWeapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadSpent, const int, Amount);

UCLASS(Abstract, Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API ABaseWeapon : public AActor, public IPickupInterface
{
	GENERATED_BODY()

public:
	ABaseWeapon();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnReloadSpent OnReloadSpent;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComponent = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* MuzzleComponent = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	FTimerHandle ReloadTimerHandle = FTimerHandle();

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	FTimerHandle CadencyTimerHandle = FTimerHandle();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Stats", meta = (ClampMin = 10.0f, ClampMax = 10000.0f))
	float MaxRange = 10000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Stats", meta = (ClampMin = 0.4f, ClampMax = 10.0f))
	float ReloadTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Stats", meta = (ClampMin = 0.1f, ClampMax = 10.0f))
	float Cadency = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Stats", meta = (ClampMin = 1, ClampMax = 100))
	int ShotCost = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Stats", meta = (ClampMin = 1, ClampMax = 100))
	int MagazineCapacity = 30;

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Stats", meta = (ClampMin = 0, ClampMax = 100))
	int Magazine = MagazineCapacity;

	UPROPERTY(VisibleAnywhere, Category = "Weapon|State")
	bool bActiveTrigger = false;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnPickup_Implementation(AShooterPlayer* Caller) override;
	virtual void OnDrop_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "Weapon|State")
	bool SetEvents(AShooterPlayer* Player, const bool bConnect = true);

	UFUNCTION(BlueprintCallable, Category = "Weapon|State")
	void HandleRealoaded(const int BulletsAmount);

	UFUNCTION(BlueprintCallable, Category = "Weapon|State")
	void HandleShootHeld(const bool bHold);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|State")
	void HandleReloadCompleted(const int BullettsAmount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|State")
	void HandleFire();

private:
	const FTimerDelegate CadencyDelegate = FTimerDelegate::CreateUFunction(this, GET_FUNCTION_NAME_CHECKED(ABaseWeapon, HandleFire));
	FTimerDelegate ReloadDelegate = FTimerDelegate();
};
