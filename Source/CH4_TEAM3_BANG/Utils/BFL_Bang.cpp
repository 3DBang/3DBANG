// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/BFL_Bang.h"

#include "Windows/WindowsPlatformHttp.h"

FString UBFL_Bang::UrlEncode(FString InString)
{
	FString EncodedString = FPlatformHttp::UrlEncode(InString);
	// UTF-8 문자열을 FString으로 반환
	return EncodedString;

}

FString UBFL_Bang::UrlDecode(FString InString)
{
	FString DecodedString = FPlatformHttp::UrlDecode(InString);
	return DecodedString;
}
