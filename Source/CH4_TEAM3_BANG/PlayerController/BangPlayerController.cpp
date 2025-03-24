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
#include "GameState/BangGameState.h"
#include "UI/BangInGameChattingWidget.h"
#include "UI/BangPlayerHUD.h"

ABangPlayerController::ABangPlayerController()
{}

void ABangPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	const FString MapName = GetWorld()->GetMapName();
	if (MapName == "StageMap")
	{
		if (TObjectPtr<AGameModeBase> GameMode = GetWorld()->GetAuthGameMode())
		{
			if (TObjectPtr<ABangGameMode> BangGameMode = Cast<ABangGameMode>(GameMode))
			{
				BangGameMode->AddPlayer(GetUniqueID());
			}
		}
	}

	// 순서 변경 필요
	if (IsLocalController())
	{
		/*if (const TObjectPtr<ABangPlayerHUD> BangHUD = Cast<ABangPlayerHUD>(GetHUD()))
		{
			BangHUD->ChattingWidgetInstance->AddMessage(FText::FromString("Hello from Controller!"), FSlateColor(FLinearColor::Green));

			TArray<UBangPlayerStatData*> PlayerStats;

			const auto Stat = NewObject<UBangPlayerStatData>();
			Stat->PlayerId = "PlayerOne";
			Stat->bIsAlive = true;
			PlayerStats.Add(Stat);
			
			BangHUD->PlayerListWidgetInstance->UpdatePlayerList(PlayerStats);
		}*/
	}
	// 순서 변경 필요
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
					Server_CloseCamera();
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
	else
	{
		//CloseHuD 
	}
	CurrentMouseCursor = EMouseCursor::Default;
}

void ABangPlayerController::Client_OpenCamera_Implementation()
{
	if (!IsLocalController())
	{
		return;
	}

	
	
	if (ABangCharacter* BangPlayer = Cast<ABangCharacter>(GetPawn()))
	{
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

void ABangPlayerController::SendChatMessage(const FString& Message)
{
	ABangPlayerState* MyPlayerState = GetPlayerState<ABangPlayerState>();
	ABangGameState* GameState = GetWorld()->GetGameState<ABangGameState>();

	if (!MyPlayerState)
	{
		UE_LOG(LogTemp, Error, TEXT("SendChatMessage 실패: PlayerState가 nullptr입니다!"));
	}
	if (!GameState)
	{
		UE_LOG(LogTemp, Error, TEXT("SendChatMessage 실패: GameState가 nullptr입니다!"));
	}

	if (MyPlayerState && GameState)
	{
		uint32 MyUniqueID = MyPlayerState->PlayerInfo.PlayerUniqueID;
		GameState->Server_SendChatMessage(MyUniqueID, Message);

		UE_LOG(LogTemp, Log, TEXT("[Client] 채팅 전송: UniqueID=%d, Message=%s"), MyUniqueID, *Message);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("채팅 전송 실패: PlayerState 또는 GameState 없음"));
	}
}

void ABangPlayerController::Server_RequestSendChatMessage_Implementation(const FString& Message)
{
	ABangGameState* GameState = GetWorld()->GetGameState<ABangGameState>();
	if (GameState)
	{
		ABangPlayerState* MyPlayerState = GetPlayerState<ABangPlayerState>();
		if (MyPlayerState)
		{
			uint32 MyUniqueID = MyPlayerState->PlayerInfo.PlayerUniqueID;
			GameState->Server_SendChatMessage(MyUniqueID, Message);

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

bool ABangPlayerController::Server_RequestSendChatMessage_Validate(const FString& Message)
{
	return !Message.IsEmpty();  // 기본적인 검증
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

// 플레이어에게 카드 선택권 요구 응답
void ABangPlayerController::Server_RespondSelectCard_Implementation()
{
	FPlayerCardCollection PlayerCardCollection;
	for (auto [Card] : SelectCardCollection.CardList)
	{
		FPlayerCardSymbol SingleCard;
		SingleCard.SymbolNumber = Card->SymbolNumber;
		SingleCard.SymbolType = Card->SymbolType;
		PlayerCardCollection.PlayerCards.Add(SingleCard);
	}
	
	SelectCardCollection.CardList.Empty();
	
	const TObjectPtr<ABangGameMode> GameMode = GetWorld()->GetAuthGameMode<ABangGameMode>();
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerController] BeginPlay Controller GameMode is NULL!"));
		return;
	}
	
	GameMode->RefundCards(PlayerCardCollection);
}

// 플레이어에게 카드 선택권 요구
void ABangPlayerController::Client_RequestSelectCard_Implementation(const uint32& PlayerUniqueID, const FPlayerCardCollection DrawCards)
{
	if (DrawCards.PlayerCards.Num() == 0) return;

	if (IsLocalController() && GetUniqueID() == PlayerUniqueID)
	{
		ABangPlayerState* BangPlayerState = GetPlayerState<ABangPlayerState>();
		BangPlayerState->GetCard(PlayerUniqueID, SelectCardCollection);
		
		// 플레이어에게 카드 선택권 요구
		
		
		// 선택한 카드 배열에서 지우기
		// SelectCardCollection
	}
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