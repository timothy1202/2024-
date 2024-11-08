// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "CustomCrowdFollowingComponent.generated.h"

/**
 * 
 */
UCLASS()
class GRADUATIONWORK2024_API UCustomCrowdFollowingComponent : public UCrowdFollowingComponent
{
	GENERATED_BODY()

public:
	int32 GetCrowdAgentAvoidanceGroup() const;
	int32 GetCrowdAgentGroupsToAvoid() const;
	int32 GetCrowdAgentGroupsToIgnore() const;

	
};
