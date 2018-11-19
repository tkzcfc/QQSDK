// Fill out your copyright notice in the Description page of Project Settings.

#include "QQSDKFunctions.h"
#include "QQSDKComponent.h"


#if PLATFORM_ANDROID

#include "Android/AndroidJNI.h"
#include "AndroidApplication.h"


#define INIT_JAVA_METHOD(name, signature) \
if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true)) { \
	name = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, #name, signature, false); \
	check(name != NULL); \
} else { \
	check(0); \
}

#define DECLARE_JAVA_METHOD(name) \
static jmethodID name = NULL;

DECLARE_JAVA_METHOD(AndroidThunkJava_QQSDK_ShareToQQ_News);
DECLARE_JAVA_METHOD(AndroidThunkJava_QQSDK_ShareToQzone_News);

void UQQSDKFunctions::InitAndroidFunctions()
{
	static bool QQSDK_InitAndroidFunctions_Tag = false;

	if (QQSDK_InitAndroidFunctions_Tag)
	{
		return;
	}

	QQSDK_InitAndroidFunctions_Tag = true;

	//(String Title, String Summary, String TargetURL, String ImageURL, String APPName)
	INIT_JAVA_METHOD(AndroidThunkJava_QQSDK_ShareToQQ_News, "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	//(String Title, String Summary, String TargetURL, String ImageURL)
	INIT_JAVA_METHOD(AndroidThunkJava_QQSDK_ShareToQzone_News, "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
}

#undef DECLARE_JAVA_METHOD
#undef INIT_JAVA_METHOD

/**
* 分享结果回调
* @param result  0分享失败 1分享成功 -1分享取消
* @param error   分享错误信息
*/
extern "C" void Java_com_epicgames_ue4_GameActivity_nativeOnTencentShareResult(JNIEnv* jenv, jobject thiz, jint result, jstring error)
{
	FString ErrorMsg;

	const char* charsResult = jenv->GetStringUTFChars(error, 0);
	ErrorMsg = FString(UTF8_TO_TCHAR(charsResult));
	jenv->ReleaseStringUTFChars(error, charsResult);

	if (ErrorMsg.IsEmpty() == false)
	{
		UE_LOG(LogAndroid, Warning, TEXT("nativeOnTencentShareResult ErrorMsg : %s "), *ErrorMsg);
	}

	AsyncTask(ENamedThreads::GameThread, [=]()
	{
		UQQSDKComponent::onShareResultDelegate.Broadcast(int32(result), ErrorMsg);
	});
}

#endif

#if PLATFORM_IOS

#import "IOS/IOSAppDelegate.h"
#import <TencentOpenAPI/TencentOAuth.h>
#import <TencentOpenAPI/QQApiInterfaceObject.h>
#import <TencentOpenAPI/QQApiInterface.h>


@interface QQSDKDelegate :NSObject<QQApiInterfaceDelegate>
@end

static QQSDKDelegate* Static_QQSDKDelegate_Instance = NULL;

@implementation QQSDKDelegate


+ (void)load
{
	if (!Static_QQSDKDelegate_Instance)
	{
		Static_QQSDKDelegate_Instance = [[QQSDKDelegate alloc] init];
	}
}

-(id)init
{
	self = [super init];

	if (self)
	{
		[[NSNotificationCenter defaultCenter] addObserver:self
			selector : @selector(applicationDidFinishLaunching : )
					   name : UIApplicationDidFinishLaunchingNotification
			object : nil];
	}

	return self;
}

-(void)applicationDidFinishLaunching:(NSNotification *)n
{
	[[TencentOAuth alloc] initWithAppId:@"111111111" andDelegate : nil];
}

// qq分享结果的回调
-(void)onResp : (QQBaseResp *)resp
{
	if ([resp isKindOfClass : [SendMessageToQQResp class]] && resp.type == ESENDMESSAGETOQQRESPTYPE)
	{
		SendMessageToQQResp* sendReq = (SendMessageToQQResp*)resp;

		// 0分享失败 1分享成功 -1分享取消
		int32 Result = 0;
		if ([sendReq.result integerValue] == 0)
		{
			Result = 1;
		}
		else if ([sendReq.result integerValue] == -5)
		{
			Result = -1;
		}

		AsyncTask(ENamedThreads::GameThread, [=]()
		{
			UQQSDKComponent::onShareResultDelegate.Broadcast(Result, TEXT(""));
		});
	}
}

/**
处理来至QQ的请求
*/
-(void)onReq:(QQBaseReq *)req
{}

/**
处理QQ在线状态的回调
*/
-(void)isOnlineResponse:(NSDictionary *)response
{}

+(void)handleSendResult : (QQApiSendResultCode)sendResult 
{
	switch (sendResult) 
	{
	case EQQAPIAPPNOTREGISTED:
	{
		UIAlertView *msgbox = [[UIAlertView alloc] initWithTitle:@"Error" message : @"App未注册" delegate:nil cancelButtonTitle:@"取消" otherButtonTitles:nil];
		[msgbox show];
		break;
	}
	case EQQAPIMESSAGECONTENTINVALID:
	case EQQAPIMESSAGECONTENTNULL:
	case EQQAPIMESSAGETYPEINVALID:
	{
		UIAlertView *msgbox = [[UIAlertView alloc] initWithTitle:@"Error" message : @"发送参数错误" delegate:nil cancelButtonTitle:@"取消" otherButtonTitles:nil];
		[msgbox show];
		break;
	}
	case EQQAPIQQNOTINSTALLED:
	{
		UIAlertView *msgbox = [[UIAlertView alloc] initWithTitle:@"Error" message : @"未安装手Q" delegate:nil cancelButtonTitle:@"取消" otherButtonTitles:nil];
		[msgbox show];
		break;
	}
	case EQQAPIQQNOTSUPPORTAPI:
	{
		UIAlertView *msgbox = [[UIAlertView alloc] initWithTitle:@"Error" message : @"API接口不支持" delegate:nil cancelButtonTitle:@"取消" otherButtonTitles:nil];
		[msgbox show];
		break;
	}
	case EQQAPISENDFAILD:
	{
		UIAlertView *msgbox = [[UIAlertView alloc] initWithTitle:@"Error" message : @"发送失败" delegate:nil cancelButtonTitle:@"取消" otherButtonTitles:nil];
		[msgbox show];
		break;
	}
	default:
		break;
	}
}

@end

static void onQQOpenURL(UIApplication*, NSURL* url, NSString*, id)
{
	[QQApiInterface handleOpenURL : url delegate : Static_QQSDKDelegate_Instance];
}

void UQQSDKFunctions::InitIOSFunctions()
{
	static bool IsIOS_QQ_RegisterOpenURLCall = false;
	if (IsIOS_QQ_RegisterOpenURLCall == false)
	{
		FIOSCoreDelegates::OnOpenURL.AddStatic(onQQOpenURL);
		IsIOS_QQ_RegisterOpenURLCall = true;
	}
}

#endif

bool UQQSDKFunctions::isQQInstalled()
{
#if PLATFORM_ANDROID
	return true;
#endif
#if PLATFORM_IOS
	return [QQApiInterface isQQInstalled] && [QQApiInterface isQQSupportApi];
#endif
	return false;
}

void UQQSDKFunctions::ShareToQQ_News(const FString& Title, const FString& Summary, const FString& TargetURL, const FString& ImageURL, const FString& APPName)
{
	if (!isQQInstalled())
	{
		return;
	}
#if PLATFORM_ANDROID
	InitAndroidFunctions();

	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true))
	{
		if (AndroidThunkJava_QQSDK_ShareToQQ_Default == 0)
		{
			UE_LOG(LogAndroid, Warning, TEXT("I can't found the java method AndroidThunkJava_QQSDK_ShareToQQ_Default()\n"));
			return;
		}

		jstring TitleArg = Env->NewStringUTF(TCHAR_TO_UTF8(*Title));
		jstring SummaryArg = Env->NewStringUTF(TCHAR_TO_UTF8(*Summary));
		jstring TargetURLArg = Env->NewStringUTF(TCHAR_TO_UTF8(*TargetURL));
		jstring ImageURLArg = Env->NewStringUTF(TCHAR_TO_UTF8(*ImageURL));
		jstring APPNameArg = Env->NewStringUTF(TCHAR_TO_UTF8(*APPName));

		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, AndroidThunkJava_QQSDK_ShareToQQ_Default, TitleArg, SummaryArg, TargetURLArg, ImageURLArg, APPNameArg);

		Env->DeleteLocalRef(TitleArg);
		Env->DeleteLocalRef(SummaryArg);
		Env->DeleteLocalRef(TargetURLArg);
		Env->DeleteLocalRef(ImageURLArg);
		Env->DeleteLocalRef(APPNameArg);

		UE_LOG(LogAndroid, Warning, TEXT("I found the java method AndroidThunkJava_QQSDK_ShareToQQ_Default()\n"));
	}
	else
	{
		UE_LOG(LogAndroid, Warning, TEXT("ERROR Could note get Java ENV\n"));
	}
