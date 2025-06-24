#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "HealEvent.h"
#include "AttributesComponent.h"

#include "ShooterPlayer.generated.h"

class ABaseWeapon;
class ADoor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponMagazineUpdated, const int, Magazine);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerAmmunitionUpdated, const int, Amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadSpent, const int, Amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShootHeld, const bool, bHold);

UCLASS(Blueprintable, BlueprintType)
class QORPOTESTJULIAN_API AShooterPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPlayerAmmunitionUpdated OnPlayerAmmunitionUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWeaponMagazineUpdated OnWeaponMagazineUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnReloadSpent OnReloadSpent;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnShootHeld OnShootHeld;

	AShooterPlayer();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	UAttributesComponent* GetAttributesComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Stats|Equipment")
	const int GetAmmunition() const;

	UFUNCTION(BlueprintCallable, Category = "Interction")
	void AddAmmunition(const int Amount);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* WeaponSocketComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAttributesComponent* AttributesComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Equipment")
	ABaseWeapon* CurrentWeapon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Movement")
	FVector MovementDirection = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	FHitResult LineTraceHitInteraction = FHitResult();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction", meta = (ClampMin = 0.0f, ClampMax = 1000.0f))
	float InteractionRange = 200.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Movement")
	float DefaultSpeed = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|Movement", meta = (ClampMin = 1.1f, ClampMax = 10.0f))
	float SprintMultiplier = 1.6f;

	UPROPERTY(ReplicatedUsing = OnReplicateAmmunition, EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|Equipment", meta = (ClampMin = 0, ClampMax = 10000))
	int Ammunition = 0;

	FCollisionObjectQueryParams ObjectParams = FCollisionObjectQueryParams(ECC_WorldDynamic);

	FCollisionQueryParams LineTraceParams = FCollisionQueryParams();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void AddControllerPitchInput(float Value) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetMovementDirection() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnEquipWeapon(ABaseWeapon* Weapon);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnUnequipWeapon();

	UFUNCTION(BlueprintCallable, Category = "Stats|Equipment")
	void OnReplicateAmmunition();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void HandleHealthChange(const float HealthResult, const float TotalHealth);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void HandleReloaded(const int AmmunitionSpent);

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

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Interaction")
	void HandleStartShoot();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Interaction")
	void HandleStopShoot();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Interaction")
	void HandleReload();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void HandleInteraction();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Interaction")
	void Server_DoorInteraction(ADoor* Door);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Movement")
	void Server_UpdatePitchView(const float PitchInput);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Movement")
	void Multicast_UpdatePitchView(const float PitchInput);
};
