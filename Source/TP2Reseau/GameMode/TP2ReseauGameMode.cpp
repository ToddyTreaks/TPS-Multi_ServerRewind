// Copyright Epic Games, Inc. All Rights Reserved.

#include "TP2ReseauGameMode.h"
#include "TP2Reseau/Character/TP2ReseauCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATP2ReseauGameMode::ATP2ReseauGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
