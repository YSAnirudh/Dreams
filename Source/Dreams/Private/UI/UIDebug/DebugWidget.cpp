// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UIDebug/DebugWidget.h"

void UDebugWidget::DebugToggle()
{
	bDebug = !bDebug;

	if (bDebug)
	{
		SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}