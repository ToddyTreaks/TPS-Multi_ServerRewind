// Fill out your copyright notice in the Description page of Project Settings.


#include "TP2PlayerController.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "TP2Reseau/HUD/TP2HUD.h"
#include "TP2Reseau/HUD/CharacterOverlay.h"


void ATP2PlayerController::BeginPlay()
{
	Super::BeginPlay();
}


void ATP2PlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SetHUDTime();
	CheckTimeSync(DeltaSeconds);
	CheckPing(DeltaSeconds);
}



void ATP2PlayerController::CheckPing(float DeltaSeconds)
{
	HighPingRunningTime += DeltaSeconds;
	if (HighPingRunningTime > CheckPingFrequency)
	{
		
		HighPingRunningTime = 0.f;
		PlayerState = PlayerState.Get() == nullptr ? TObjectPtr<APlayerState>(GetPlayerState<APlayerState>()) : PlayerState;
		if (PlayerState)
		{
			if (PlayerState->GetPingInMilliseconds() > HighPingThreshold)
			{
				HighPingWarning();
				PingAnimationRunningTime = 0.f;
				
				if (HUD && HUD->CharacterOverlay && HUD->CharacterOverlay->HighPingImage && HUD->CharacterOverlay->HighPingAnimation)
				{
					HUD->CharacterOverlay->HighPingImage->SetRenderOpacity(1.f);
				}
			}
		}
	}
	else if (HUD && HUD->CharacterOverlay && HUD->CharacterOverlay->HighPingImage && HUD->CharacterOverlay->HighPingAnimation)
	{
		HUD->CharacterOverlay->HighPingImage->SetOpacity(0.f);
	}
	if (HUD && HUD->CharacterOverlay && HUD->CharacterOverlay->HighPingAnimation && HUD->CharacterOverlay->IsAnimationPlaying(HUD->CharacterOverlay->HighPingAnimation))
	{
		PingAnimationRunningTime += DeltaSeconds;
		if (PingAnimationRunningTime > HighPingDuration)
		{
			StopHighPingWarning();
		}
	}
}

void ATP2PlayerController::HighPingWarning()
{
	HUD = HUD == nullptr ? Cast<ATP2HUD>(GetHUD()) : HUD;
	if (HUD && HUD->CharacterOverlay && HUD->CharacterOverlay->HighPingImage && HUD->CharacterOverlay->HighPingAnimation)
	{
		HUD->CharacterOverlay->HighPingImage->SetOpacity(1.f);
		HUD->CharacterOverlay->PlayAnimation(HUD->CharacterOverlay->HighPingAnimation, 0.f, 5);
	}
}

void ATP2PlayerController::StopHighPingWarning()
{
	HUD = HUD == nullptr ? Cast<ATP2HUD>(GetHUD()) : HUD;
	if (HUD && HUD->CharacterOverlay && HUD->CharacterOverlay->HighPingImage)
	{
		
		if (HUD->CharacterOverlay->HighPingAnimation && HUD->CharacterOverlay->IsAnimationPlaying(HUD->CharacterOverlay->HighPingAnimation))
		{
			HUD->CharacterOverlay->StopAnimation(HUD->CharacterOverlay->HighPingAnimation);
		}
	}
}


void ATP2PlayerController::SetHUDTime()
{
	uint32 SecondsLeft = FMath::CeilToInt(MatchTime - GetServerTime());
	if (CountdownInt != SecondsLeft)
	{
		SetHUDMatchCountdown(MatchTime - GetServerTime());
	}
	CountdownInt = SecondsLeft;
}

void ATP2PlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	HUD = HUD == nullptr ? Cast<ATP2HUD>(GetHUD()) : HUD;
	if (HUD && HUD->CharacterOverlay && HUD->CharacterOverlay->HealthBar && HUD->CharacterOverlay->HealthText)
	{
		const float HealthPercentage = Health / MaxHealth;
		HUD->CharacterOverlay->HealthBar->SetPercent(HealthPercentage);
		HUD->CharacterOverlay->HealthText->SetText(FText::FromString(FString::Printf(TEXT("%d/ %d"), FMath::CeilToInt(Health),  FMath::CeilToInt(MaxHealth))));
	}
}

void ATP2PlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	HUD = HUD == nullptr ? Cast<ATP2HUD>(GetHUD()) : HUD;
	if (HUD && HUD->CharacterOverlay && HUD->CharacterOverlay->MatchCountdownText)
	{
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60);
		int32 Seconds = FMath::CeilToInt(FMath::Fmod(CountdownTime, 60));
		
		FString CountdownString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		HUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownString));
	}
}

void ATP2PlayerController::CheckTimeSync(float DeltaSeconds)
{
	TimeSyncRunningTime += DeltaSeconds;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}


void ATP2PlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void ATP2PlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest,
	float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	SingleTripTime = RoundTripTime * 0.5f;
	float CurrentServerTime = TimeServerReceivedClientRequest + SingleTripTime;
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
	
}


float ATP2PlayerController::GetServerTime()
{
	if (HasAuthority())	return GetWorld()->GetTimeSeconds();
	else return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void ATP2PlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ATP2PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	HUD = Cast<ATP2HUD>(GetHUD());
	
}

