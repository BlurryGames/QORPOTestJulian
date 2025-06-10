#include "BaseItem.h"

ABaseItem::ABaseItem()
{
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(FName("RootComponent")));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeshComponent"));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	MeshComponent->SetupAttachment(GetRootComponent());
}

void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
	OriginalPosition = GetActorLocation();
	OriginalRotation = GetActorRotation();
}

void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Execute_OnInteractionAnimation(this, DeltaTime);
}

void ABaseItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorldTimerManager().ClearAllTimersForObject(this);
}

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

void ABaseItem::OnTurnEnabled_Implementation(const bool bEnabled)
{
	IInteractableInterface::OnTurnEnabled_Implementation(bEnabled);
	if (IsValid(MeshComponent))
	{
		MeshComponent->SetVisibility(bEnabled);
		MeshComponent->SetCollisionEnabled(bEnabled ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	}

	bActiveAnimation = bEnabled;
	if (!bEnabled)
	{
		StartRespawnCountdown();
	}
}

void ABaseItem::StartRespawnCountdown()
{
	FTimerManager& TimerManager = GetWorldTimerManager();
	TimerManager.ClearTimer(RespawnTimerHandle);
	TimerManager.SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnTime, false);
}
