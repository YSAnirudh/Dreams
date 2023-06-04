// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UIDebug/DebugWidget.h"
#include "DreamHUD.generated.h"

/**
 * 
 */
UCLASS()
class DREAMS_API ADreamHUD : public AHUD
{
	GENERATED_BODY()
public:
	// --- FUNCTIONS ---
	// Sets default values for this widget's properties
	ADreamHUD();

	virtual void DrawHUD() override;

	virtual void Tick(float DeltaSeconds) override;
	
	// --- VARIABLES ---
protected:
	// --- FUNCTIONS ---
	virtual void BeginPlay() override;

	// --- VARIABLES ---
private:
	// --- FUNCTIONS ---

	// --- VARIABLES ---
	// Debug Widget for Movement Debugs
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> DebugWidgetClass;
	
	TObjectPtr<class UDebugWidget> DebugWidget = nullptr;
};
