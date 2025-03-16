// Fill out your copyright notice in the Description page of Project Settings.


#include "LagCompensationComponent.h"

#include "Components/BoxComponent.h"
#include "TP2Reseau/Character/TP2ReseauCharacter.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

ULagCompensationComponent::ULagCompensationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void ULagCompensationComponent::BeginPlay()
{
	Super::BeginPlay();

}


FFramePackage ULagCompensationComponent::InterBetweenFrames(const FFramePackage& Older, const FFramePackage& Younger,
	float HitTime)
{
	const float Distance = Younger.Time - Older.Time;
	const float InterpFraction = FMath::Clamp((HitTime - Older.Time) / Distance, 0.f, 1.f);

	FFramePackage InterpFramePackage;
	InterpFramePackage.Time = HitTime;

	for (auto& YougerPair : Younger.HitBoxInfo)
	{
		const FName& BoxInfoName = YougerPair.Key;
    
		// Ensure the Older frame contains this hitbox info
		if (!Older.HitBoxInfo.Contains(BoxInfoName)) continue;
    
		const FBoxInformation& OlderBox = Older.HitBoxInfo[BoxInfoName];
		const FBoxInformation& YoungerBox = YougerPair.Value; // Already validated
    
		FBoxInformation InterpBoxInfo;
		InterpBoxInfo.Location = FMath::VInterpTo(OlderBox.Location, YoungerBox.Location, 1.f, InterpFraction);
		InterpBoxInfo.Rotation = FMath::RInterpTo(OlderBox.Rotation, YoungerBox.Rotation, 1.f, InterpFraction);
		InterpBoxInfo.BoxExtent = YoungerBox.BoxExtent;

		InterpFramePackage.HitBoxInfo.Add(BoxInfoName, InterpBoxInfo);
	}
	
	return InterpFramePackage;
}

FServerSideRewindResult ULagCompensationComponent::ConfirmHit(
    const FFramePackage& Package,
    class ATP2ReseauCharacter* HitCharacter,
    const FVector_NetQuantize& TraceStart,
    const FVector_NetQuantize& HitLocation)
{
    if (!HitCharacter || !HitCharacter->HitCollisionBoxes.Contains(FName("head"))) 
        return FServerSideRewindResult();

    FFramePackage CurrentFrame;
    CacheBoxPositions(HitCharacter, CurrentFrame);
    MoveBoxes(HitCharacter, Package);
    EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::NoCollision);

    UWorld* World = GetWorld();
    if (!World) 
    {
        ResetHitBoxes(HitCharacter, CurrentFrame);
        EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
        return FServerSideRewindResult{ false, false };
    }

    // Compute the extended trace end point
    const FVector TraceEnd = TraceStart + (HitLocation - TraceStart) * 1.25f;

    // Function to perform the hit confirmation trace
    auto PerformHitCheck = [&](UBoxComponent* CollisionBox) -> bool {
        if (CollisionBox)
        {
            CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Block);
            FHitResult HitResult;
            World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility);
            return HitResult.bBlockingHit;
        }
        return false;
    };

    // Check the head hitbox first
    UBoxComponent* HeadBox = HitCharacter->HitCollisionBoxes[FName("head")];
    if (PerformHitCheck(HeadBox)) 
    {
        ResetHitBoxes(HitCharacter, CurrentFrame);
        EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
        return FServerSideRewindResult{ true, true }; // Headshot confirmed
    }

    // Enable collision for all hitboxes and check for a body shot
    for (auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
    {
        if (HitBoxPair.Key != FName("head") && PerformHitCheck(HitBoxPair.Value)) 
        {
            ResetHitBoxes(HitCharacter, CurrentFrame);
            EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
            return FServerSideRewindResult{ true, true }; // Body hit confirmed
        }
    }

    // No hit detected
    ResetHitBoxes(HitCharacter, CurrentFrame);
    EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
    return FServerSideRewindResult{ false, false };
}


