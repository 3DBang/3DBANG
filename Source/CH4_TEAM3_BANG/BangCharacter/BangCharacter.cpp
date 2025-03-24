// Fill out your copyright notice in the Description page of Project Settings.


#include "BangCharacter/BangCharacter.h"
#include "PlayerController/BangPlayerController.h"
#include "CharacterUIActor/BangUIActor.h"
#include "CharacterUIActor/BangHPActor.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"  

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
	FollowCamera->ComponentTags.Add(FName("Main"));


	BangCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("BangCamera"));
	BangCamera->ComponentTags.Add(FName("AtBang"));

	CameraBoom->bUsePawnControlRotation = true;
	bUseControllerRotationPitch = false;
	//값수정한것 
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	if (UPrimitiveComponent* PrimComponent = Cast<UPrimitiveComponent>(GetRootComponent()))
	{
		PrimComponent->SetGenerateOverlapEvents(true);
		PrimComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		PrimComponent->OnBeginCursorOver.AddDynamic(this, &ABangCharacter::OnCursorBegin);
		PrimComponent->OnEndCursorOver.AddDynamic(this, &ABangCharacter::OnCursorEnd);
	}

}

// Called when the game starts or when spawned
void ABangCharacter::BeginPlay()
{
	Super::BeginPlay();

	/**Set TextActor*/
	//HasAuthority 설정 필수 !서버에만 스폰이 되게
	if (HasAuthority() && TextActorUIClass)
	{
		float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		
		FVector RelativeLocation(0.f, 0.f, CapsuleHalfHeight + 70.f);
		FRotator RelativeRotation = FRotator::ZeroRotator;
		FTransform RelativeTransform(RelativeRotation, RelativeLocation);

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		//지연스폰 ! FinishSpawingActor를 호출하면 그 때 스폰됨
		ABangUIActor* DeferredTextActor = GetWorld()->SpawnActorDeferred<ABangUIActor>(TextActorUIClass, RelativeTransform, this);
		if (DeferredTextActor)
		{
			// CapsuleComponent에 부착하면서 KeepRelativeTransform 규칙을 사용
			DeferredTextActor->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);

			// FinishSpawningActor를 호출하면, 설정한 RelativeTransform을 기준으로 스폰
			UGameplayStatics::FinishSpawningActor(DeferredTextActor, RelativeTransform);
			TextActor = DeferredTextActor;
		}
	}
	if (APlayerStart* TempPlayerStart = GetFlaggedActor())
	{
		FVector TargetLocation = TempPlayerStart->GetActorLocation();
		FlagLocation = TargetLocation;
		TargetLocation.Z += 1000.f;
		BangCamera->SetWorldLocation(TargetLocation);

		FVector DownVector = -TempPlayerStart->GetActorUpVector();
		FRotator CameraRotation = DownVector.Rotation();
		BangCamera->SetWorldRotation(CameraRotation);

	}
	if (CameraBoom)
	{
		InitialBoomTransform = CameraBoom->GetRelativeTransform();
	}
	if (FollowCamera)
	{
		InitialCameraTransform = FollowCamera->GetRelativeTransform();
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
	float ZoomDelta = Value.Get<float>();

	if (CameraBoom)
	{
		const float ZoomSpeed = 20.0f;
		const float MinArmLength = 50.0f;  
		const float MaxArmLength = 400.0f; 
		const float FirstPersonThreshold = 150.0f; 

		float NewArmLength = FMath::Clamp(CameraBoom->TargetArmLength - ZoomDelta * ZoomSpeed, MinArmLength, MaxArmLength);
		CameraBoom->TargetArmLength = NewArmLength;

		bFirstPersonMode = (NewArmLength <= FirstPersonThreshold);

		if (bFirstPersonMode)
		{
			GetMesh()->SetVisibility(false);
			
			FollowCamera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

			float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			float CapsuleRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();
			FVector HeadOffset = FVector(CapsuleRadius+10.f, 0.f, CapsuleHalfHeight+10.f);
			FVector HeadLocation = GetActorLocation() + GetActorRotation().RotateVector(HeadOffset);
			FRotator CameraRotation = GetActorRotation();

			FollowCamera->SetWorldLocation(HeadLocation);
			FollowCamera->SetWorldRotation(CameraRotation);

			FollowCamera->bUsePawnControlRotation = true;
			CameraBoom->bUsePawnControlRotation = false;
		}
		else
		{
			GetMesh()->SetVisibility(true);
			FollowCamera->AttachToComponent(CameraBoom, FAttachmentTransformRules::SnapToTargetIncludingScale);
			FollowCamera->bUsePawnControlRotation = false;
			CameraBoom->bUsePawnControlRotation = true;
			//소켓써보려다가 개털려서 이렇게 할게요 회전값 안돼서 ㄷㄷ
		}
	}
}

