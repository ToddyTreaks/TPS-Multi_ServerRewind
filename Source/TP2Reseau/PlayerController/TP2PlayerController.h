// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TP2PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TP2RESEAU_API ATP2PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDMatchCountdown(float CountdownTime);
	virtual void Tick(float DeltaSeconds) override;
	virtual float GetServerTime(); // Synced with server world clock
	virtual void ReceivedPlayer() override; // Sync with the server clock as soon as possible

	float SingleTripTime = 0.f;
protected:
	virtual void BeginPlay() override;
	void SetHUDTime();

	/**
	 * Syncs the time between client and server
	 */

	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	float ClientServerDelta = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "Time")
	float TimeSyncFrequency = 5.f;

	float TimeSyncRunningTime = 0.f;
	
	void CheckTimeSync(float DeltaSeconds);
	virtual void OnPossess(APawn* InPawn) override;

	/**
	 *  Prints a warning on the HUD if the ping is too high
	 */
	void HighPingWarning();
	void StopHighPingWarning();
	void CheckPing(float DeltaSeconds);
	
private:
	class ATP2HUD* HUD;

	float MatchTime = 120.f;
	uint32 CountdownInt = 0;

	/**
	 *  Ping check
	 */
	float HighPingRunningTime = 0.f;
	UPROPERTY(EditAnywhere)
	float HighPingDuration = 5.f;
	float PingAnimationRunningTime = 0.f;
	UPROPERTY(EditAnywhere)
	float CheckPingFrequency = 10.f;
	UPROPERTY(EditAnywhere)
	float HighPingThreshold = 50.f;
	
};
