// Copyright Epic Games, Inc. All Rights Reserved.

#include "TP2ReseauCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "TP2Reseau/TP2ReseauComponent/CombatComponent.h"
#include "TP2Reseau/Weapon/Weapon.h"
#include "TP2Reseau/TP2Reseau.h"
#include "TP2Reseau/PlayerController/TP2PlayerController.h"
#include "TP2Reseau/GameMode/TP2GameMode.h"
#include "TimerManager.h"
#include "Components/BoxComponent.h"
#include "TP2Reseau/TP2ReseauComponent/LagCompensationComponent.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ATP2ReseauCharacter

ATP2ReseauCharacter::ATP2ReseauCharacter()
{

	PrimaryActorTick.bCanEverTick = true;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	LagCompensation = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensation"));
	LagCompensation->SetIsReplicated(true);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	/**
	 * Hit boxes for server side rewind
	 */

	head = CreateDefaultSubobject<UBoxComponent>(TEXT("head"));
	head->SetupAttachment(GetMesh(), "head");
	head->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("head"), head);
	
	pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("pelvis"));
	pelvis->SetupAttachment(GetMesh(), "pelvis");
	pelvis->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("pelvis"), pelvis);
	
	spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_02"));
	spine_02->SetupAttachment(GetMesh(), "spine_02");
	spine_02->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("spine_02"), spine_02);
	
	spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_03"));
	spine_03->SetupAttachment(GetMesh(), "spine_03");
	spine_03->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("spine_03"), spine_03);
	
	upperarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_l"));
	upperarm_l->SetupAttachment(GetMesh(), "upperarm_l");
	upperarm_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("upperarm_l"), upperarm_l);
	
	upperarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_r"));
	upperarm_r->SetupAttachment(GetMesh(), "upperarm_r");
	upperarm_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("upperarm_r"), upperarm_r);
	
	lowerarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_l"));
	lowerarm_l->SetupAttachment(GetMesh(), "lowerarm_l");
	lowerarm_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("lowerarm_l"), lowerarm_l);
	
	lowerarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_r"));
	lowerarm_r->SetupAttachment(GetMesh(), "lowerarm_r");
	lowerarm_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("lowerarm_r"), lowerarm_r);
	
	hand_l = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_l"));
	hand_l->SetupAttachment(GetMesh(), "hand_l");
	hand_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("hand_l"), hand_l);
	
	hand_r = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_r"));
	hand_r->SetupAttachment(GetMesh(), "hand_r");
	hand_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("hand_r"), hand_r);
	
	backpack = CreateDefaultSubobject<UBoxComponent>(TEXT("backpack"));
	backpack->SetupAttachment(GetMesh(), "backpack");
	backpack->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("backpack"), backpack);
	
	blanket = CreateDefaultSubobject<UBoxComponent>(TEXT("blanket"));
	blanket->SetupAttachment(GetMesh(), "backpack");
	blanket->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("blanket"), blanket);
	
	thigh_l = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_l"));
	thigh_l->SetupAttachment(GetMesh(), "thigh_l");
	thigh_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("thigh_l"), thigh_l);
	
	thigh_r = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_r"));
	thigh_r->SetupAttachment(GetMesh(), "thigh_r");
	thigh_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("thigh_r"), thigh_r);
	
	calf_l = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_l"));
	calf_l->SetupAttachment(GetMesh(), "calf_l");
	calf_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("calf_l"), calf_l);
	
	calf_r = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_r"));
	calf_r->SetupAttachment(GetMesh(), "calf_r");
	calf_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("calf_r"), calf_r);
	
	foot_l = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_l"));
	foot_l->SetupAttachment(GetMesh(), "foot_l");
	foot_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("foot_l"), foot_l);
	
	foot_r = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_r"));
	foot_r->SetupAttachment(GetMesh(), "foot_r");
	foot_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("foot_r"), foot_r);
	
}

void ATP2ReseauCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ATP2ReseauCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ATP2ReseauCharacter, Health);
}

void ATP2ReseauCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
	}
	if (LagCompensation)
	{
		LagCompensation->Character = this;
		if (Controller)
		{
			LagCompensation->Controller = Cast<ATP2PlayerController>(Controller);
		}
	}
}


