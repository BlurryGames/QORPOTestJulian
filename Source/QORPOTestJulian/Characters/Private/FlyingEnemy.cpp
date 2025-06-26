// Copyright (c) Julián López Barañano. All Rights Reserved.

/**
 * @file FlyingEnemy.cpp
 * @brief Implements the logic for the AFlyingEnemy class, a specialized enemy with flying movement and targeting behavior.
 *
 * This class overrides the target update logic to provide custom movement for flying enemies.
 * It selects the closest valid target and moves towards it using the floating movement component.
 */

#include "../Public/FlyingEnemy.h"

/**
 * Updates the current target for the flying enemy and moves towards it.
 *
 * This implementation selects the closest valid target from the Targets array.
 * If a valid target is found, the enemy requests movement in the direction of the target using the floating movement component.
 * The function does nothing if the enemy is not enabled or hasn't Authority.
 */
void AFlyingEnemy::OnUpdateTarget_Implementation()
{
    if (!HasAuthority() && !bEnableStatus)
    {
        return;
    }

    const FVector& CurrentPosition = GetActorLocation();
    float MinDistance = IsValid(CurrentTarget) ? FVector::Distance(CurrentPosition, CurrentTarget->GetActorLocation()) : float(INT_MAX);
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
