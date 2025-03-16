// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"

#include "Projectile.h"
#include "Components/BoxComponent.h"

AProjectileWeapon::AProjectileWeapon()
{
	SpawnPoint = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SpawnPoint->SetupAttachment(RootComponent);
	SpawnPoint->SetCollisionObjectType(ECC_WorldDynamic);
	SpawnPoint->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SpawnPoint->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	if (!HasAuthority()) return;

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	if (ProjectileClass && InstigatorPawn)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = GetOwner();
		SpawnParameters.Instigator = InstigatorPawn;

		FVector SpawnLocation = SpawnPoint->GetComponentLocation();
		FVector MuzzleToTarget = (HitTarget - SpawnLocation).GetSafeNormal();
		FRotator SpawnRotation = MuzzleToTarget.Rotation();


		UWorld* World = GetWorld();
		if (World)
		{
			AProjectile* Projectile = World->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParameters);
			if (Projectile)
			{
				// Ensure the projectile moves forward in the expected direction
				Projectile->SetActorRotation(SpawnRotation);
				Projectile->SetActorScale3D(FVector(0.5f, 0.5f, 0.5f));
			}
		}
	}
	
}
