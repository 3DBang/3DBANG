// Copyright Epic Games, Inc. All Rights Reserved.

#include "CH4_TEAM3_BANGCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "PlayerController/BangPlayerController.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ACH4_TEAM3_BANGCharacter

ACH4_TEAM3_BANGCharacter::ACH4_TEAM3_BANGCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	bReplicates = true;
	SetReplicateMovement(true);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
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

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input
//
//void ACH4_TEAM3_BANGCharacter::NotifyControllerChanged()
//{
//	Super::NotifyControllerChanged();
//
//	// Add Input Mapping Context
//	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
//	{
//		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
//		{
//			Subsystem->AddMappingContext(DefaultMappingContext, 0);
//		}
//	}
//}

void ACH4_TEAM3_BANGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Moving Test
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACH4_TEAM3_BANGCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACH4_TEAM3_BANGCharacter::Look);

		// Menu
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACH4_TEAM3_BANGCharacter::Menu);
		
		// CardInformation
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACH4_TEAM3_BANGCharacter::Inforamation);

		// Camera Zoom
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACH4_TEAM3_BANGCharacter::Zoom);
		
		// UserClick
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACH4_TEAM3_BANGCharacter::Click);

	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ACH4_TEAM3_BANGCharacter::Move(const FInputActionValue& Value)
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
	UE_LOG(LogTemp, Display, TEXT("dtdtdtdtd"));
}

void ACH4_TEAM3_BANGCharacter::Look(const FInputActionValue& Value)
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

void ACH4_TEAM3_BANGCharacter::Menu(const FInputActionValue& Value)
{
	//OpenMenuHUD()
}

void ACH4_TEAM3_BANGCharacter::Inforamation(const FInputActionValue& Value)
{
	//OpenCardInforamtion
}

void ACH4_TEAM3_BANGCharacter::Zoom(const FInputActionValue& Value)
{
	//Camera
}

void ACH4_TEAM3_BANGCharacter::Click(const FInputActionValue& Value)
{
	//Click Other User ->GetUsersController -> PlayerState 
	//How Can I GET Users Controller?
}
