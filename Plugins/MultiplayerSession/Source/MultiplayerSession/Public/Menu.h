// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSION_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumberOfPublicConnections = 4, FString TypeOffMatch = "ToddyFFA", FString LobbyPath = FString(TEXT("/Game/ThirdPerson/Maps/Lobby")));


protected:

	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	/**
	 * Callbacks for the custom delegates
	 */
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResult, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);
	
private:

	UFUNCTION(BlueprintCallable)
	void HostButtonClicked();
	
	UFUNCTION(BlueprintCallable)
	void JoinButtonClicked();

	

	

	void MenuTearDown();

	// Subsystem that handle online session functionality.
	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	int32 NumPublicConnections{4};
	FString MatchType{"ToddyFFA"};
	FString PathToLobby{TEXT("")};
	
};


