// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Engine/DataAsset.h"
#include "InputActionsConfig.generated.h"

/**
 * A Static Class to Store input actions
 */
UCLASS()
class DREAMS_API UInputActionsConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	// --- VARIABLES ---
	// INPUT - Actions
	// Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input|Actions")
	TObjectPtr<UInputAction> MovementAction;

	// Look
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input|Actions")
	TObjectPtr<UInputAction> LookAction;

	// Jump
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input|Actions")
	TObjectPtr<UInputAction> JumpAction;

	// Sprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input|Actions")
	TObjectPtr<UInputAction> SprintAction;
	
	// Crouch
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input|Actions")
	TObjectPtr<UInputAction> CrouchAction;

	// Walk
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input|Actions")
	TObjectPtr<UInputAction> WalkAction;
	
	// Dash
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input|Actions")
	TObjectPtr<UInputAction> DashAction;
	
	// Interact
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input|Actions")
	TObjectPtr<UInputAction> InteractAction;
};
