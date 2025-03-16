// Fill out your copyright notice in the Description page of Project Settings.


#include "TP2HUD.h"

#include "CharacterOverlay.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

void ATP2HUD::BeginPlay()
{
	Super::BeginPlay();
	AddCharacterOverlay();
}

void ATP2HUD::AddCharacterOverlay()
{
	APlayerController* Controller = GetOwningPlayerController();
	if (Controller && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(Controller, CharacterOverlayClass);
		if (CharacterOverlay)
		{
			CharacterOverlay->AddToViewport();
		}
	}
}
void ATP2HUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2d ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize((ViewportSize));
		const FVector2D ViewportCenter(ViewportSize.X /2.f, ViewportSize.Y / 2.f);

		if (HUDPackage.CrosshairsCenter)
		{
			DrawCrosshair(HUDPackage.CrosshairsCenter, ViewportCenter, HUDPackage.CrosshairColor);
		}
		if (HUDPackage.CrosshairsLeft)
		{
			DrawCrosshair(HUDPackage.CrosshairsLeft, ViewportCenter, HUDPackage.CrosshairColor);
		}
		if (HUDPackage.CrosshairsRight)
		{
			DrawCrosshair(HUDPackage.CrosshairsRight, ViewportCenter, HUDPackage.CrosshairColor);
		}
		if (HUDPackage.CrosshairsTop)
		{
			DrawCrosshair(HUDPackage.CrosshairsTop, ViewportCenter, HUDPackage.CrosshairColor);
		}
		if (HUDPackage.CrosshairsBottom)
		{
			DrawCrosshair(HUDPackage.CrosshairsBottom, ViewportCenter, HUDPackage.CrosshairColor);
		}
	}
}



void ATP2HUD::DrawCrosshair(UTexture2D* Texture, FVector2d ViewportCenter, FLinearColor CrosshairColor)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(ViewportCenter.X - (TextureWidth /2.f), ViewportCenter.Y - (TextureHeight /2.f));
	DrawTexture(Texture, TextureDrawPoint.X, TextureDrawPoint.Y, TextureWidth, TextureHeight, 0.f, 0.f, 1.f, 1.f, CrosshairColor);
	
}
