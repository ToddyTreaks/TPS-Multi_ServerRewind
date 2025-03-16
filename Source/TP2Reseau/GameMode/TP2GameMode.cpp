// Fill out your copyright notice in the Description page of Project Settings.


#include "TP2GameMode.h"

#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "TP2Reseau/Character/TP2ReseauCharacter.h"
#include "TP2Reseau/PlayerController/TP2PlayerController.h"

ATP2GameMode::ATP2GameMode()
{
	bDelayedStart = true;
}


void ATP2GameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void ATP2GameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountDownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountDownTime <= 0.f)
		{
			StartMatch();
		}
	}
}

void ATP2GameMode::PlayerEliminated(class ATP2ReseauCharacter* EliminatedPlayer,
                                    class ATP2PlayerController* VictimController, class ATP2PlayerController* AttackerController)
{
	if (EliminatedPlayer)
	{
		EliminatedPlayer->Elim();
	}
}

void ATP2GameMode::RequestRespawn(class ACharacter* ElimmedCharacter, class AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
		
	}
	if (ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
		
	}
}

