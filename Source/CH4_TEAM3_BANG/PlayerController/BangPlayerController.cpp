#include "BangPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Data/CardEnums.h"
#include "GameMode/BangGameMode.h"
#include "PlayerState/BangPlayerState.h"
#include "BangCharacter/BangCharacter.h"
#include "CharacterUIActor/BangUIActor.h"
#include "Camera/CameraComponent.h" 
#include "Camera/CameraActor.h"
#include "Materials/MaterialInterface.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameState/BangGameState.h"
#include "UI/BangInGameChattingWidget.h"
#include "UI/BangPlayerHUD.h"
#include "Components/WidgetComponent.h"
#include "EngineUtils.h"
#include "Components/CapsuleComponent.h"


ABangPlayerController::ABangPlayerController()
{}

void ABangPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				LocalPlayerSubsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
	if (IsLocalController())
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABangPlayerController::GetPlayerStateAtBegin);
	}
	/*FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	bShowMouseCursor = true;*/
}

void ABangPlayerController::Server_UseCardReturn_Implementation(bool IsAble)
{
	
}

void ABangPlayerController::Server_EndTurn_Implementation(const uint32 UniqueID, ECharacterType PlayerCharacter)
{
	
}

void ABangPlayerController::Client_SetControllerRotation_Implementation(FRotator NewRotation)
{
	if (IsLocalController())
	{
		SetControlRotation(NewRotation);
	}	
}

void ABangPlayerController::UpdatePlayerUI(FName& NewText)
{
	if (HasAuthority())
	{
		ABangCharacter* BangCharacter = Cast<ABangCharacter>(GetPawn());
		if (BangCharacter && BangCharacter->TextActor.IsValid())
		{
			BangCharacter->TextActor->SetDisplayText(NewText);
		}
	}
}

void ABangPlayerController::UpdatePlayerHP(int32 NewHP)
{
	if (HasAuthority())
	{
		ABangCharacter* BangCharacters = Cast<ABangCharacter>(GetPawn());
		if (BangCharacters)
		{
			BangCharacters->UpdateHPActors(NewHP);
		}
	}

}

void ABangPlayerController::SetInitializeHP(int32 NewHP)
{
	if (HasAuthority())
	{
		ABangCharacter* BangCharacterHP = Cast<ABangCharacter>(GetPawn());
		if (BangCharacterHP)
		{
			BangCharacterHP->SetHP(NewHP);
		}
	}
}

void ABangPlayerController::Client_SelectCard_Implementation()
{
    // UI 창 띄우기 (보유 중인 카드 표시)
    // 예시: UWidget* CardUI = CreateWidget<UWidget>(this, CardUIClass);
    // CardUI->AddToViewport();

    // 사용자가 카드 선택 (입력 대기)
    EActiveType SelectedActiveCard = EActiveType::None;
    EPassiveType SelectedPassiveCard = EPassiveType::None;

    // 카드 선택 후 처리 (별도 함수 호출)
    Client_HandleCardSelection(SelectedActiveCard);
}

void ABangPlayerController::Client_HandleCardSelection_Implementation(EActiveType SelectedCard)
{
    uint32 TargetPlayerID = 0; // 기본값, 상대가 필요하면 SelectTarget()에서 설정

    bool NeedsTarget = (SelectedCard == EActiveType::Bang ||
        SelectedCard == EActiveType::Robbery ||
        SelectedCard == EActiveType::CatBalou ||
        SelectedCard == EActiveType::Duel ||
        SelectedCard == EActiveType::Jail);

    if (NeedsTarget)
    {
        Client_SelectTarget();
        // 공격할 대상 선택 (레이 트레이싱 등)
        TargetPlayerID = 57;//SelectTarget();
        if (TargetPlayerID == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("대상이 필요한 카드인데 선택되지 않음!"));
            return;
        }
    }
    Server_UseCard(SelectedCard, TargetPlayerID);
}

///////////////////////////
//// 원명 추가 
//////////////////////////


