#include "ReusableInterface.h"
#include "ShooterPlayerController.h"

FVector IReusableInterface::GetOriginalPosition_Implementation()
{
	return OriginalPosition;
}

FRotator IReusableInterface::GetOriginalRotation_Implementation()
{
	return OriginalRotation;
}

void IReusableInterface::SetOriginalPositionAndRotation_Implementation(FVector Position, FRotator Rotation)
{
	if (OriginalPosition != Position)
	{
		OriginalPosition = Position;
	}

	if (OriginalRotation != Rotation)
	{
		OriginalRotation = Rotation;
	}
}

void IReusableInterface::SetOriginalPosition_Implementation(FVector Position)
{
	if (OriginalPosition != Position)
	{
		OriginalPosition = Position;
	}
}

void IReusableInterface::SetOriginalRotation_Implementation(FRotator Rotation)
{
	if (OriginalRotation != Rotation)
	{
		OriginalRotation = Rotation;
	}
}

void IReusableInterface::OnTurnEnabled_Implementation(const bool bEnabled)
{
	bEnableStatus = bEnabled;
	AActor* Self = Cast<AActor>(this);
	if (IsValid(Self))
	{
		if (bEnabled)
		{
			Self->SetActorLocation(OriginalPosition);
			Self->SetActorRotation(OriginalRotation);
		}

		Self->SetHidden(!bEnabled);
		Self->SetActorTickEnabled(bEnabled);
		Self->SetActorEnableCollision(bEnabled);
	}
	
	TArray<UPrimitiveComponent*> Keys = TArray<UPrimitiveComponent*>();
	CollisionEnabledTypes.GetKeys(Keys);
	for (UPrimitiveComponent* P : Keys)
	{
		if (IsValid(P))
		{
			P->SetHiddenInGame(!bEnabled);
			P->SetCollisionEnabled(bEnabled ? CollisionEnabledTypes[P].GetValue() : ECollisionEnabled::NoCollision);
			P->UpdateOverlaps();
		}
	}
}

void IReusableInterface::AddEnabledType_Implementation(UPrimitiveComponent* PrimitiveComponent)
{
	if (IsValid(PrimitiveComponent))
	{
		CollisionEnabledTypes.FindOrAdd(PrimitiveComponent);
		CollisionEnabledTypes[PrimitiveComponent] = PrimitiveComponent->GetCollisionEnabled();
	}
}

void IReusableInterface::DoDamage_Implementation(AActor* DamageReceiver, float DamageAmount, FDamageEvent const& DamageEvent)
{
	AActor* Self = Cast<AActor>(this);
	if (!IsValid(Self))
	{
		return;
	}

	AShooterPlayerController* ShooterPlayerController = Self->GetInstigatorController<AShooterPlayerController>();
	if (IsValid(ShooterPlayerController))
	{
		ShooterPlayerController->Server_DoDamage(DamageReceiver, DamageAmount, DamageEvent, ShooterPlayerController, Self);
	}
}