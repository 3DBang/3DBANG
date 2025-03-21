#include "BangPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "CharacterUIActor/BangUIActor.h"
#include "BangCharacter/BangCharacter.h"

ABangPlayerController::ABangPlayerController()
{
	UE_LOG(LogTemp, Error, TEXT("플레이어 컨트롤러가 생성되었습니다 순서는 누가 더 빠른가요?"));
}

void ABangPlayerController::BeginPlay()
{
	Super::BeginPlay();

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
	//여기에서 HasAuthoriy를 사용하면 서버이자 클라이언트는 Tick이 활성화가 되지 않는다
	if (IsLocalController())
	{
		SetActorTickEnabled(true);
	}
	else
	{
		SetActorTickEnabled(false);
	}
	/*FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetWidgetToFocus(nullptr);
	SetInputMode(InputMode);*/
	bShowMouseCursor = true;
}

void ABangPlayerController::Server_UseCardReturn_Implementation(bool IsAble)
{
	
}

void ABangPlayerController::Server_EndTurn_Implementation()
{
	ABangGameModeBase* GM = GetWorld()->GetAuthGameMode<ABangGameModeBase>();
	if (GM)
	{
		GM->EndTurn();
	}
}

/*void ABangPlayerController::Server_AttackPlayer_Implementation(APlayerState* TargetPlayer)
{
	ABangGameModeBase* GM = GetWorld()->GetAuthGameMode<ABangGameModeBase>();
	if (GM)
	{
		GM->HandleAttack(GetPlayerState<ABangPlayerState>(), Cast<ABangPlayerState>(TargetPlayer));
	}
}*/
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
		ABangCharacter* BangCharacterHP = Cast<ABangCharacter>(GetPawn());
		if (BangCharacterHP)
		{
			BangCharacterHP->UpdateHPActors(NewHP);
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
///////////////////////////
//// 원명 추가 
//////////////////////////
void ABangPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FHitResult HitResult;
	if (GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, HitResult))
	{
		DrawDebugSphere(GetWorld(), HitResult.Location, 10.f, 8, FColor::Red, false, 1.5f);
		ACharacter* HitChar = Cast<ACharacter>(HitResult.GetActor());
		if (HitChar && HitChar != GetPawn())
		{
			if (ABangCharacter* OtherPlayer = Cast<ABangCharacter>(HitChar))
			{
				OtherPlayers = OtherPlayer;
				CurrentMouseCursor = EMouseCursor::Hand;
				return;
			}
		}
	}
	OtherPlayers = nullptr;
	CurrentMouseCursor = EMouseCursor::Default;
}