void ABangPlayerController::MouseClicked()
{
	FHitResult HitResult;
	if (GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, HitResult))
	{
		DrawDebugSphere(GetWorld(), HitResult.Location, 10.f, 8, FColor::Red, false, 1.5f);
		ACharacter* HitChar = Cast<ACharacter>(HitResult.GetActor());
		
		if(HitChar && HitChar != GetPawn())
		{

			if (ABangCharacter* OtherPlayer = Cast<ABangCharacter>(HitChar))
			{
				CurrentMouseCursor = EMouseCursor::Hand;
				if (bIsCameraMode)
				{
					uint32 GetUID = GetUniqueID();
					//SendToServer And Send CloseCamera Request
					//TODO : SendToServerMethod()
					Server_CloseCamera();

				}

				else
				{

					// === 위젯 생성 및 표시 ===
					if (InteractionWidgetClass) // InteractionWidgetClass가 유효한지 확인
					{
						if (PlayerWidgets.Contains(OtherPlayer->GetPlayerState()->GetPlayerId()))
						{
							InteractionWidgetComponent = *PlayerWidgets.Find(OtherPlayer->GetPlayerState()->GetPlayerId());
						}
						else
						{
							InteractionWidgetComponent = NewObject<UWidgetComponent>(OtherPlayer);
							InteractionWidgetComponent->SetupAttachment(OtherPlayer->GetRootComponent());
							InteractionWidgetComponent->RegisterComponent();
							InteractionWidgetComponent->SetWidgetClass(InteractionWidgetClass);
							InteractionWidgetComponent->InitWidget();

							InteractionWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
							InteractionWidgetComponent->SetDrawSize(FVector2D(400, 200)); // 예시 크기
							InteractionWidgetComponent->SetRelativeLocation(
								FVector(0.f, 0.f, OtherPlayer->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 50.f)
							);
							PlayerWidgets.Add(OtherPlayer->GetPlayerState()->GetPlayerId(), InteractionWidgetComponent); // 맵에 저장
						//
						}

						// 2. 위젯 표시
						if (InteractionWidgetComponent)
						{
							InteractionWidgetComponent->SetVisibility(true);
							InteractionWidgetComponent->SetHiddenInGame(false);
						}
					}
					/**Test*/
					uint32 PlayerStateID = 0;
					uint32 TestTemp = 0;
					GEngine->AddOnScreenDebugMessage(
						-1,
						5.f,
						FColor::Red,
						TEXT("This is a debug message!")
					);
					if (OtherPlayer->GetPlayerState())
					{
						PlayerStateID = OtherPlayer->GetPlayerState()->GetPlayerId();
						printf("");
						ABangPlayerState* PlayerBangState = Cast<ABangPlayerState>(OtherPlayer->GetPlayerState());
						if (PlayerBangState)
						{
							//Get Information for UI
							//And Open UI
						}
					}
				}
			}
		}

	}
	else
	{
		//CloseHuD 
	}
	CurrentMouseCursor = EMouseCursor::Default;

	///////Test
	/*AActor* HitActor = HitResult.GetActor();
	if (HitActor)
	{
		ABangCharacter* HitPawn = Cast<ABangCharacter>(HitActor);
		if (HitPawn)
		{
			ABangPlayerController* HitController = Cast<ABangPlayerController>(HitPawn->GetController());
			if (HitController)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Controller IS valid ,"));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Controller IS NULL"));
			}
		}
	}*/

	//////TestEnd
}

