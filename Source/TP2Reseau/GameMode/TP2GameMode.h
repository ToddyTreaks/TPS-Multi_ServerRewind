// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TP2GameMode.generated.h"

/**
 * 
 */
UCLASS()
class TP2RESEAU_API ATP2GameMode : public AGameMode
{
	GENERATED_BODY()

public :
	ATP2GameMode();
	virtual  void Tick(float DeltaSeconds) override;
	virtual void PlayerEliminated(class ATP2ReseauCharacter* EliminatedPlayer, class ATP2PlayerController* VictimController, class ATP2PlayerController* AttackerController);
	virtual void RequestRespawn(class ACharacter* ElimmedCharacter, class AController* ElimmedController);

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 5.f;

	float LevelStartingTime = 0.f;

protected:
	virtual void BeginPlay() override;
	

private:
	float CountDownTime = 0.f;
};
