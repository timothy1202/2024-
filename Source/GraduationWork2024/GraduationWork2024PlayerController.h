// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "GraduationWork2024PlayerController.generated.h"

/** Forward declaration to improve compiling times */
class UInputMappingContext;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS()
class AGraduationWork2024PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AGraduationWork2024PlayerController();

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

protected:
	virtual void SetupInputComponent() override;
	
	// To add mapping context
	virtual void BeginPlay();
};


