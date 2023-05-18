// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "DreamCharacterMovementComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDashStartDelegate);

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_None		UMETA(Hidden),
	CMOVE_MAX		UMETA(HIdden)
};

/**
 * 
 */
UCLASS()
class DREAMS_API UDreamCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	// --- FUNCTIONS ---
	UDreamCharacterMovementComponent();

	// Movement Functions to handle User Input
	// SPRINT
	UFUNCTION(BlueprintCallable)
	void SprintPressed();
	UFUNCTION(BlueprintCallable)
	void SprintReleased();
	// Get if the player wants to sprint
	FORCEINLINE bool GetWantsToSprint() const { return bWantsToSprint; }
	
	// CROUCH
    UFUNCTION(BlueprintCallable)
    void CrouchPressed();
    UFUNCTION(BlueprintCallable)
    void CrouchReleased();
	// Get if the player wants to crouch
	FORCEINLINE bool GetWantsToCrouch() const { return bWantsToCrouch; }

	// DASH
	UFUNCTION(BlueprintCallable)
	void DashPressed();
	UFUNCTION(BlueprintCallable)
	void DashReleased();
	// Get if the player wants to dash
	FORCEINLINE bool GetWantsToDash() const { return bWantsToDash; }

	// --- VARIABLES ---
	// Delegates
	// Dash Start Delegate
	UPROPERTY(BlueprintAssignable)
	FDashStartDelegate DashStartDelegate;
protected:
	// --- FUNCTIONS ---
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	
	// --- VARIABLES ---
private:
	// --- FUNCTIONS ---
	// Dash Mechanic
	void OnDashCooldownFinished();
	bool CanDash() const;
	void PerformDash();

	// --- VARIABLES ---
	// Dash
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement", meta = (AllowPrivateAccess = "true"))
	float DashImpulse = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement", meta = (AllowPrivateAccess = "true"))
	float DashCooldownDuration = 1.f;


	// Safe WantsTo Variables
	bool bWantsToSprint;
	bool bWantsToDash;

	// Transient Variables (Counter)
	float DashStartTime;
	FTimerHandle TimerHandle_DashCooldown;

	// Speed while Standing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement", meta = (AllowPrivateAccess = "true"))
	float Sprint_MaxWalkSpeed = 900.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement", meta = (AllowPrivateAccess = "true"))
	float Jog_MaxWalkSpeed = 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement", meta = (AllowPrivateAccess = "true"))
	float Walk_MaxWalkSpeed = 300.f;

	// Speed while Crouched
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement", meta = (AllowPrivateAccess = "true"))
	float CrouchJog_MaxWalkSpeed = 400.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement", meta = (AllowPrivateAccess = "true"))
	float CrouchWalk_MaxWalkSpeed = 200.f;
	
	// --- CLASSES ---
};
