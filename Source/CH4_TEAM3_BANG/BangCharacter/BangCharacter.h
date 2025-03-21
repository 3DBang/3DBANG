// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BangCharacter.generated.h"

struct FInputActionValue;
class USpringArmComponent;
class UCameraComponent;

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


};