void ABangPlayerController::Client_OpenCamera_Implementation()
{
	if (!IsLocalController())
	{
		return;
	}

	
	
	if (ABangCharacter* BangPlayer = Cast<ABangCharacter>(GetPawn()))
	{
		if (BangPlayer->GetFirstPersonMode())
		{
			BangPlayer->GetMesh()->SetVisibility(true);
		}

		UCameraComponent* StartCam = BangPlayer->FollowCamera;
		UCameraComponent* EndCam = BangPlayer->BangCamera;
		if (!StartCam || !EndCam) return;

	
		const FTransform StartTransform = StartCam->GetComponentTransform();
		ACameraActor* TempCam = GetWorld()->SpawnActor<ACameraActor>(
			ACameraActor::StaticClass(), StartTransform);
		if (!TempCam) return;

	
		BangPlayer->FollowCamera->Deactivate();
		BangPlayer->BangCamera->Deactivate();

		constexpr float BlendTime = 5.f;
		CameraOpenBlendStartTime = FPlatformTime::Seconds();
		SetViewTarget(TempCam);
		
		const FVector StartLocation = StartTransform.GetLocation();
		const FVector EndLocation = EndCam->GetComponentLocation()+300.f; // 마지막에 회전하는 효과를 주고 싶어서 벡터를 사용해서 300f만큼 이동 그러면 마지막에 꿀벌마냥 회전할것
		
		//BangCamera의 위치를 한번 봐야할듯
		const FVector FlagLocation = BangPlayer->GetFlagLocation();

		GetWorldTimerManager().SetTimer(CameraOpenBlendTimerHandle, FTimerDelegate::CreateLambda(
			[this, BangPlayer, TempCam, StartLocation, EndLocation, FlagLocation]() mutable
			{
				//좋아..상대시간 굳 
				float Elapsed = FPlatformTime::Seconds() - CameraOpenBlendStartTime;
				float Alpha = FMath::Clamp(Elapsed / BlendTime, 0.f, 1.f);

				FVector NewLoc = FMath::Lerp(StartLocation, EndLocation, Alpha);
				TempCam->SetActorLocation(NewLoc);
				TempCam->SetActorRotation((FlagLocation - NewLoc).Rotation());
				if (Alpha >= 1.f)
				{
					GEngine->AddOnScreenDebugMessage(
						-1,
						5.f,
						FColor::Red,
						TEXT("Alpha")
					);
					BangPlayer->BangCamera->Activate();
					SetViewTarget(BangPlayer);
					bIsCameraMode = true;
					GetWorldTimerManager().ClearTimer(CameraOpenBlendTimerHandle);
					if (GetWorldTimerManager().IsTimerActive(CameraOpenBlendTimerHandle))
					{
						GEngine->AddOnScreenDebugMessage(
							-1,
							5.f,
							FColor::Red,
							TEXT("Error OpenTimer Acive")
						);
						TempCam->Destroy();
					}
					TempCam->Destroy();
				}
			}), 0.01f, true);
		GetWorldTimerManager().SetTimer(BangModeTimerHandle, this, &ABangPlayerController::Server_CloseCamera, 10.f, false);
	}
	
}
void ABangPlayerController::Client_SetInputEnabled_Implementation(bool IsAttacker)
{
	if (!IsLocalController())
	{
		return;
	}
		
	if (IsAttacker)
	{
		if (auto LocalPlayer = GetLocalPlayer())
		{
			if (auto Sub = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Sub->RemoveMappingContext(InputMappingContext);
				Sub->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
	else
	{
		if (ULocalPlayer* LP = GetLocalPlayer())
		{
			if (auto* Subsys = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsys->RemoveMappingContext(InputMappingContext);
			}
		}
	}
}

void ABangPlayerController::Server_OpenCamera_Implementation()
{
	if (!HasAuthority())
	{
		return;
	}
	uint32 BangUID= GetUniqueID();
	ABangGameMode* GM = GetWorld()->GetAuthGameMode<ABangGameMode>();
	if (GM)
	{
		GM->OpenCamera(BangUID);
	}
}

void ABangPlayerController::Server_CloseCamera_Implementation()
{
	//왜 HasAuthority를 사용했는가? ->서버의 컨트롤러에서만 하게하려고 
	//아니라면 이야기해주세요 -원명
	if (!HasAuthority())
	{
		return;
	}
	if (GetWorldTimerManager().IsTimerActive(BangModeTimerHandle))
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.f,
			FColor::Red,
			TEXT("Bang Complete ,  Timer Clear")
		);
		GetWorldTimerManager().ClearTimer(BangModeTimerHandle);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.f,
			FColor::Red,
			TEXT("Duration End")
		);

	}
	ABangGameMode* GM = GetWorld()->GetAuthGameMode<ABangGameMode>();
	if (GM)
	{
		GM->CloseCamera();
	}

}
void ABangPlayerController::Client_CloseCamera_Implementation()
{
	if (!IsLocalController())
	{
		return;
	}
	bIsCameraMode = false;

	if (ABangCharacter* BangPlayer = Cast<ABangCharacter>(GetPawn()))
	{
		
		UCameraComponent* StartCam = BangPlayer->BangCamera; 
		UCameraComponent* EndCam = BangPlayer->FollowCamera;
		if (!StartCam || !EndCam) return;


		const FTransform StartTransform = StartCam->GetComponentTransform();
		ACameraActor* TempCam = GetWorld()->SpawnActor<ACameraActor>(
			ACameraActor::StaticClass(), StartTransform);
		if (!TempCam) return;


		BangPlayer->FollowCamera->Deactivate();
		BangPlayer->BangCamera->Deactivate();

		constexpr float BlendTime = 5.f;
		CameraOpenBlendStartTime = FPlatformTime::Seconds();
		SetViewTarget(TempCam);

		const FVector StartLocation = StartTransform.GetLocation()-100.f;
		const FVector EndLocation = EndCam->GetComponentLocation();
		
		const FVector FlagLocation = BangPlayer->GetFlagLocation();

		
		GetWorldTimerManager().SetTimer(CameraCloseBlendTimerHandle, FTimerDelegate::CreateLambda(
			[this, BangPlayer, TempCam, StartLocation, EndLocation, FlagLocation]() mutable
			{
				float Elapsed = FPlatformTime::Seconds() - CameraOpenBlendStartTime;
				float Alpha = FMath::Clamp(Elapsed / BlendTime, 0.f, 1.f);

				FVector NewLoc = FMath::Lerp(StartLocation, EndLocation, Alpha);
				TempCam->SetActorLocation(NewLoc);
				TempCam->SetActorRotation((FlagLocation - NewLoc).Rotation());

				if (Alpha >= 1.f)
				{
					//FTransform TempTransform = BangPlayer->GetInitialTransform();
					//사용자 움직이면 그냥 ㅈ대는 로직임 수정하긴해야하는데 
					//TempTransform.SetLocation(EndLocation);
					//CachedBangCameraTransform.SetRotation(TempTransform.GetRotation());
					//BangPlayer->FollowCamera->SetWorldTransform(CachedBangCameraTransform);
					/*BangPlayer->FollowCamera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
					BangPlayer->FollowCamera->AttachToComponent(BangPlayer->CameraBoom,
						FAttachmentTransformRules::SnapToTargetNotIncludingScale, USpringArmComponent::SocketName);*/
					//
					/*if (BangPlayer->CameraBoom)
					{
						BangPlayer->CameraBoom->SetRelativeTransform(BangPlayer->GetInitialBoomTransform());
					}
					BangPlayer->FollowCamera->SetRelativeTransform(BangPlayer->GetInitialCameraTransform());*/
					if (BangPlayer->GetFirstPersonMode())
					{
						BangPlayer->GetMesh()->SetVisibility(false);
					}
					BangPlayer->FollowCamera->Activate();
					SetViewTarget(BangPlayer);

					GetWorldTimerManager().ClearTimer(CameraCloseBlendTimerHandle);
					if (GetWorldTimerManager().IsTimerActive(CameraCloseBlendTimerHandle))
					{
						GEngine->AddOnScreenDebugMessage(
							-1,
							5.f,
							FColor::Red,
							TEXT("Error CloseTimer Acive")
						);
					}
					TempCam->Destroy();
				}
			}), 0.01f, true);
	}
}

