// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class TP2RESEAU_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	AHitScanWeapon();
	virtual void Fire(const FVector& HitTarget);
private :

	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticle;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* SpawnPoint;
};
