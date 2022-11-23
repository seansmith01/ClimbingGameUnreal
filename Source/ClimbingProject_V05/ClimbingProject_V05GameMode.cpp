// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClimbingProject_V05GameMode.h"
#include "ClimbingProject_V05Character.h"
#include "UObject/ConstructorHelpers.h"

AClimbingProject_V05GameMode::AClimbingProject_V05GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
