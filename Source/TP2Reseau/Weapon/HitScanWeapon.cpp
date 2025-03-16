// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"

#include "Components/BoxComponent.h"
#include "TP2Reseau/Character/TP2ReseauCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "TP2Reseau/PlayerController/TP2PlayerController.h"
#include "TP2Reseau/TP2ReseauComponent/LagCompensationComponent.h"

AHitScanWeapon::AHitScanWeapon()
{
	SpawnPoint = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SpawnPoint->SetupAttachment(RootComponent);
}

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();
	
	if (SpawnPoint)
	{
		if (InstigatorController)
		{
			FVector Start = SpawnPoint->GetComponentLocation();
			FVector End = Start + (HitTarget - Start) * 1.25f;

			FHitResult FireHit;
			UWorld* World = GetWorld();
			if (World)
			{
				World->LineTraceSingleByChannel(FireHit, Start, End, ECC_Visibility);
				// DrawDebugLine( GetWorld(), Start, End, FColor::Red, false, 5.0f, 0, 2.0f);
				if (FireHit.bBlockingHit)
				{
					ATP2ReseauCharacter* Character = Cast<ATP2ReseauCharacter>(FireHit.GetActor());
					if (Character && InstigatorController)
					{
						if (HasAuthority() && !bUseServerSideRewind)
						{
							UGameplayStatics::ApplyDamage(Character, 100.f, InstigatorController, this, UDamageType::StaticClass());
						}
						if (!HasAuthority() && bUseServerSideRewind)
						{
							TP2OwnerCharacter = TP2OwnerCharacter == nullptr ? Cast<ATP2ReseauCharacter>(OwnerPawn) : TP2OwnerCharacter;
							TP2OwnerController = TP2OwnerController == nullptr ? Cast<ATP2PlayerController>(InstigatorController) : TP2OwnerController;
							
							if (TP2OwnerCharacter && TP2OwnerController && TP2OwnerCharacter->GetLagCompensation())
							{
								TP2OwnerCharacter->GetLagCompensation()->ServerScoreRequest(Character, Start, HitTarget, TP2OwnerController->GetServerTime() - TP2OwnerController->SingleTripTime, this);
								DrawDebugSphere(GetWorld(), HitTarget, 30.f, 12, FColor::Red, false, 5.f, 0, 4.f);
							}
						}
					}
					if (ImpactParticle)
					{
						UGameplayStatics::SpawnEmitterAtLocation(World, ImpactParticle, End, FireHit.ImpactNormal.Rotation());
					}
				}
			}
		}
	}
}