void ABangPlayerController::Client_SelectTarget_Implementation()
{
    uint32 TargetPlayerID = 15;//GetSelectedTargetID(); // 상대 플레이어 ID를 가져옴 (레이 트레이싱 담당자에게 받아올 부분)

    if (TargetPlayerID > 0)
    {

    }
}

void ABangPlayerController::Server_UseCard_Implementation(EActiveType SelectedCard, uint32 TargetPlayerID)
{
    ABangPlayerState* BangPlayerState = GetPlayerState<ABangPlayerState>();
    if (BangPlayerState)
    {
        //BangPlayerState->ProcessCardUsage(SelectedCard, TargetPlayerID);
    }
}
UCameraComponent* ABangPlayerController::FindCameraByTag(APawn* Player12, const FName& Tag)
{
	TArray<UCameraComponent*> BangCameras;
	Player12->GetComponents<UCameraComponent>(BangCameras);
	for (UCameraComponent* Cam : BangCameras)
	{
		if (Cam && Cam->ComponentHasTag(Tag))
		{
			return Cam;
		}
	}
	return nullptr;
}
void ABangPlayerController::Client_SetOutline_Implementation(bool bEnable, int32 StencilValue)
{
	if (!IsLocalController())
		return;


	APawn* MyPawn = GetPawn();
	if (!MyPawn) return;

	TArray<UMeshComponent*> Meshes;
	MyPawn->GetComponents<UMeshComponent>(Meshes);

	for (UMeshComponent* Mesh : Meshes)
	{
		Mesh->SetRenderCustomDepth(bEnable);
		Mesh->SetCustomDepthStencilValue(bEnable ? StencilValue : 0);
	}
}
///////////////////////////
//// 찬호 추가 
//////////////////////////
void ABangPlayerController::Client_DisplayBangUI_Implementation()
{
	if (const TObjectPtr<ABangPlayerHUD> BangHUD = Cast<ABangPlayerHUD>(GetHUD()))
	{
		BangHUD->ChattingWidgetInstance->AddMessage(
			FText::FromString(FString::Printf(TEXT("Hello from %d"), GetUniqueID())),
			FSlateColor(FLinearColor::Green)
		);
	}	
}