void ATP2ReseauCharacter::BeginPlay()
{
	Super::BeginPlay();
	UpdateHUDHealth();
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ATP2ReseauCharacter::ReceiveDamage);
	}
}

void ATP2ReseauCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	HideCameraIfCharacterClose();
}


void ATP2ReseauCharacter::PlayFireMontage(bool bIsAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName =  bIsAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
		
	}
}

void ATP2ReseauCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);
	}
}

void ATP2ReseauCharacter::Elim()
{
	MultiCastElim();
	GetWorldTimerManager().SetTimer(ElimTimer, this, &ATP2ReseauCharacter::ElimTimerFinished, ElimDelay);
}

void ATP2ReseauCharacter::MultiCastElim_Implementation()
{
	bElimmed = true;
	PlayElimMontage();
}

void ATP2ReseauCharacter::ElimTimerFinished()
{
	ATP2GameMode* ATP2GameMode = GetWorld()->GetAuthGameMode<class ATP2GameMode>();
	if (ATP2GameMode)
	{
		ATP2GameMode->RequestRespawn(this, Controller);
	}
}

void ATP2ReseauCharacter::PlayHitReactMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("FromFront");
		AnimInstance->Montage_JumpToSection(SectionName);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Hit React Montage"));
	}
}

void ATP2ReseauCharacter::ReceiveDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType,
	class AController* InstigatorController, AActor* DamageCauser)
{
	if (Health < 0.f) return;
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	UpdateHUDHealth();
	PlayHitReactMontage();

	
	
	if (Health == 0.f)
	{
		UWorld* World = GetWorld();
		if (!World) return;

		ATP2GameMode* ATP2GameMode = World->GetAuthGameMode<class ATP2GameMode>();
		if (!ATP2GameMode) return;

		if (Controller)
		{
			TP2PlayerController = TP2PlayerController == nullptr ? Cast<ATP2PlayerController>(Controller) : TP2PlayerController;
		}
		ATP2PlayerController* AttackerController = InstigatorController ? Cast<ATP2PlayerController>(InstigatorController) : nullptr;

		ATP2GameMode->PlayerEliminated(this, TP2PlayerController, AttackerController);
	}
	
}


void ATP2ReseauCharacter::UpdateHUDHealth()
{
	TP2PlayerController = TP2PlayerController == nullptr ? Cast<ATP2PlayerController>(GetController()) : TP2PlayerController;
	if (TP2PlayerController)
	{
		TP2PlayerController->SetHUDHealth(Health, MaxHealth);
	}
}


void ATP2ReseauCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled()) return;
	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

void ATP2ReseauCharacter::OnRep_Health()
{
	UpdateHUDHealth();
	PlayHitReactMontage();
}

void ATP2ReseauCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

bool ATP2ReseauCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool ATP2ReseauCharacter::IsAiming()
{
	return (Combat && Combat->bIsAiming);
}

void ATP2ReseauCharacter::OnRep_OverlappingWeapon(AWeapon* LastOverlappingWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastOverlappingWeapon)
	{
		LastOverlappingWeapon->ShowPickupWidget(false);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input


void ATP2ReseauCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATP2ReseauCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATP2ReseauCharacter::Look);

		//Interacting
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ATP2ReseauCharacter::Interact);

		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ATP2ReseauCharacter::Aim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ATP2ReseauCharacter::AimStop);

		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ATP2ReseauCharacter::StartFiring);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ATP2ReseauCharacter::StopFiring);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ATP2ReseauCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ATP2ReseauCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ATP2ReseauCharacter::Interact(const FInputActionValue& Value)
{
	if (Combat)
	{
		if (HasAuthority())
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			ServerInteract();
		}
	}
}


void ATP2ReseauCharacter::ServerInteract_Implementation()
{
	if (Combat)
	{
		if (OverlappingWeapon)
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
	}
}


void ATP2ReseauCharacter::Aim(const FInputActionValue& Value)
{
	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void ATP2ReseauCharacter::AimStop(const FInputActionValue& Value)
{
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

void ATP2ReseauCharacter::StartFiring(const FInputActionValue& Value)
{
	if (Combat)
	{
		Combat->FireButtonPressed(true);
	}
}

void ATP2ReseauCharacter::StopFiring(const FInputActionValue& Value)
{
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
}


