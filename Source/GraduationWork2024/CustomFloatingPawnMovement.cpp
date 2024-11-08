// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomFloatingPawnMovement.h"
#include "Components/CapsuleComponent.h"
#include "EnemyBase.h"

void UCustomFloatingPawnMovement::PostLoad()
{
    Super::PostLoad();

    CharacterOwner = Cast<AEnemyBase>(PawnOwner);
}

void UCustomFloatingPawnMovement::SetRVOAvoidanceUID(int32 UID)
{
    AvoidanceUID = UID;
}

int32 UCustomFloatingPawnMovement::GetRVOAvoidanceUID()
{
    return AvoidanceUID;
}

void UCustomFloatingPawnMovement::SetRVOAvoidanceWeight(float Weight)
{
    AvoidanceWeight = Weight;
}

float UCustomFloatingPawnMovement::GetRVOAvoidanceWeight()
{
    return AvoidanceWeight;
}

FVector UCustomFloatingPawnMovement::GetRVOAvoidanceOrigin()
{
    return GetActorFeetLocation();
}

float UCustomFloatingPawnMovement::GetRVOAvoidanceRadius()
{
    UCapsuleComponent* CapsuleComp = GetCharacterOwner()->GetCapsule();
    return CapsuleComp ? CapsuleComp->GetScaledCapsuleRadius() : 0.0f;
}

float UCustomFloatingPawnMovement::GetRVOAvoidanceHeight()
{
    UCapsuleComponent* CapsuleComp = GetCharacterOwner()->GetCapsule();
    return CapsuleComp ? CapsuleComp->GetScaledCapsuleHalfHeight() : 0.0f;
}

float UCustomFloatingPawnMovement::GetRVOAvoidanceConsiderationRadius()
{
    return AvoidanceConsiderationRadius;
}

FVector UCustomFloatingPawnMovement::GetVelocityForRVOConsideration()
{
    return Velocity;
}

void UCustomFloatingPawnMovement::SetAvoidanceGroupMask(int32 GroupFlags)
{
    AvoidanceGroup.SetFlagsDirectly(GroupFlags);
}

int32 UCustomFloatingPawnMovement::GetAvoidanceGroupMask()
{
    return AvoidanceGroup.Packed;
}

void UCustomFloatingPawnMovement::SetGroupsToAvoidMask(int32 GroupFlags)
{
    GroupsToAvoid.SetFlagsDirectly(GroupFlags);
}

int32 UCustomFloatingPawnMovement::GetGroupsToAvoidMask()
{
    return GroupsToAvoid.Packed;
}

void UCustomFloatingPawnMovement::SetGroupsToIgnoreMask(int32 GroupFlags)
{
    GroupsToIgnore.SetFlagsDirectly(GroupFlags);
}

int32 UCustomFloatingPawnMovement::GetGroupsToIgnoreMask()
{
    return GroupsToIgnore.Packed;
}
