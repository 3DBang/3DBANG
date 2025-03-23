// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BFL_Bang.generated.h"

/**
 * 
 */
UCLASS()
class CH4_TEAM3_BANG_API UBFL_Bang : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintPure, Category = "BFL_Bang")
	static FString UrlEncode(FString InString);

	UFUNCTION(BlueprintPure, Category = "BFL_Bang")
	static FString UrlDecode(FString InString);
};
