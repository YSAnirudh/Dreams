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
	FORCEINLINE bool GetWantsToSprint() const { return Safe_bWantsToSprint; }
	
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
	FORCEINLINE bool GetWantsToDash() const { return Safe_bWantsToDash; }

	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// --- VARIABLES ---
	// Delegates
	// Dash Start Delegate
	UPROPERTY(BlueprintAssignable)
	FDashStartDelegate DashStartDelegate;
protected:
	// --- FUNCTIONS ---
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	
	// --- VARIABLES ---
private:
	// --- FUNCTIONS ---
	// Dash Mechanic
	void OnDashCooldownFinished();
	bool CanDash() const;
	void PerformDash();

	// Replication
	UFUNCTION()
	void OnRep_DashStart();

	// --- VARIABLES ---
	// Dash
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement", meta = (AllowPrivateAccess = "true"))
	float DashImpulse = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement", meta = (AllowPrivateAccess = "true"))
	float DashCooldownDuration = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement", meta = (AllowPrivateAccess = "true"))
	float AuthDashCooldownDuration = 0.9f;


	// Safe WantsTo Variables
	bool Safe_bWantsToSprint;
	bool Safe_bWantsToDash;

	// Transient Variables (Counter)
	float DashStartTime;
	FTimerHandle TimerHandle_DashCooldown;

	// Replication
	UPROPERTY(ReplicatedUsing=OnRep_DashStart)
	bool Proxy_bDashStart;

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
	// Saved Move
	class FSavedMove_Main : public FSavedMove_Character
	{
	public:
		enum CompressedFlags
		{
			FLAG_Sprint			= 0x10,
			FLAG_Dash			= 0x20,
			FLAG_Custom_2		= 0x30,
			FLAG_Custom_3		= 0x40
		};
		
		FSavedMove_Main();

		uint8 Saved_bWantsToSprint:1;
		uint8 Saved_bWantsToDash:1;

		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;
	};

	class FNetworkPredictionData_Client_Main : public FNetworkPredictionData_Client_Character
	{
	public:
		FNetworkPredictionData_Client_Main(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;

		virtual FSavedMovePtr AllocateNewMove() override;
	};
};
