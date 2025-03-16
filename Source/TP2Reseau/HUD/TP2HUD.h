// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TP2HUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()

public:
	UTexture2D* CrosshairsCenter;
	UTexture2D* CrosshairsLeft;
	UTexture2D* CrosshairsRight;
	UTexture2D* CrosshairsTop;
	UTexture2D* CrosshairsBottom;

	FLinearColor CrosshairColor;
};

/**
 * 
 */
UCLASS()
class TP2RESEAU_API ATP2HUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;
	
	class UCharacterOverlay* CharacterOverlay;

protected:
	virtual void BeginPlay() override;
	void AddCharacterOverlay();
	virtual void Tick(float DeltaSeconds) override;
	virtual void AddLagComponent();
	virtual void ShowPackageHUD(bool bShow, float Duration);

	bool bShowFramePackageStatus = false;

	
private:
	FHUDPackage HUDPackage;

	void DrawCrosshair (UTexture2D* Texture, FVector2d ViewportCenter, FLinearColor CrosshairColor);
	class ULagCompensationComponent* LagCompComponent = nullptr;

public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package;}
};


