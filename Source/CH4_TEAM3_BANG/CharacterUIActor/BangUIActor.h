// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BangUIActor.generated.h"

class UTextRenderComponent;
UCLASS()
class CH4_TEAM3_BANG_API ABangUIActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABangUIActor();
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetDisplayText(const FName& NewText);
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UTextRenderComponent> TextRenderComponent;

	UPROPERTY(ReplicatedUsing = OnRep_DisplayText, VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	FName DisplayText;

	UFUNCTION()
	void OnRep_DisplayText();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
