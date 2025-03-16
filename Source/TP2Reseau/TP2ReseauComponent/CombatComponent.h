// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TP2Reseau/HUD/TP2HUD.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGHT 10000.f

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TP2RESEAU_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	UCombatComponent();
	friend class ATP2ReseauCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	friend class ATP2ReseauCharacter;
	void EquipWeapon(class AWeapon* WeaponToEquip);

	virtual void BeginPlay() override;
	void SetAiming(bool bAiming);

	UFUNCTION(Server,Reliable)
	void ServerSetAiming(bool bAiming);
protected:
	

	UFUNCTION()
	void OnRep_EquippedWeapon();

	void FireButtonPressed(bool bPressed);

	UFUNCTION(Server,Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast,Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);
	
private:
	class ATP2ReseauCharacter* Character;
	class ATP2PlayerController* Controller;
	class ATP2HUD* HUD;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	class AWeapon* EquippedWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_Aiming)
	bool bIsAiming = false;

	bool bAimButtonPressed = false;
	
	UFUNCTION()
	void OnRep_Aiming();

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	bool bFireButtonPressed;

	/**
	 * Crosshair
	 */

	FHUDPackage HUDPackage;

	/**
	 * Aiming and FOV
	 */

	float DefaultFOV;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomedFOV = 30.f;
	float CurrentFOV;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomInterpSpeed = 20.f;

	void InterFOV(float DeltaTime);
};

