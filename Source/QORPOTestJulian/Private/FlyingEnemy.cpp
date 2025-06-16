#include "FlyingEnemy.h"

void AFlyingEnemy::OnUpdateTarget_Implementation()
{
    if (!bEnableStatus)
    {
        return;
    }

    const FVector& CurrentPosition = GetActorLocation();
    float MinDistance = IsValid(CurrentTarget) ? FVector::Distance(CurrentPosition, CurrentTarget->GetActorLocation()) : 999999.0f;
    for (AActor* T : Targets)
    {
        if (T == CurrentTarget || !IsValid(T))
        {
            continue;
        }

        const FVector& TargetPosition = T->GetActorLocation();
        const float CurrentDistance = FVector::Distance(CurrentPosition, TargetPosition);
        if (CurrentDistance < MinDistance)
        {
            MinDistance = CurrentDistance;
            CurrentTarget = T;
        }
    }

    if (IsValid(FloatingMovement) && IsValid(CurrentTarget))
    {
        FloatingMovement->RequestPathMove((CurrentTarget->GetActorLocation() - CurrentPosition).GetSafeNormal());
    }
}
