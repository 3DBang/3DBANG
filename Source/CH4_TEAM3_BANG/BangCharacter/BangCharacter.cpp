// Fill out your copyright notice in the Description page of Project Settings.


#include "BangCharacter/BangCharacter.h"
#include "PlayerController/BangPlayerController.h"
#include "CharacterUIActor/BangUIActor.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"

// Sets default values
ABangCharacter::ABangCharacter()
{
	
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; 
	

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false;
	CameraBoom->bUsePawnControlRotation = true;
	bUseControllerRotationPitch = false;
	//값수정한것 
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

}

// Called when the game starts or when spawned
void ABangCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (TextActorUIClass)
	{
		//캡슐컴포넌트의 길이 절반 받아서+100. f 
		float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		//FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, CapsuleHalfHeight + 50.f);
		//FRotator SpawnRotation = FRotator::ZeroRotator;
		FVector RelativeLocation(0.f, 0.f, CapsuleHalfHeight + 50.f);
		FRotator RelativeRotation = FRotator::ZeroRotator;
		FTransform RelativeTransform(RelativeRotation, RelativeLocation);

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		//TextActor = GetWorld()->SpawnActor<ABangUIActor>(TextActorUIClass, SpawnLocation, SpawnRotation, SpawnParams);
		ABangUIActor* DeferredTextActor = GetWorld()->SpawnActorDeferred<ABangUIActor>(TextActorUIClass, RelativeTransform, this);
		if (DeferredTextActor)
		{
			// CapsuleComponent에 부착하면서 KeepRelativeTransform 규칙을 사용합니다.
			DeferredTextActor->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);

			// FinishSpawningActor를 호출하면, 설정한 RelativeTransform을 기준으로 스폰
			UGameplayStatics::FinishSpawningActor(DeferredTextActor, RelativeTransform);
			TextActor = DeferredTextActor;
		}


		/*if (TextActor)
		{
			TextActor->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		}*/
	}
}
void ABangCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void ABangCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ABangPlayerController* PlayerController = Cast<ABangPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ABangCharacter::Move
				);
			}
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ABangCharacter::Look
				);
			}
			if (PlayerController->MenuAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MenuAction,
					ETriggerEvent::Triggered,
					this,
					&ABangCharacter::Menu
				);
			}
			if (PlayerController->InformationAction)
			{
				EnhancedInput->BindAction(
					PlayerController->InformationAction,
					ETriggerEvent::Triggered,
					this,
					&ABangCharacter::Information
				);
			}
			if (PlayerController->ZoomAction)
			{
				EnhancedInput->BindAction(
					PlayerController->ZoomAction,
					ETriggerEvent::Triggered,
					this,
					&ABangCharacter::Zoom
				);
			}
			if (PlayerController->ClickAction)
			{
				EnhancedInput->BindAction(
					PlayerController->ClickAction,
					ETriggerEvent::Triggered,
					this,
					&ABangCharacter::Click
				);
			}

		}
	}

}

void ABangCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MovementVector.X))
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.X);
	}

	if (!FMath::IsNearlyZero(MovementVector.Y))
	{
		AddMovementInput(GetActorRightVector(), MovementVector.Y);
	}
}

void ABangCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ABangCharacter::Menu(const FInputActionValue& Value)
{
	//OpenMenuHUD()
}

void ABangCharacter::Information(const FInputActionValue& Value)
{
	//OpenCardInforamtion
}

void ABangCharacter::Zoom(const FInputActionValue& Value)
{
	//CameraAction
}

void ABangCharacter::Click(const FInputActionValue& Value)
{
	//How Can I GET Users Controller?
	//Server Player?
}

