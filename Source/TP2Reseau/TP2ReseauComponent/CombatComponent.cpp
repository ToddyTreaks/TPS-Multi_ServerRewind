// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "TP2Reseau/Character/TP2ReseauCharacter.h"
#include "TP2Reseau/Weapon/Weapon.h"
#include "TP2Reseau/PlayerController/TP2PlayerController.h"


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bIsAiming);
}



void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	

	Character = Cast<ATP2ReseauCharacter>(GetOwner());
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
		if (Character->GetFollowCamera())
		{
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
	}
}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Character && Character->IsLocallyControlled())
	{
		
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		
		SetHUDCrosshairs(DeltaTime);
		InterFOV(DeltaTime);
	}
}


void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (!Character)
	{
		return;
	}
	Controller = Controller == nullptr ? Cast<ATP2PlayerController>(Character->Controller) : Controller;
	if (!Controller)
	{
		return;
	}
	HUD = HUD == nullptr ? Cast<ATP2HUD>(Controller->GetHUD()) : HUD;
	if (!HUD)
	{
		return;
	}
	if (!EquippedWeapon)
	{
		return;
	}
	
	HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
	HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
	HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
	HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
	HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;

	HUD->SetHUDPackage(HUDPackage);
}



void UCombatComponent::SetAiming(bool bAiming)
{
	bIsAiming = bAiming;
	
	ServerSetAiming(bIsAiming);
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming?  AimWalkSpeed : BaseWalkSpeed;
	}
	if (Character->IsLocallyControlled())  bAimButtonPressed = bAiming; // For the local Character
}


void UCombatComponent::OnRep_Aiming()
{
	if (Character && Character->IsLocallyControlled())
	{
		bIsAiming = bAimButtonPressed;
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool bAiming)
{
	bIsAiming = bAiming;
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming?  AimWalkSpeed : BaseWalkSpeed;
	}
}


void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && Character)
	{
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;

	if (bFireButtonPressed)
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		ServerFire(HitResult.ImpactPoint);
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon == nullptr) return;
	if (Character)
	{
		Character->PlayFireMontage(bIsAiming);
		EquippedWeapon->Fire(TraceHitTarget);
	}
}


void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2d ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2d CrosshairLocation(ViewportSize.X / 2, ViewportSize.Y / 2);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this,0), CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		FVector TraceStart = CrosshairWorldPosition;
		if (Character)
        {
            float DistanceToCharacter = (Character->GetActorLocation() - TraceStart).Size();
			TraceStart += CrosshairWorldDirection * (DistanceToCharacter + 100.f);
        }
		FVector TraceEnd = CrosshairWorldPosition + CrosshairWorldDirection * TRACE_LENGHT;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(GetOwner());
		GetWorld()->LineTraceSingleByChannel(TraceHitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = TraceEnd;
		}
		
		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairInterface>())
		{
			HUDPackage.CrosshairColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.CrosshairColor = FLinearColor::White;
		}
	}
	
}



void UCombatComponent::EquipWeapon(class AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;
	EquippedWeapon = WeaponToEquip;

	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->GetAreaSphere()->SetCollisionResponseToAllChannels(ECR_Ignore);
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

void UCombatComponent::InterFOV(float DeltaTime)
{
	if (EquippedWeapon == nullptr) return;
	if (bIsAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), DeltaTime, EquippedWeapon->GetZoomInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, EquippedWeapon->GetZoomInterpSpeed());
	}

	if (Character && Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}

