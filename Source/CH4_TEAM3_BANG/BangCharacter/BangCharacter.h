// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BangCharacter.generated.h"

struct FInputActionValue;
class USpringArmComponent;
class UCameraComponent;
class ABangUIActor;
class UTextRenderComponent;
class ABangHPActor;

UCLASS()
class CH4_TEAM3_BANG_API ABangCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABangCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//////////////////////////
	/// Enhanced Input
	//////////////////////////
	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void Look(const FInputActionValue& Value);

	UFUNCTION()
	void Menu(const FInputActionValue& Value);

	UFUNCTION()
	void Information(const FInputActionValue& Value);

	UFUNCTION()
	void Zoom(const FInputActionValue& Value);

	UFUNCTION()
	void Click(const FInputActionValue& Value);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

public:
	
	//BluePrint에서 BP_xxx를 사용하기 위한 Subclass
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<ABangUIActor> TextActorUIClass;
	
	//실제 담을 인스턴스
	UPROPERTY()
	TWeakObjectPtr<ABangUIActor> TextActor;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HP")
	TSubclassOf<ABangHPActor> HPActorClass;

	// 스폰한 HPActor들을 저장할 배열
	UPROPERTY()
	TArray<TWeakObjectPtr<ABangHPActor>> HPActors;

public:
	void UpdateHPActors(int32 NewHP);
	void SetHP(int32 NewHP);
private:
	int32 HP = 5;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};
