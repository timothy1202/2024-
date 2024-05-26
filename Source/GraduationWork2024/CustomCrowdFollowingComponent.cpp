// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCrowdFollowingComponent.h"

int32 UCustomCrowdFollowingComponent::GetCrowdAgentAvoidanceGroup() const
{
	return 1;
}

int32 UCustomCrowdFollowingComponent::GetCrowdAgentGroupsToAvoid() const
{
	return 1;
}

int32 UCustomCrowdFollowingComponent::GetCrowdAgentGroupsToIgnore() const
{
	return 0;
}
