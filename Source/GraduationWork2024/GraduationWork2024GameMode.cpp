// Copyright Epic Games, Inc. All Rights Reserved.

#include "GraduationWork2024GameMode.h"
#include "GraduationWork2024PlayerController.h"
#include "GraduationWork2024Character.h"
#include "UObject/ConstructorHelpers.h"

AGraduationWork2024GameMode::AGraduationWork2024GameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AGraduationWork2024PlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_Player"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_PlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}