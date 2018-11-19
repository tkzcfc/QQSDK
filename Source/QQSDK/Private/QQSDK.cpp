// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "QQSDK.h"
#include "Core.h"
#include "ModuleManager.h"
#include "IPluginManager.h"
#include "QQSDKFunctions.h"


#define LOCTEXT_NAMESPACE "FQQSDKModule"

class FQQSDKModule : public IQQSDKModule
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FQQSDKModule, QQSDK)

void FQQSDKModule::StartupModule()
{
#if PLATFORM_ANDROID 
	UQQSDKFunctions::InitAndroidFunctions();
#endif
}

void FQQSDKModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
