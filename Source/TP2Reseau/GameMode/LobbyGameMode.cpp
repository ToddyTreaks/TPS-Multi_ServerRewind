// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (GameState)
	{
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
		if (NumberOfPlayers == 2)
		{
			UWorld* World = GetWorld();
			if (World)
			{
				bUseSeamlessTravel = true;
				World->ServerTravel("/Game/Maps/GameMap?listen");
			}
		}
		
		if (GEngine)
		{

			if(APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>())
			{
				FString PlayerName = PlayerState->GetPlayerName();
			}
		}
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>())
	{
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Yellow, FString::Printf(TEXT("Number of players: %d"), NumberOfPlayers - 1));

		FString PlayerName = PlayerState->GetPlayerName();
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("%s has exited the game!"), *PlayerName));
	}
}
