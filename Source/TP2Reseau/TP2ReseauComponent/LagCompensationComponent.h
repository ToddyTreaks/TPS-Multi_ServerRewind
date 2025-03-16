// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TP2Reseau/Character/TP2ReseauCharacter.h"
#include "LagCompensationComponent.generated.h"


USTRUCT(BlueprintType)
struct FBoxInformation
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;
	UPROPERTY()
	FRotator Rotation;
	UPROPERTY()
	FVector BoxExtent;
};

USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()

	UPROPERTY()
	float Time;

	TMap<FName, FBoxInformation> HitBoxInfo;

	UPROPERTY()
	ATP2ReseauCharacter* Character;
};

USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	bool bHitConfirmed;

	UPROPERTY()
	bool bHeadShot;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TP2RESEAU_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULagCompensationComponent();
	friend class ATP2ReseauCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ShowFramePackage(FFramePackage& Package, const FColor& Color, float Duration);
	FServerSideRewindResult ServerSideRewind(class ATP2ReseauCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime);
	UFUNCTION(Server, Reliable)
	void ServerScoreRequest(ATP2ReseauCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime, class AWeapon* DamageCauser);
protected:
	virtual void BeginPlay() override;
	void SaveFramePackage(FFramePackage& Package);
	FFramePackage InterBetweenFrames(const FFramePackage& Older, const FFramePackage& Younger, float HitTime);
	FServerSideRewindResult ConfirmHit(const FFramePackage& Package, class ATP2ReseauCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation);
	void CacheBoxPositions(ATP2ReseauCharacter* HitCharacter, FFramePackage& OutFramePackage);
	void MoveBoxes(ATP2ReseauCharacter* HitCharacter,const FFramePackage& Package);
	void ResetHitBoxes(ATP2ReseauCharacter* HitCharacter, const FFramePackage& Package);
	void EnableCharacterMeshCollision(ATP2ReseauCharacter* HitCharacter, ECollisionEnabled::Type CollisionEnabled);
	void SaveFramePackage();
	FFramePackage GetFrameToCheck(ATP2ReseauCharacter* HitCharacter, float HitTime);

private:
	UPROPERTY()
	ATP2ReseauCharacter* Character;

	UPROPERTY()
	class ATP2PlayerController* Controller;

	TDoubleLinkedList<FFramePackage> FrameHistory;

	UPROPERTY(EditAnywhere)
	float MaxRecordTime = 2.f;

	
	UPROPERTY(EditAnywhere)
	bool bShowPackage = false;

	UPROPERTY(EditAnywhere)
	float fShowPackageDuration = 1.f;

public:	

	FORCEINLINE void SetShowPackage(bool bShow) { bShowPackage = bShow; }
	FORCEINLINE void SetShowPackageDuration(float Duration) { fShowPackageDuration = Duration; }
};