void ULagCompensationComponent::CacheBoxPositions(ATP2ReseauCharacter* HitCharacter, FFramePackage& OutFramePackage)
{
	if (HitCharacter == nullptr) return;
	for (auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
	{
		if (HitBoxPair.Value != nullptr)
		{
			FBoxInformation BoxInfo;
			BoxInfo.Location = HitBoxPair.Value->GetComponentLocation();
			BoxInfo.Rotation = HitBoxPair.Value->GetComponentRotation();
			BoxInfo.BoxExtent = HitBoxPair.Value->GetScaledBoxExtent();
			OutFramePackage.HitBoxInfo.Add(HitBoxPair.Key, BoxInfo);
		}
	}
}

void ULagCompensationComponent::MoveBoxes(ATP2ReseauCharacter* HitCharacter, const FFramePackage& Package)
{
	if (HitCharacter == nullptr) return;
	for (auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
	{
		if (HitBoxPair.Value == nullptr) continue;
    
		if (!Package.HitBoxInfo.Contains(HitBoxPair.Key)) continue;

		HitBoxPair.Value->SetWorldLocation(Package.HitBoxInfo[HitBoxPair.Key].Location);
		HitBoxPair.Value->SetWorldRotation(Package.HitBoxInfo[HitBoxPair.Key].Rotation);
		HitBoxPair.Value->SetBoxExtent(Package.HitBoxInfo[HitBoxPair.Key].BoxExtent);
	}

}

void ULagCompensationComponent::ResetHitBoxes(ATP2ReseauCharacter* HitCharacter, const FFramePackage& Package)
{
	if (HitCharacter == nullptr) return;
	for (auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
	{
		if (HitBoxPair.Value == nullptr) continue;

		if (!Package.HitBoxInfo.Contains(HitBoxPair.Key)) continue;

		HitBoxPair.Value->SetWorldLocation(Package.HitBoxInfo[HitBoxPair.Key].Location);
		HitBoxPair.Value->SetWorldRotation(Package.HitBoxInfo[HitBoxPair.Key].Rotation);
		HitBoxPair.Value->SetBoxExtent(Package.HitBoxInfo[HitBoxPair.Key].BoxExtent);
		HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

}

void ULagCompensationComponent::EnableCharacterMeshCollision(ATP2ReseauCharacter* HitCharacter,
	ECollisionEnabled::Type CollisionEnabled)
{
	if (HitCharacter && HitCharacter->GetMesh())
	{
		HitCharacter->GetMesh()->SetCollisionEnabled(CollisionEnabled);
	}
}


void ULagCompensationComponent::ShowFramePackage(FFramePackage& Package, const FColor& Color)
{
	for (auto& BoxInfo : Package.HitBoxInfo)
	{
		DrawDebugBox(
			GetWorld(),
			BoxInfo.Value.Location,
			BoxInfo.Value.BoxExtent,
			FQuat(BoxInfo.Value.Rotation),
			Color,
			false,
			4.f
		);
	}
}

FServerSideRewindResult ULagCompensationComponent::ServerSideRewind(class ATP2ReseauCharacter* HitCharacter,
	const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime)
{	
	FFramePackage FrameToCheck = GetFrameToCheck(HitCharacter, HitTime);
	return ConfirmHit(FrameToCheck, HitCharacter, TraceStart, HitLocation);
}

FFramePackage ULagCompensationComponent::GetFrameToCheck(ATP2ReseauCharacter* HitCharacter, float HitTime)
{
	bool bReturn =
		HitCharacter == nullptr ||
		HitCharacter->GetLagCompensation() == nullptr ||
		HitCharacter->GetLagCompensation()->FrameHistory.GetHead() == nullptr ||
		HitCharacter->GetLagCompensation()->FrameHistory.GetTail() == nullptr;
	if (bReturn) return FFramePackage();
	// Frame package that we check to verify a hit
	FFramePackage FrameToCheck;
	bool bShouldInterpolate = true;
	// Frame history of the HitCharacter
	const TDoubleLinkedList<FFramePackage>& History = HitCharacter->GetLagCompensation()->FrameHistory;
	const float OldestHistoryTime = History.GetTail()->GetValue().Time;
	const float NewestHistoryTime = History.GetHead()->GetValue().Time;
	if (OldestHistoryTime > HitTime)
	{
		// too far back - too laggy to do SSR
		return FFramePackage();
	}
	if (OldestHistoryTime == HitTime)
	{
		FrameToCheck = History.GetTail()->GetValue();
		bShouldInterpolate = false;
	}
	if (NewestHistoryTime <= HitTime)
	{
		FrameToCheck = History.GetHead()->GetValue();
		bShouldInterpolate = false;
	}

	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Younger = History.GetHead();
	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Older = Younger;
	while (Older->GetValue().Time > HitTime) // is Older still younger than HitTime?
	{
		if (Older->GetNextNode() == nullptr) break; // Prevent null access
		Older = Older->GetNextNode();
		if (Older->GetValue().Time > HitTime)
		{
			Younger = Older;
		}
	}
	if (Older->GetValue().Time == HitTime) // highly unlikely, but we found our frame to check
	{
		FrameToCheck = Older->GetValue();
		bShouldInterpolate = false;
	}
	if (bShouldInterpolate)
	{
		// Interpolate between Younger and Older
		FrameToCheck = InterBetweenFrames(Older->GetValue(), Younger->GetValue(), HitTime);
	}
	FrameToCheck.Character = HitCharacter;
	return FrameToCheck;
}

void ULagCompensationComponent::ServerScoreRequest_Implementation(ATP2ReseauCharacter* HitCharacter,
	const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime,
	class AWeapon* DamageCauser)
{
	FServerSideRewindResult Confirm = ServerSideRewind(HitCharacter, TraceStart, HitLocation, HitTime);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("HERE") );
	if (!Character) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Character is null"));
	if (!HitCharacter) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("HitCharacter is null"));
	if (!DamageCauser) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("DamageCauser is null"));
	if (!Confirm.bHitConfirmed) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Hit not confirmed"));
	
	if (Character && HitCharacter && DamageCauser && Confirm.bHitConfirmed)
	{
		UGameplayStatics::ApplyDamage(HitCharacter, DamageCauser->GetDamage(), Character->Controller, DamageCauser, UDamageType::StaticClass());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Hit Confirmed: %s"), Confirm.bHeadShot ? TEXT("HeadShot") : TEXT("BodyShot")));
	}
}

void ULagCompensationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	SaveFramePackage();
}