void ABangPlayerController::NotifyHUDLoaded()
{
	Server_HUDLoaded();
	if (!HasAuthority())
	{
		if (const TObjectPtr<ABangPlayerHUD> BangHUD = Cast<ABangPlayerHUD>(GetHUD()))
		{
			BangHUD->ChattingWidgetInstance->StartButton->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ABangPlayerController::Server_HUDLoaded_Implementation()
{
	const TObjectPtr<ABangGameMode> GameMode = GetWorld()->GetAuthGameMode<ABangGameMode>();
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerController] BeginPlay Controller GameMode is NULL!"));
		return;
	}

	GameMode->UpdatePlayerHUD();
}

void ABangPlayerController::SendMessageToServer(FString Message)
{
	if (Message.IsEmpty()) return;

	FString ToPlayerNickname = "";

	// 귓속말의 경우 /{플레이어 아이디} {채팅내용}
	if (Message[0] == '/')
	{
		// 귓속말
		FString RawContent = Message.RightChop(1);

		FString TargetIDString;
		FString ChatContent;

		if (RawContent.Split(TEXT(" "), &TargetIDString, &ChatContent))
		{
			ToPlayerNickname = TargetIDString;
		}
	}

	// 전체챗팅
	Server_SendMessage(Message, PlayerNickname, ToPlayerNickname);
}

void ABangPlayerController::Client_ReceiveMessage_Implementation(const FString& Message, const FString& FromNickname, const FString& ToPlayerNickname)
{
	if (Message.IsEmpty()) return;

	if (FromNickname.IsEmpty())
	{
		// 전역
		if (const TObjectPtr<ABangPlayerHUD> BangHUD = Cast<ABangPlayerHUD>(GetHUD()))
		{
			BangHUD->ChattingWidgetInstance->AddMessage(
				FText::FromString(FString::Printf(TEXT("%s: %s"),*FromNickname, *Message)),
				FSlateColor(FLinearColor::White)
			);
		}	
	}
	else
	{
		// 특정
		if (PlayerNickname == FromNickname)
		{
			if (const TObjectPtr<ABangPlayerHUD> BangHUD = Cast<ABangPlayerHUD>(GetHUD()))
			{
				BangHUD->ChattingWidgetInstance->AddMessage(
					FText::FromString(FString::Printf(TEXT("%s: %s"),*FromNickname, *Message)),
					FSlateColor(FLinearColor::Red)
				);
			}
		}
	}
}

void ABangPlayerController::Server_SendMessage_Implementation(const FString& Message, const FString& FromNickname, const FString& ToPlayerNickname)
{
	if (ABangGameState* BangGameState = GetWorld()->GetGameState<ABangGameState>())
	{
		BangGameState->BroadcastChatMessage(Message, FromNickname, ToPlayerNickname);
	}
}

void ABangPlayerController::Server_StartGame_Implementation()
{
	const TObjectPtr<ABangGameMode> GameMode = GetWorld()->GetAuthGameMode<ABangGameMode>();
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerController] BeginPlay Controller GameMode is NULL!"));
		return;
	}

	GameMode->ForceUpdate_StartGame_Real();
}

void ABangPlayerController::StartButtonCLicked()
{
	Server_StartGame();
}

void ABangPlayerController::Server_StartTest_Implementation()
{
	const TObjectPtr<ABangGameMode> GameMode = GetWorld()->GetAuthGameMode<ABangGameMode>();
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerController] BeginPlay Controller GameMode is NULL!"));
		return;
	}

	GameMode->StartTest();
}

void ABangPlayerController::TestButtonCLicked()
{
	Server_StartTest();

}
void ABangPlayerController::Client_ToggleMappingContext_Implementation()
{
	if (!IsLocalController())
	{
		return;
	}

	if (ULocalPlayer* LocalBangPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsys = LocalBangPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (bIsCameraContextActive)
			{
				Subsys->RemoveMappingContext(CameraMappingContext);
				Subsys->AddMappingContext(InputMappingContext, 0);
			}
			else
			{
				Subsys->RemoveMappingContext(InputMappingContext);
				Subsys->AddMappingContext(CameraMappingContext, 0);
			}
			bIsCameraContextActive = !bIsCameraContextActive;
		}
	}
}

