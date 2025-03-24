#include "BangPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Data/CardEnums.h"
#include "GameMode/BangGameMode.h"
#include "PlayerState/BangPlayerState.h"
#include "BangCharacter/BangCharacter.h"
#include "Card/BangCardManager.h"
#include "Card/BaseCard/BangCardBase.h"

#include "CharacterUIActor/BangUIActor.h"
#include "Camera/CameraComponent.h" 
#include "Camera/CameraActor.h"
#include "Materials/MaterialInterface.h"
#include "Camera/PlayerCameraManager.h"
#include "GameState/BangGameState.h"
#include "UI/BangPlayerHUD.h"
#include "UI/Card/CardList.h"
#include "UI/Chat/BangInGameChattingWidget.h"

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
	
	/*FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	bShowMouseCursor = true;*/
}

void ABangPlayerController::Server_UseCardReturn_Implementation(bool IsAble)
{
	
}

void ABangPlayerController::Client_SetControllerRotation_Implementation(FRotator NewRotation)
{
	if (IsLocalController())
	{
		SetControlRotation(NewRotation);
	}	
}

void ABangPlayerController::Client_OnTurnStart_Implementation(const FCardCollection& DrawCards)
{
	bCanUseBang = true;
	UE_LOG(LogTemp, Warning, TEXT("[ABangPlayerController::Client_OnTurnStart_Implementation]: It's my turn! Controller Name: %s"), *GetName());
	ABangPlayerState* BangPlayerState= GetPlayerState<ABangPlayerState>();
	if (!PlayerState)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerController::Client_OnTurnStart_Implementation]: PlayerState is null!"));
		return;
	}

	if (ABangPlayerHUD* BangHUD = Cast<ABangPlayerHUD>(GetHUD())) // HUD 캐스팅 및 유효성 검사
	{
		if (UCardList* CardListWidget = BangHUD->CardListWidgetInstance) // CardListWidgetInstance 유효성 검사
		{
			for (const FSingleCard& Card : DrawCards.CardList)
			{
				CardListWidget->AddCard(Card); // 카드 위젯 추가
			}

			// 허드에 있는 카드 사용 버튼 활성화
			// 턴종료 버튼 만들어서 활성화
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[ABangPlayerState::StartTurn] CardListWidgetInstance 없음 HUD 있음"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerState::StartTurn] BangHUD 없음"));
	}
	
}

void ABangPlayerController::UpdateCardList()
{
	UE_LOG(LogTemp, Log, TEXT("[ABangPlayerController::Client_UpdateCardList_Implementation] UI 카드리스트 업데이트"));

	ABangPlayerState* BangPlayerState = GetPlayerState<ABangPlayerState>();
	if (!PlayerState)
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerController::Client_UpdateCardList_Implementation] PlayerState 없음"));
		return;
	}

	FCardCollection MyCardCollection;
	BangPlayerState->GetCard(GetUniqueID(), MyCardCollection); // PS에서 카드 정보 가져오기
	UE_LOG(LogTemp, Error, TEXT("[ABangPlayerController::Client_UpdateCardList_Implementation] Player ID: %d"), GetUniqueID());

	if (ABangPlayerHUD* BangHUD = Cast<ABangPlayerHUD>(GetHUD())) // HUD 캐스팅 및 유효성 검사
	{
		if (UCardList* CardListWidget = BangHUD->CardListWidgetInstance) // CardListWidgetInstance 유효성 검사
		{
			CardListWidget->ClearCards(); // 기존 카드 리스트 비우기

			for (const FSingleCard& Card : MyCardCollection.CardList)
			{
				CardListWidget->AddCard(Card); // 카드 위젯 추가
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[ABangPlayerController::Client_UpdateCardList_Implementation] CardListWidgetInstance 없음 HUD 있음"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[ABangPlayerController::Client_UpdateCardList_Implementation] BangHUD 없음"));
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
	// PlayerState에서 받아온 CurrentCardCollection을 기반으로
	// 유저가 실제로 들고 있는 카드 중에서 선택하도록 UI 연동

	const FCardCollection& MyCards = CurrentCardCollection;
	//이걸 기반으로 카드 UI에 표시하기
	
	// 유저가 클릭한 카드의 정보로
	//FSingleCard SelectedCard = /* 유저가 선택한 카드 */;
	//Client_HandleCardSelection(SelectedCard);
}


