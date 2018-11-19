// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreUObject.h"
#include "Engine.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "QQSDKFunctions.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintable)
class QQSDK_API UQQSDKFunctions : public UObject
{
	GENERATED_BODY()
	
public:

#if PLATFORM_ANDROID 
	static void InitAndroidFunctions();
#endif

#if PLATFORM_IOS
	static void InitIOSFunctions();
#endif

	static bool isQQInstalled();

	/**
	* 分享新闻到QQ
	* @param Title               分享的标题。注：PARAM_TITLE、PARAM_IMAGE_URL、PARAM_SUMMARY不能全为空，最少必须有一个是有值的。
	* @param Summary             分享的消息摘要，最长50个字
	* @param TargetURL           这条分享消息被好友点击后的跳转URL
	* @param ImageURL            缩略图图地址
	* @param APPName             手Q客户端顶部，替换“返回”按钮文字，如果为空，用返回代替(IOS该字段无效)
	*/
	UFUNCTION(BlueprintCallable, meta = (Keywords = "QQSDK "), Category = "QQSDK|")
	static void ShareToQQ_News(const FString& Title, const FString& Summary, const FString& TargetURL, const FString& ImageURL, const FString& APPName);

	/**
	* 分享新闻到QQ空间
	* @param Title               分享的标题(必填)
	* @param Summary             分享的消息摘要(必填)
	* @param TargetURL           跳转URL(必填)
	* @param ImageURL            缩略图图地址
	*/
	UFUNCTION(BlueprintCallable, meta = (Keywords = "QQSDK "), Category = "QQSDK|")
	static void ShareToQzone_News(const FString& Title, const FString& Summary, const FString& TargetURL, const FString& ImageURL);
};
