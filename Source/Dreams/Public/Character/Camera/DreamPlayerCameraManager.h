// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "DreamPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class DREAMS_API ADreamPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
public:	
	// --- FUNCTIONS ---
	// Constructor
	ADreamPlayerCameraManager();

	// Handle View target update logic (Camera View Update)
	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
	
	// --- VARIABLES ---
protected:	
	// --- FUNCTIONS ---

	// --- VARIABLES ---
private:	
	// --- FUNCTIONS ---

	// --- VARIABLES ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Crouch", meta=(AllowPrivateAccess = "true"))\
	float CrouchBlendDuration = 0.5f;

	float CrouchBlendTime;
};
