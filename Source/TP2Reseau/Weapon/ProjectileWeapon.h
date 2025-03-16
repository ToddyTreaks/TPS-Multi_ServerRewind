// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ProjectileWeapon.generated.h"

UCLASS()
class TP2RESEAU_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	AProjectileWeapon();
	virtual void Fire(const FVector& HitTarget) override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* SpawnPoint;
};
