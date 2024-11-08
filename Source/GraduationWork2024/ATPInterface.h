// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ATPInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UATPInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GRADUATIONWORK2024_API IATPInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "ATP")
	int GetATP() const;
};
