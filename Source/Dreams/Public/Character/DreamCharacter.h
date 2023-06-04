// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Input/InputActionsConfig.h"
#include "Interaction/Interactable.h"
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

	#pragma region Input
	// Move Input - WASD/L Stick
	UFUNCTION(BlueprintCallable)
	void Move(const FInputActionValue& ActionValue);
	
	// Look Input - Mouse Move/Right Stick
	UFUNCTION(BlueprintCallable)
	void Look(const FInputActionValue& ActionValue);
	
	// Jump Input - Space/ O
	virtual void Jump() override;
	virtual void StopJumping() override;
	
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

	// Walk Input - L Ctrl/LT
	UFUNCTION(BlueprintCallable)
	void StartWalk(const FInputActionValue& ActionValue);
	UFUNCTION(BlueprintCallable)
	void StopWalk(const FInputActionValue& ActionValue);
	
	// Dash Input - L Alt
	UFUNCTION(BlueprintCallable)
	void Dash(const FInputActionValue& ActionValue);
	UFUNCTION(BlueprintCallable)
	void StopDash(const FInputActionValue& ActionValue);
	#pragma endregion
	
	// Get Actors (self and children) to ignore while performing a line trace
	FCollisionQueryParams GetIgnoreCharacterParams() const;
	
	// INLINE FUNCTIONS
	// Get Camera Boom Spring Arm
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	// Get Follow Camera
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	// Get Custom Character Movement Component
	FORCEINLINE class UDreamCharacterMovementComponent* GetDreamCharacterMovement() const { return DreamCharacterMovementComponent; }
	
	// --- VARIABLES ---
	// Boolean to Override basic jump
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Player|Movement")
	bool bPressedDreamJump;
	// Boolean to keep track of whether the Character is Sprinting or not
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Player|Movement")
	bool bIsInAir;
	// Boolean to keep track of whether the Character is Sprinting or not
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Player|Movement")
	bool bIsSprinting;
	// Boolean to keep track of whether the Character is Walking or not
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Player|Movement")
	bool bIsWalking;
	// Boolean to keep track of whether the Character is Jogging or not
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Player|Movement")
	bool bIsJogging;
	// Boolean to keep track of whether the Character is Jogging or not
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Player|Movement")
    bool bIsSliding;
	// Boolean to keep track of whether the Character is WallRunning or not
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Player|Movement")
	bool bIsWallRunning;
	
protected:
	// --- FUNCTIONS ---
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Add the required Input Mapping Contexts to handle enhanced input
	virtual void AddMappingContexts();

	// --- VARIABLES ---
	// Our Custom Movement Component
	TObjectPtr<class UDreamCharacterMovementComponent> DreamCharacterMovementComponent = nullptr;

private:	
	// --- FUNCTIONS ---
	// Function to trace for objects and set WithinRadius on the interactable
	void FindInteractable();
	// Function to trace for objects and set CanInteract on the interactable
	void FindInteractableObjectsWithinRadius() const;
	// Function to check if the character is facing the Interactable
	bool CharacterFacingInteractable(FVector InteractableLocation) const;

	// --- VARIABLES ---
	
	// SETTINGS - (Ex: Sensitivity)
	#pragma region Settings
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

	// Toggle Walk boolean (Toggle if true, Hold if false)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings", meta = (AllowPrivateAccess = "true"))
	bool bWalkToggle = false;
	#pragma endregion

	// INTERACTION
	#pragma region Interaction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player|Interaction", meta = (AllowPrivateAccess = "true"))
	float InteractionRadius = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player|Interaction", meta = (AllowPrivateAccess = "true"))
	float InteractionRange = 200.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Player|Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AInteractable> InteractableActor = nullptr;
	TObjectPtr<AInteractable> PrevInteractableActor = nullptr;

	#pragma endregion 

	// CAMERA
	// Spring Arm
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player|Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;
	// Camera
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player|Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	// INPUT - Mapping Contexts
	// Base Mapping context to handle Ground Movement and Basic Input
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player|Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> BaseMappingContext;

	// INPUT - Action Config
	// Base Input Action Config to handle Ground Movement and Basic Input
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player|Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputActionsConfig> BaseInputActionsConfig;
};
