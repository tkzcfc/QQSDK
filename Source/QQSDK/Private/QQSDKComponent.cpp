// Fill out your copyright notice in the Description page of Project Settings.

#include "QQSDKComponent.h"


void UQQSDKComponent::OnRegister()
{
	Super::OnRegister();

	UQQSDKComponent::onShareResultDelegate.AddUObject(this, &UQQSDKComponent::onShareResult_Handler);
}

void UQQSDKComponent::OnUnregister()
{
	Super::OnUnregister();

	UQQSDKComponent::onShareResultDelegate.RemoveAll(this);
}

UQQSDKComponent::FQQSDKonShareResultDelegate UQQSDKComponent::onShareResultDelegate;