void ABangCharacter::Click(const FInputActionValue& Value)
{
	//OnMouseClicked.Broadcast();
	if (ABangPlayerController* PC = Cast<ABangPlayerController>(GetController()))
	{
		PC->MouseClicked();
	}
}
/**히든 처리*/
void ABangCharacter::UpdateHPActors(int32 NewHP)
{
	for (int32 i = 0; i < HPActors.Num(); i++)
	{
		if (HPActors[i].IsValid())
		{
			bool bVisible = (i < NewHP);
			HPActors[i]->SetHiddenActorState(!bVisible);
		}
	}
}

void ABangCharacter::SetHP(int32 NewHP)
{
	/**Set HPActor*/
	//HasAuthority 설정 필수 ! 서버에만 스폰이 되게 
	if (HasAuthority() && HPActorClass)
	{
		HP = NewHP;
		const float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		for (int32 i = 0; i < NewHP; i++)
		{
			
			FVector RelativeLocation(0.f, (i - (HP - 1) / 2.f) * 30.f, CapsuleHalfHeight + 10.f);
			FRotator RelativeRotation = FRotator::ZeroRotator;
			FTransform RelativeTransform(RelativeRotation, RelativeLocation);

			FVector Offset = FVector(0.f, i * 30.f, 100.f);
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;

			ABangHPActor* HPActor = GetWorld()->SpawnActorDeferred<ABangHPActor>(
				HPActorClass, RelativeTransform, this);
			if (HPActor)
			{
				HPActor->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);
				HPActor->SetHiddenActorState(false);
				UGameplayStatics::FinishSpawningActor(HPActor, RelativeTransform);
				HPActors.Add(HPActor);
			}
		}
	}
}

FVector ABangCharacter::GetFlagLocation() const
{
	return FlagLocation;
}

void ABangCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// 서버 권한에서만 HP / Text Actor 파괴 (안전 장치) 
	if (HasAuthority())
	{
		if (TextActor.IsValid())
		{
			TextActor->Destroy();
			TextActor = nullptr;
		}

		for (auto& WeakHP : HPActors)
		{
			if (WeakHP.IsValid())
			{
				WeakHP->Destroy();
			}
		}
		HPActors.Empty(); 
	}
}
APlayerStart* ABangCharacter::GetFlaggedActor()
{
	UWorld* World = GetWorld();

	if (!World)
	{
		return nullptr;
	}
	TArray<AActor*> AllPlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), AllPlayerStarts);

	APlayerStart* FlaggedStarts;
	for (AActor* Actor : AllPlayerStarts)
	{
		APlayerStart* Start = Cast<APlayerStart>(Actor);
		if (Start && Start->ActorHasTag(FName("Flagged")))
		{
			FlaggedStarts = Start;
			return FlaggedStarts;
		}
	}
	return nullptr;
}


const FTransform& ABangCharacter::GetInitialBoomTransform() const
{
	return InitialBoomTransform;
}

const FTransform& ABangCharacter::GetInitialCameraTransform() const
{
	return InitialCameraTransform;
}

bool ABangCharacter::GetFirstPersonMode()
{
	return bFirstPersonMode;
}

void ABangCharacter::OnCursorBegin(UPrimitiveComponent* MouseComp)
{
	if (ABangPlayerController* PC = Cast<ABangPlayerController>(GetController()))
	{
		int32 PlayerID = GetPlayerState()->GetPlayerId();
		FString Msg = FString::Printf(TEXT("Touch Start — PlayerState ID: %d"), PlayerID);
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.f,
			FColor::Red,
			Msg
		);
		PC->SetWidgetVisibility(GetPlayerState()->GetPlayerId(), true);
	}
}

void ABangCharacter::OnCursorEnd(UPrimitiveComponent* MouseComp)
{
	if (ABangPlayerController* PC = Cast<ABangPlayerController>(GetController()))
	{
		int32 PlayerID = GetPlayerState()->GetPlayerId();
		FString Msg = FString::Printf(TEXT("Touch End — PlayerState ID: %d"), PlayerID);
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.f,
			FColor::Red,
			Msg
		);
		PC->SetWidgetVisibility(GetPlayerState()->GetPlayerId(), false);
	}
}

