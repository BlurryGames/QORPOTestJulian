// Copyright (c) Julián López Barañano. All Rights Reserved.

/**
 * @file BaseItem.cpp
 * @brief Implements the logic for the ABaseItem class, which serves as the base class for all interactable items in the game.
 *
 * This class handles initialization, animation, interaction, and respawn logic for items.
 * It is designed to be extended for specific item types and supports both C++ and Blueprint customization.
 */

#include "../Public/BaseItem.h"

/**
 * Default constructor.
 * Initializes components, sets up collision, movement, and replication properties.
 */
ABaseItem::ABaseItem()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	SetReplicateMovement(true);

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(FName("RootComponent")));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeshComponent"));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	MeshComponent->SetupAttachment(GetRootComponent());
}

/**
 * Called when the game starts or when spawned.
 * Registers the mesh as an enabled type and stores the original position and rotation.
 */
void ABaseItem::BeginPlay()
{
	Super::BeginPlay();

	Execute_AddEnabledType(this, MeshComponent);
	Execute_SetOriginalPositionAndRotation(this, GetActorLocation(), GetActorRotation());
}

/**
 * Called every frame.
 * Handles item animation logic.
 *
 * @param DeltaTime Time elapsed since the last tick.
 */
void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Execute_OnInteractionAnimation(this, DeltaTime);
}

/**
 * Called when the item is removed from the world.
 * Clears all timers associated with this item.
 *
 * @param EndPlayReason The reason for removal.
 */
void ABaseItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearAllTimersForObject(this);
}

/**
 * Enables or disables the item and its animation.
 * If disabled, starts the respawn countdown.
 *
 * @param bEnabled Whether the item should be enabled.
 */
void ABaseItem::OnTurnEnabled_Implementation(const bool bEnabled)
{
	IReusableInterface::OnTurnEnabled_Implementation(bEnabled);

	bActiveAnimation = bEnabled;
	if (!bEnabled)
	{
		StartRespawnCountdown();
	}
}

/**
 * Handles the item's interaction animation each tick.
 * Rotates and moves the item for visual feedback.
 *
 * @param DeltaTime Time elapsed since the last tick.
 */
void ABaseItem::OnInteractionAnimation_Implementation(const float DeltaTime)
{
	if (!bActiveAnimation)
	{
		return;
	}

	AddActorLocalRotation(RotationDirection.GetSafeNormal().Rotation() * DeltaTime * RotationSpeed);
	AddActorLocalOffset(DisplacementDirection.GetSafeNormal() * DeltaTime * DisplacementSpeed);
	if (FVector::Distance(OriginalPosition, GetActorLocation()) > DisplacementDistanceLimit)
	{
		DisplacementDirection = -DisplacementDirection;
	}
}

/**
 * Starts the respawn countdown timer for the item.
 * When the timer expires, the item will be re-enabled.
 */
void ABaseItem::StartRespawnCountdown()
{
	FTimerManager& TimerManager = GetWorldTimerManager();
	TimerManager.ClearTimer(RespawnTimerHandle);
	TimerManager.SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnTime, false);
}
