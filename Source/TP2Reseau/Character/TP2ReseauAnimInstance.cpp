// Fill out your copyright notice in the Description page of Project Settings.


#include "TP2ReseauAnimInstance.h"

#include "TP2ReseauCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UTP2ReseauAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	TP2ReseauCharacter = Cast<ATP2ReseauCharacter>(TryGetPawnOwner());
}

void UTP2ReseauAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (TP2ReseauCharacter == nullptr)
	{
		TP2ReseauCharacter = Cast<ATP2ReseauCharacter>(TryGetPawnOwner());
		return;
	}

	FVector Velocity = TP2ReseauCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = TP2ReseauCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = TP2ReseauCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;
	bWeaponEquipped = TP2ReseauCharacter->IsWeaponEquipped();
	bIsAiming = TP2ReseauCharacter->IsAiming();
	bElimmed = TP2ReseauCharacter->IsElimmed();

	// Offset Yaw for Straffing
	FRotator AimRotation = TP2ReseauCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(TP2ReseauCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(AimRotation, MovementRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaSeconds, 6.f);
	
	YawOffset = DeltaRotation.Yaw;
	

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = TP2ReseauCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotationLastFrame, CharacterRotation);
	const float Target = Delta.Yaw / DeltaSeconds;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaSeconds, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);
}
