// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QQSDKFunctions.h"
#include "QQSDKComponent.generated.h"


UCLASS(ClassGroup = Advertising, HideCategories = (Activation, "Components|Activation", Collision), meta = (BlueprintSpawnableComponent))
class QQSDK_API UQQSDKComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	DECLARE_MULTICAST_DELEGATE_TwoParams(FQQSDKonShareResultDelegate, int32, FString);

	static FQQSDKonShareResultDelegate onShareResultDelegate;

	/**
	* 分享结果回调
	* @param Result  0分享失败 1分享成功 -1分享取消
	* @param Error   错误信息
	*/
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FQQSDKonShareResultDynDelegate, int32, Result, FString, Error);

	UPROPERTY(BlueprintAssignable)
	FQQSDKonShareResultDynDelegate onShareResult;

	void OnRegister() override;
	void OnUnregister() override;

private:
	UFUNCTION()
	void onShareResult_Handler(int32 Result, FString Error) { onShareResult.Broadcast(Result, Error); }
	
};
