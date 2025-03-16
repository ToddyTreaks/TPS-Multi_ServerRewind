// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "Logging/LogMacros.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "TP2Reseau/Weapon/Weapon.h"
#include "TP2Reseau/Interfaces/InteractWithCrosshairInterface.h"
#include "TP2ReseauCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ATP2ReseauCharacter : public ACharacter, public IInteractWithCrosshairInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;


public:
	ATP2ReseauCharacter();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void PlayFireMontage(bool bIsAiming);
	void PlayElimMontage();
	void Elim();
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastElim();

	UPROPERTY()
	TMap<FName, class UBoxComponent*> HitCollisionBoxes;

protected:

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Interact(const FInputActionValue& Value);
	void Aim(const FInputActionValue& Value);
	void AimStop(const FInputActionValue& Value);
	void StartFiring(const FInputActionValue& Value);
	void StopFiring(const FInputActionValue& Value);
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;
	void PlayHitReactMontage();
	
	UFUNCTION()
	void ReceiveDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
	void UpdateHUDHealth();

	/**
	* HitBoxes for server side rewind
	*/
	UPROPERTY(EditAnywhere)
	UBoxComponent* head;
	UPROPERTY(EditAnywhere)
	UBoxComponent* pelvis;
	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_02;
	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_03;
	UPROPERTY(EditAnywhere)
	UBoxComponent* upperarm_l;
	UPROPERTY(EditAnywhere)
	UBoxComponent* upperarm_r;
	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_l;
	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_r;
	UPROPERTY(EditAnywhere)
	UBoxComponent* hand_l;
	UPROPERTY(EditAnywhere)
	UBoxComponent* hand_r;
	UPROPERTY(EditAnywhere)
	UBoxComponent* backpack;
	UPROPERTY(EditAnywhere)
	UBoxComponent* blanket;
	UPROPERTY(EditAnywhere)
	UBoxComponent* thigh_l;
	UPROPERTY(EditAnywhere)
	UBoxComponent* thigh_r;
	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_l;
	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_r;
	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_l;
	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_r;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;
	
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastOverlappingWeapon);

	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;

	UPROPERTY(VisibleAnywhere)
	class ULagCompensationComponent* LagCompensation;

	UFUNCTION(Server, Reliable)
	void ServerInteract();

	UPROPERTY(EditAnywhere, Category = "Combat")
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	class UAnimMontage* HitReactMontage;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	class UAnimMontage* ElimMontage;
	
	void HideCameraIfCharacterClose();

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 150.f;

	/*
	 * Player Health
	 */

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health = MaxHealth;

	UFUNCTION()
	void OnRep_Health();

	class ATP2PlayerController* TP2PlayerController;

	bool bElimmed = false;

	FTimerHandle ElimTimer;
	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = .1f;
	void ElimTimerFinished();

	
public:
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();

	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE ULagCompensationComponent* GetLagCompensation() const { return LagCompensation; }
	
};