/**
 * 카드 선택에 대한 클라이언트 작업을 처리합니다. 선택된 카드와 필요에 따라 타겟 플레이어를 기반으로 적절한 서버 작업을 실행합니다.
 *
 * @param SingleCard 선택된 카드에 대한 정보를 포함하는 구조체입니다.
 */
void ABangPlayerController::Client_HandleCardSelection_Implementation(const FSingleCard& SingleCard)
{
    uint32 TargetPlayerID = 0; // 기본값, 상대가 필요하면 SelectTarget()에서 설정
	if (!SingleCard.Card)return;

	EActiveType OutActiveType;
	EPassiveType OutPassiveType;

	ABangPlayerState* PS = GetPlayerState<ABangPlayerState>();
	if (!PS) return;

	FPlayerInformation* Info = PS->PlayerInfo.GetPlayerInformation(GetUniqueID());
	if (!Info) return;

	PS->GetCardType(GetUniqueID(), SingleCard, OutActiveType, OutPassiveType);

	if (OutActiveType == EActiveType::Missed)
	{
		UE_LOG(LogTemp, Warning, TEXT("Missed card cannot be used on your own turn"));
		return;
	}
	if (OutActiveType == EActiveType::Bang)
	{
		if (!bCanUseBang)return;
		if (Info->CharacterCardType == ECharacterType::WillyTheKid)
		{
			Client_SelectTarget();
		}
		else
		{
			bCanUseBang = false;
			Client_SelectTarget();
		}
	}
	bool bNeedsTarget = (OutActiveType == EActiveType::Bang ||
		OutActiveType == EActiveType::Robbery ||
		OutActiveType == EActiveType::CatBalou ||
		OutActiveType == EActiveType::Duel ||
		OutActiveType == EActiveType::Jail);

	if (bNeedsTarget)
	{
		Client_SelectTarget(); // 나중에 실제 대상 선택 구현 예정

		if (TargetPlayerID == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Target required but not selected"));
			return;
		}
	}
	else 
	{
		Server_UseCard(SingleCard, TargetPlayerID);
	}
}

void ABangPlayerController::Server_EndTurn_Implementation()
{
	ABangPlayerState* PS = GetPlayerState<ABangPlayerState>();
	if (!PS)return;

	const uint32 UniqueID = GetUniqueID();
	FPlayerInformation* MyInfo = PS->PlayerInfo.GetPlayerInformation(UniqueID);
	if (!MyInfo)
	{
		UE_LOG(LogTemp, Error, TEXT("[Controller] PlayerInfo not found for ID: %d"), UniqueID);
		return;
	}

	const int32 CurrentHealth = MyInfo->CurrentHealth;
	const int32 CardCount = MyInfo->MyCards.PlayerCards.Num();

	if (CardCount > CurrentHealth)
	{
		// 클라이언트에 카드 버리기 UI 요청
		Client_RequestCardSelection(CurrentCardCollection, CardCount-CurrentHealth, ECardSelectPurpose::DiscardCard);
		return;
	}

	// 턴 종료 요청
	//PS->Server_EndTurn(UniqueID, MyInfo->CharacterCardType);
}


