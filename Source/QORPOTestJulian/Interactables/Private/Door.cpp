// Copyright (c) Julián López Barañano. All Rights Reserved.

/**
 * @file Door.cpp
 * @brief Implements the logic for the ADoor class, which represents an interactable and animated door in the game world.
 *
 * This class handles door initialization, animation, interaction, and network replication.
 * It supports both position and rotation changes for opening and closing, and is designed to be extended in C++ or Blueprints.
 */

#include "../Public/Door.h"

/**
 * Default constructor.
 * Initializes components, sets up collision, movement, and replication properties for the door.
 */
ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	SetReplicateMovement(true);

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(FName("RootComponent")));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeshComponent"));
	MeshComponent->SetCanEverAffectNavigation(true);
	MeshComponent->SetMobility(EComponentMobility::Movable);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	MeshComponent->SetupAttachment(GetRootComponent());
	SetRootComponent(MeshComponent);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(FName("BoxComponent"));
	BoxComponent->SetCanEverAffectNavigation(true);
	BoxComponent->bDynamicObstacle = true;
	BoxComponent->SetMobility(EComponentMobility::Movable);
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Block);
	BoxComponent->SetupAttachment(MeshComponent);
}

/**
 * Called when the game starts or when spawned.
 * Registers components for interaction and sets the initial desired position and rotation for the door.
 */
void ADoor::BeginPlay()
{
	Super::BeginPlay();

	Execute_AddEnabledType(this, MeshComponent);
	Execute_AddEnabledType(this, BoxComponent);

	Execute_SetOriginalPositionAndRotation(this, GetActorLocation(), GetActorRotation());

	DesiredPosition = (FVector::Distance(OriginalPosition, OriginalPosition + ClosePositionOffset)
		< FVector::Distance(OriginalPosition, OriginalPosition + OpenPositionOffset)
		? ClosePositionOffset : OpenPositionOffset) + OriginalPosition;

	DesiredRotation = (OriginalRotation.GetManhattanDistance(OriginalRotation + CloseRotationOffset) 
		< OriginalRotation.GetManhattanDistance(OriginalRotation + OpenRotationOffset) 
		? CloseRotationOffset : OpenRotationOffset) + OriginalRotation;
}

/**
 * Registers properties for network replication.
 * @param OutLifetimeProps The array to add replicated properties to.
 */
void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADoor, DesiredPosition);
	DOREPLIFETIME(ADoor, DesiredRotation);
	DOREPLIFETIME(ADoor, bActiveAnimation);
}

/**
 * Called every frame.
 * Handles the door's animation logic for opening and closing.
 * @param DeltaTime Time elapsed since the last tick.
 */
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Execute_OnInteractionAnimation(this, DeltaTime);
}

/**
 * Called when this door is interacted with (e.g., by a player).
 * Toggles the door's open/close state and starts the animation.
 * @param Caller The actor that initiated the interaction.
 */
void ADoor::OnInteract_Implementation(AActor* Caller)
{
	if (IsValid(Caller) && !bActiveAnimation)
	{
		DesiredPosition = (DesiredPosition.Equals(OriginalPosition + ClosePositionOffset, PositionToleranceOffset) ?
			OpenPositionOffset : ClosePositionOffset) + OriginalPosition;
		DesiredRotation = (DesiredRotation.EqualsOrientation(OriginalRotation + CloseRotationOffset, RotationToleranceOffset) ?
			OpenRotationOffset : CloseRotationOffset) + OriginalRotation;
		bActiveAnimation = true;
	}
}

/**
 * Handles the door's animation each tick, moving and rotating the door towards its desired state.
 * Animation completes when both position and rotation reach their targets.
 * @param DeltaTime Time elapsed since the last tick.
 */
void ADoor::OnInteractionAnimation_Implementation(const float DeltaTime)
{
	if (!bActiveAnimation)
	{
		return;
	}

	const FVector& CurrentPosition = GetActorLocation();
	const bool bPositionComplete = CurrentPosition.Equals(DesiredPosition, PositionToleranceOffset);
	if (!bPositionComplete)
	{
		AddActorLocalOffset((DesiredPosition - CurrentPosition).GetSafeNormal() * DeltaTime * PositionSpeed, true);
	}
	else if (CurrentPosition != DesiredPosition)
	{
		SetActorLocation(DesiredPosition, true);
	}

	const FRotator& CurrentRotation = GetActorRotation();
	const bool bRotationComplete = CurrentRotation.EqualsOrientation(DesiredRotation, RotationToleranceOffset);
	if (!bRotationComplete)
	{
		AddActorLocalRotation((DesiredRotation - CurrentRotation).GetNormalized() * DeltaTime * RotationSpeed, true);
	}
	else if (CurrentRotation != DesiredRotation)
	{
		SetActorRotation(DesiredRotation);
	}

	if (bPositionComplete && bRotationComplete)
	{
		bActiveAnimation = false;
	}
}