# QQSDK
ue4 qq

引擎版本:4.20

使用方法
一、将该插件拷贝至工程目录下的Plugins文件夹中

二、APPID修改

[Android] Source/QQSDK/QQSDK_APL.xml 39行
[Android] Source/QQSDK/QQSDK_APL.xml 78行

[IOS] APPID修改:

		(1) Source/QQSDK/QQSDK_AdditionalPlistData.xml中38行

		(2) Source/QQSDK/Private/QQSDKFunctions.cpp中114行

三、编辑器设置
[IOS] 打开编辑器 [编辑]->[项目设置]，将修改后Source/QQSDK/QQSDK_AdditionalPlistData.xml文件中的内容拷贝到[平台]->[IOS]->[Extra Plist Data]->[Additional Plist Data]中
	
	注 ：
	
	1)这一步可以类似安卓写xml配置文件，IOS的有点复杂，有兴趣的可以自己去试试)
		
	2)WeixinSDK插件也是类似的做法，要同时使用这两个插件要将两个xml文件的元素合一下,合过的代码我放在工程Resources/QQ&WX.xml中的
		 
四、目前只有分享新闻功能，需要其他可以自己加