void ULagCompensationComponent::SaveFramePackage()
{
	if (Character == nullptr || !Character->HasAuthority()) return;
	if (FrameHistory.Num() <= 1)
	{
		FFramePackage ThisFrame;
		SaveFramePackage(ThisFrame);
		FrameHistory.AddHead(ThisFrame);
	}
	else
	{
		float HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		while (HistoryLength > MaxRecordTime)
		{
			FrameHistory.RemoveNode(FrameHistory.GetTail());
			HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		}
		FFramePackage ThisFrame;
		SaveFramePackage(ThisFrame);
		FrameHistory.AddHead(ThisFrame);

		//ShowFramePackage(ThisFrame, FColor::Red);
	}
}

void ULagCompensationComponent::SaveFramePackage(FFramePackage& Package)
{
	Character = Character == nullptr ? Cast<ATP2ReseauCharacter>(GetOwner()) : Character;
	if (Character)
	{
		Package.Time = GetWorld()->GetTimeSeconds();
		Package.Character = Character;
		for (auto& BoxPair : Character->HitCollisionBoxes)
		{
			FBoxInformation BoxInformation;
			BoxInformation.Location = BoxPair.Value->GetComponentLocation();
			BoxInformation.Rotation = BoxPair.Value->GetComponentRotation();
			BoxInformation.BoxExtent = BoxPair.Value->GetScaledBoxExtent();
			Package.HitBoxInfo.Add(BoxPair.Key, BoxInformation);
		}
	}
}


