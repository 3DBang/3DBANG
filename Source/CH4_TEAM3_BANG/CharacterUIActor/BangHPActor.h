// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BangHPActor.generated.h"

UCLASS()
class CH4_TEAM3_BANG_API ABangHPActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABangHPActor();

	UFUNCTION(BlueprintCallable, Category = "HP")
	void SetHiddenActorState(bool bIsHiddenHp);
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HP")
	UStaticMeshComponent* MeshComponent;
	/**불 값을 서버와 동기화 시켜서 Hidden처리 , Hidden은 서버와 자동동기화가 되지 않음 */
	UPROPERTY(ReplicatedUsing = OnRep_HiddenState, VisibleAnywhere, Category = "HP")
	bool bIsHpHidden = false;

	UFUNCTION()
	void OnRep_HiddenState();
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