#endif

#if PLATFORM_IOS
	InitIOSFunctions();

	NSString* TitleArg = Title.GetNSString();
	NSString* SummaryArg = Summary.GetNSString();
	NSString* TargetURLArg = TargetURL.GetNSString();
	NSString* ImageURLArg = ImageURL.GetNSString();
	//NSString* APPNameArg = APPName.GetNSString();

	QQApiNewsObject*newsObj = [QQApiNewsObject
		objectWithURL : [NSURL URLWithString : TargetURLArg]
				title : TitleArg
		  description : SummaryArg
	  previewImageURL : [NSURL URLWithString : ImageURLArg]];

	SendMessageToQQReq*req = [SendMessageToQQReq reqWithContent : newsObj];

	dispatch_async(dispatch_get_main_queue(), ^{
		QQApiSendResultCode sent = [QQApiInterface sendReq : req];
		[QQSDKDelegate handleSendResult : sent];
	});

#endif
}


void UQQSDKFunctions::ShareToQzone_News(const FString& Title, const FString& Summary, const FString& TargetURL, const FString& ImageURL)
{
	if (!isQQInstalled())
	{
		return;
	}
#if PLATFORM_ANDROID
	InitAndroidFunctions();

	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv(true))
	{
		if (AndroidThunkJava_QQSDK_ShareToQzone_ImgText == 0)
		{
			UE_LOG(LogAndroid, Warning, TEXT("I can't found the java method AndroidThunkJava_QQSDK_ShareToQzone_ImgText()\n"));
			return;
		}

		jstring TitleArg = Env->NewStringUTF(TCHAR_TO_UTF8(*Title));
		jstring SummaryArg = Env->NewStringUTF(TCHAR_TO_UTF8(*Summary));
		jstring TargetURLArg = Env->NewStringUTF(TCHAR_TO_UTF8(*TargetURL));
		jstring ImageURLArg = Env->NewStringUTF(TCHAR_TO_UTF8(*ImageURL));

		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, AndroidThunkJava_QQSDK_ShareToQzone_ImgText, TitleArg, SummaryArg, TargetURLArg, ImageURLArg);

		Env->DeleteLocalRef(TitleArg);
		Env->DeleteLocalRef(SummaryArg);
		Env->DeleteLocalRef(TargetURLArg);
		Env->DeleteLocalRef(ImageURLArg);

		UE_LOG(LogAndroid, Warning, TEXT("I found the java method AndroidThunkJava_QQSDK_ShareToQzone_ImgText()\n"));
	}
	else
	{
		UE_LOG(LogAndroid, Warning, TEXT("ERROR Could note get Java ENV\n"));
	}
#endif

#if PLATFORM_IOS
	InitIOSFunctions();

	NSString* TitleArg = Title.GetNSString();
	NSString* SummaryArg = Summary.GetNSString();
	NSString* TargetURLArg = TargetURL.GetNSString();
	NSString* ImageURLArg = ImageURL.GetNSString();

	QQApiNewsObject*newsObj = [QQApiNewsObject
		objectWithURL : [NSURL URLWithString : TargetURLArg]
				title : TitleArg
		  description : SummaryArg
	  previewImageURL : [NSURL URLWithString : ImageURLArg]];

	SendMessageToQQReq*req = [SendMessageToQQReq reqWithContent : newsObj];

	dispatch_async(dispatch_get_main_queue(), ^{
		QQApiSendResultCode sent = [QQApiInterface SendReqToQZone : req];
		[QQSDKDelegate handleSendResult : sent];
	});

#endif
}

