// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DreamHUD.h"

ADreamHUD::ADreamHUD()
{
}

void ADreamHUD::DrawHUD()
{
	Super::DrawHUD();
}

void ADreamHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ADreamHUD::BeginPlay()
{
	Super::BeginPlay();
	
	if (!DebugWidget && DebugWidgetClass)
	{
		DebugWidget = CreateWidget<UDebugWidget>(GetWorld(), DebugWidgetClass, TEXT("Movement Debug Widget"));
		DebugWidget->AddToViewport();
	}
}

void UDebugWidget::NativeConstruct()
{
	Super::NativeConstruct();
}