//void ABangPlayerController::SetWidgetVisibility(uint32 PlayerID, bool bVisible)
void ABangPlayerController::SetWidgetVisibility(uint32 PlayerID, bool bVisible)
{
	//if (!IsLocalController()) return;

	//InteractionWidgetComponent->SetHiddenInGame(!bVisible);
	//InteractionWidgetComponent->SetVisibility(bVisible);
	/*if (UWidgetComponent** CompPtr = PlayerWidgets.Find(PlayerID))
	{
		(*CompPtr)->SetVisibility(bVisible);
	}*/

	if (!IsLocalController() || ControllerPlayerStateID == PlayerID)
		return;
	
	if (UWidgetComponent** CompPtr = PlayerWidgets.Find(PlayerID))
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.f,
			FColor::Red,
			TEXT(" 위젯 찾았습니다  ")
		);
		UWidgetComponent* Comp = *CompPtr;
		Comp->SetVisibility(bVisible);
		Comp->SetHiddenInGame(!bVisible);
		//UUserWidget* BangUserWidget = Cast<UUserWidget>((*CompPtr)->GetUserWidgetObject());
		//BangUserWidget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		//BangUserWidget->SetHiddenInGame(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}
void ABangPlayerController::GetUserInformationUI(uint32 BangPlayerStateID)
{
	
}

void ABangPlayerController::GetPlayerStateAtBegin()
{
	UE_LOG(LogTemp, Error, TEXT("GetBegin시작"));
	if (!IsLocalController())
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			10.f,
			FColor::Red,
			TEXT("Local에서 걸림 ")
		);
		UE_LOG(LogTemp, Error, TEXT("로컬에서 걸림요 "));
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("스테이트 시작"));
	if (ABangPlayerState* MyPS = GetPlayerState<ABangPlayerState>())
	{
		ControllerPlayerStateID = MyPS->GetPlayerId();
		FString Msg = FString::Printf(TEXT("Local Controller PlayerStateID = %d"), ControllerPlayerStateID);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Msg); 
		UE_LOG(LogTemp, Error, TEXT("스테이트 있습니다"));
	}
	UE_LOG(LogTemp, Error, TEXT("플레이어 스테이트 액터 이터레이터 시작  "));

	for (TActorIterator<ABangCharacter> It(GetWorld()); It; ++It)
	{
		UE_LOG(LogTemp, Error, TEXT("플레이어 스테이트 액터 이터레이터 시작 내부 "));
		ABangCharacter* BangPlayer = *It;
		if (!BangPlayer)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				10.f,
				FColor::Red,
				TEXT("No Player")
			);
			UE_LOG(LogTemp, Error, TEXT("플레이어 없습니다 "));
		}
		if (APlayerState* PS = BangPlayer->GetPlayerState())
		{
			uint32 ID = PS->GetPlayerId();

			//UWidgetComponent* WidgetComp = NewObject<UWidgetComponent>(BangPlayer, UWidgetComponent::StaticClass(), TEXT("InteractionWidget"));
			UWidgetComponent* WidgetComp = NewObject<UWidgetComponent>(BangPlayer);
			WidgetComp->SetupAttachment(BangPlayer->GetRootComponent());
			WidgetComp->RegisterComponent();

			WidgetComp->SetWidgetClass(InteractionWidgetClass);
			WidgetComp->InitWidget();

			WidgetComp->SetWidgetSpace(EWidgetSpace::World);
			WidgetComp->SetDrawSize(FVector2D(400, 200));
			WidgetComp->SetRelativeLocation(
				FVector(0.f, 0.f, BangPlayer->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 50.f)
			);

			WidgetComp->SetVisibility(false);
			WidgetComp->SetHiddenInGame(true);
			WidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			WidgetComp->SetGenerateOverlapEvents(false);
			WidgetComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
			PlayerWidgets.Add(ID, WidgetComp);
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("위젯 저장 완료"));
			UE_LOG(LogTemp, Error, TEXT("위젯 저장 완료"));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				10.f,
				FColor::Red,
				TEXT("No Player State")
			);
		}
	}
	UE_LOG(LogTemp, Error, TEXT("GetPlayerStateAtBegin 함수 종료  "));
}

void ABangPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	//GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABangPlayerController::GetPlayerStateAtBegin);
}