void ABangPlayerController::Client_RequestCardSelection_Implementation(
	const FCardCollection& CardsToChooseFrom,
	int32 RequiredSelectCount,
	ECardSelectPurpose Purpose)
{
	switch (Purpose)
	{
	case ECardSelectPurpose::UseCard:
		// 내 턴에서 카드 사용 (자유롭게 선택)
		break;

	case ECardSelectPurpose::DiscardCard:
		// 보유 카드 수 > 체력, 초과분 만큼 버려야 함
		break;

	case ECardSelectPurpose::GeneralStoreDraft:
		// 잡화점 – 전체 플레이어가 순서대로 카드 중 1장 선택
		break;

	case ECardSelectPurpose::KitCarlsonDrawCard:
		// 키트 칼슨 능력 – 카드 3장 중 2장 선택
		//MyInfo.SelectableCards 를 화면에 띄워야하고 뽑아야되는 카드 수는 2
		break;

	case ECardSelectPurpose::StealFromOpponent:
		// 상대의 보유 카드 중 1장을 선택 (정보가 안 보일 수 있음)
		//MyInfo.SelectableCards = Target.MyCard 를 화면에 뒷면으로 띄워야하고 뽑아야되는 카드 수는 1
		break;

	case ECardSelectPurpose::RespondToDuel:
		// 결투 중 뱅 카드 선택
		// 보유 카드 띄우기 1장 선택(뱅만)
		break;

	case ECardSelectPurpose::RespondToIndians:
		// 인디언 카드 대응 – 뱅 카드 선택
		// 보유 카드 중 1장 선택(뱅만)
		break;

	case ECardSelectPurpose::RespondToAttack:
		// Bang, Gatling 등의 공격에 대해 Missed 카드 선택
		// 보유카드 중 1장 선택(Missed)만
		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("Unknown Card Select Purpose"));
		break;
	}
}



void ABangPlayerController::OnCardSelectionComplete(
	const FCardCollection& CardsToChooseFrom,       // 원래 주어진 카드 목록
	const TArray<FSingleCard>& SelectedCards,       // 플레이어가 실제로 선택한 카드들
	int32 RequiredSelectCount,                      // 선택해야 할 개수
	ECardSelectPurpose Purpose)                     // 선택 목적
{

	ABangPlayerState* PS = GetPlayerState<ABangPlayerState>();
	//if (!PS || !PS->CardManager)return;

	FPlayerInformation* MyInfo = PS->PlayerInfo.GetPlayerInformation(GetUniqueID());
	if (!MyInfo)return;

	bool bAllowEmptySelection = (
		Purpose == ECardSelectPurpose::RespondToDuel ||
		Purpose == ECardSelectPurpose::RespondToIndians ||
		Purpose == ECardSelectPurpose::RespondToAttack
		);

	if (!bAllowEmptySelection && SelectedCards.Num() != RequiredSelectCount)return;


	switch (Purpose)
	{
	case ECardSelectPurpose::UseCard:
	{
		//카드 사용하기
		Client_HandleCardSelection(SelectedCards[0]);
		break;
	}
	case ECardSelectPurpose::DiscardCard:
	{
		for (const FSingleCard& Card : SelectedCards)
		{
			//보유 카드에서 제거 후 버린카드덱에 추가
			MyInfo->MyCards.RemoveCard(Card.Card->SymbolType, Card.Card->SymbolNumber);
			PS->RestoreCard(GetUniqueID(), Card);
		}

		// 턴 종료 호출
		PS->Server_EndTurn(GetUniqueID());
		break;
	}

	case ECardSelectPurpose::GeneralStoreDraft:
		// 잡화점 – 전체 플레이어가 순서대로 카드 중 1장 선택
		//MyInfo.SelectableCards 삭제 선택한 카드 보유카드에 추가.
		break;

	case ECardSelectPurpose::KitCarlsonDrawCard:
		// 키트 칼슨 능력 – 카드 3장 중 2장 선택
		// 안뽑은 1장을 뽑을카드더미에 올리기
		break;

	case ECardSelectPurpose::StealFromOpponent:
		// 상대의 보유 카드 중 1장을 선택 (정보가 안 보일 수 있음)
		break;

	case ECardSelectPurpose::RespondToDuel:
		// 결투 중 뱅 카드 선택
		break;

	case ECardSelectPurpose::RespondToIndians:
		// 인디언 카드 대응 – 뱅 카드 선택
		break;

	case ECardSelectPurpose::RespondToAttack:
		// Bang, Gatling 등의 공격에 대해 Missed 카드 선택
		break;

	default:
		break;
	}
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
		
		if (HitChar && HitChar != GetPawn())
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

void ABangPlayerController::Server_UseCard_Implementation(const FSingleCard& SingleCard, int32 TargetID)
{
	ABangPlayerState* PS = GetPlayerState<ABangPlayerState>();
	if (!PS) return;

	const int32 FromID = PS->GetUniqueID(); // 혹은 PlayerState에 저장된 내 ID
	PS->UseCard(FromID, SingleCard, TargetID);
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