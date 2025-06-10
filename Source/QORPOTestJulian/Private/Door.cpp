#include "Door.h"

ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(FName("RootComponent")));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeshComponent"));
	MeshComponent->SetupAttachment(GetRootComponent());

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(FName("BoxComponent"));
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Block);
	BoxComponent->SetupAttachment(MeshComponent);
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	OriginalPosition = GetActorLocation();
	OriginalRotation = GetActorRotation();

	DesiredPosition = (FVector::Distance(OriginalPosition, OriginalPosition + ClosePositionOffset)
		< FVector::Distance(OriginalPosition, OriginalPosition + OpenPositionOffset)
		? ClosePositionOffset : OpenPositionOffset) + OriginalPosition;

	DesiredRotation = (OriginalRotation.GetManhattanDistance(OriginalRotation + CloseRotationOffset) 
		< OriginalRotation.GetManhattanDistance(OriginalRotation + OpenRotationOffset) 
		? CloseRotationOffset : OpenRotationOffset) + OriginalRotation;
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Execute_OnInteractionAnimation(this, DeltaTime);
}

void ADoor::OnInteract_Implementation(AActor* Caller)
{
	if (IsValid(Caller) && !bActiveAnimation)
	{
		DesiredPosition = (DesiredPosition.Equals(OriginalPosition + ClosePositionOffset, PositionToleranceOffset) ?
			OpenPositionOffset : ClosePositionOffset) + OriginalPosition;
		DesiredRotation = (DesiredRotation.EqualsOrientation(OriginalRotation + CloseRotationOffset, RotationToleranceOffset) ?
			OpenRotationOffset : CloseRotationOffset) + OriginalRotation;
		Execute_OnTurnEnabled(this, true);
	}
}

void ADoor::OnTurnEnabled_Implementation(const bool bEnabled)
{
	bActiveAnimation = bEnabled;
}

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
		Execute_OnTurnEnabled(this, false);
	}
}