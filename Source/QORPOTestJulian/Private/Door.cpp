#include "Door.h"

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

void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADoor, DesiredPosition);
	DOREPLIFETIME(ADoor, DesiredRotation);
	DOREPLIFETIME(ADoor, bActiveAnimation);
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
		bActiveAnimation = true;
	}
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
		bActiveAnimation = false;
	}
}