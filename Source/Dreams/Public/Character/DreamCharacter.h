// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Input/InputActionsConfig.h"
#include "DreamCharacter.generated.h"

UCLASS()
class DREAMS_API ADreamCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// --- FUNCTIONS ---

	// Sets default values for this character's properties
	ADreamCharacter(const FObjectInitializer& ObjectInitializer);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Move Input - WASD/L Stick
	UFUNCTION(BlueprintCallable)
	void Move(const FInputActionValue& ActionValue);
	// Look Input - Mouse Move/Right Stick
	UFUNCTION(BlueprintCallable)
	void Look(const FInputActionValue& ActionValue);
	// Sprint Input - L Shift/RT
	UFUNCTION(BlueprintCallable)
	void Sprint(const FInputActionValue& ActionValue);
	UFUNCTION(BlueprintCallable)
	void StopSprint(const FInputActionValue& ActionValue);
	// Crouch Input - C/LB
	UFUNCTION(BlueprintCallable)
	void StartCrouch(const FInputActionValue& ActionValue);
	UFUNCTION(BlueprintCallable)
	void StopCrouch(const FInputActionValue& ActionValue);
	// Dash Input - L Alt
	UFUNCTION(BlueprintCallable)
	void Dash(const FInputActionValue& ActionValue);
	UFUNCTION(BlueprintCallable)
	void StopDash(const FInputActionValue& ActionValue);

	// INLINE FUNCTIONS
	// Get Camera Boom Spring Arm
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	// Get Follow Camera
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	// Get Custom Character Movement Component
	FORCEINLINE class UDreamCharacterMovementComponent* GetDreamCharacterMovement() const { return DreamCharacterMovementComponent; }

	// --- VARIABLES ---

protected:
	// --- FUNCTIONS ---
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Add the required Input Mapping Contexts to handle enhanced input
	virtual void AddMappingContexts();

	// --- VARIABLES ---
	// Our Custom Movement Component
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character|Movement", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UDreamCharacterMovementComponent> DreamCharacterMovementComponent = nullptr;

private:	
	// --- FUNCTIONS ---

	// --- VARIABLES ---
	
	// SETTINGS - (Ex: Sensitivity)
	// Mouse Sensitivity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings", meta = (AllowPrivateAccess = "true"))
	float MouseSensitivityX = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings", meta = (AllowPrivateAccess = "true"))
	float MouseSensitivityY = 1.0f;
	
	// Toggle Sprint boolean (Toggle if true, Hold if false)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings", meta = (AllowPrivateAccess = "true"))
	bool bSprintToggle = false;
	
	// Toggle Crouch boolean (Toggle if true, Hold if false)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings", meta = (AllowPrivateAccess = "true"))
    bool bCrouchToggle = false;
	
	// CAMERA
	// Spring Arm
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera|SpringArm", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;
	// Camera
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera|SpringArm", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	// INPUT - Mapping Contexts
	// Base Mapping context to handle Ground Movement and Basic Input
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PlayerInput|Mapping", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> BaseMappingContext;

	// INPUT - Action Config
	// Base Input Action Config to handle Ground Movement and Basic Input
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PlayerInput|Mapping", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputActionsConfig> BaseInputActionsConfig;
};
