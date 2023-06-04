// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/DreamCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "DreamCharacterMovementComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDashStartDelegate);

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_None		UMETA(Hidden),
	CMOVE_Slide     UMETA(DisplayName = "Slide"),
	CMOVE_WallRun	UMETA(DisplayName = "Wall Run"),
	CMOVE_Climb		UMETA(DisplayName = "Climb"),
	CMOVE_MAX		UMETA(HIdden)
};

UENUM(BlueprintType)
enum EVaultSelection
{
	VAULT_None				UMETA(Hidden),
	VAULT_Short				UMETA(DisplayName = "Short Vault"),
	VAULT_ShortCross		UMETA(DisplayName = "Short Cross Vault"),
	VAULT_Medium			UMETA(DisplayName = "Medium Vault"),
	VAULT_MediumCross		UMETA(DisplayName = "Medium Cross Vault"),
	VAULT_Tall				UMETA(DisplayName = "Tall Vault"),
	VAULT_TallCross			UMETA(DisplayName = "Tall Cross Vault"),
	VAULT_Max				UMETA(Hidden)
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

	// Initialize component variables
	virtual void InitializeComponent() override;

	virtual bool IsMovingOnGround() const override;
	virtual bool CanCrouchInCurrentState() const override;

	virtual float GetMaxSpeed() const override;
	virtual float GetMaxBrakingDeceleration() const override;

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	
	virtual bool CanAttemptJump() const override;
	virtual bool DoJump(bool bReplayingMoves) override;

	// Movement Functions to handle User Input START
	// JUMPING
	//UFUNCTION()
	//bool GetIsInAir() const;

	// JOG
	UFUNCTION(BlueprintCallable) bool GetIsJogging() const;
	
	// SPRINT
	UFUNCTION(BlueprintCallable) void SprintPressed();
	UFUNCTION(BlueprintCallable) void SprintReleased();
	// Get if the player wants to sprint
	UFUNCTION() FORCEINLINE bool GetWantsToSprint() const { return bWantsToSprint; }
	UFUNCTION(BlueprintCallable) bool GetIsSprinting() const;
	
	// CROUCH
    UFUNCTION(BlueprintCallable) void CrouchPressed();
    UFUNCTION(BlueprintCallable) void CrouchReleased();
	// Get if the player wants to crouch
	UFUNCTION() FORCEINLINE bool GetWantsToCrouch() const { return bWantsToCrouch; }
	
	// SLIDE (Included with crouch)
	UFUNCTION(BlueprintCallable) bool GetIsSliding() const;
	
	// WALK
	UFUNCTION(BlueprintCallable) void WalkPressed();
	UFUNCTION(BlueprintCallable) void WalkReleased();
	// Get if the player wants to sprint
	UFUNCTION() FORCEINLINE bool GetWantsToWalk() const { return bWantsToWalk; }
	UFUNCTION(BlueprintCallable) bool GetIsWalking() const;
	
	// DASH
	UFUNCTION(BlueprintCallable) void DashPressed();
	UFUNCTION(BlueprintCallable) void DashReleased();
	// Get if the player wants to dash
	UFUNCTION() FORCEINLINE bool GetWantsToDash() const { return bWantsToDash; }
	UFUNCTION(BlueprintCallable) bool GetIsDashing() const;

	// Get if the player is Climbing
	UFUNCTION(BlueprintPure)
	bool IsClimbing() const { return IsCustomMovementMode(CMOVE_Climb); }

	// WALL RUN
	// Get if the player is wall running
	UFUNCTION(BlueprintPure)
	bool IsWallRunning() const { return IsCustomMovementMode(CMOVE_WallRun); }
	UFUNCTION(BlueprintPure)
	bool WallRunningIsRight() const { return bWallRunIsRight; }

	// GRAB LEDGE
	UFUNCTION(BlueprintCallable) void GrabLedgePressed();
	UFUNCTION(BlueprintCallable) void GrabLedgeReleased();
	// Get if the player wants to grab ledge
	UFUNCTION() FORCEINLINE bool GetWantsToGrabLedge() const { return bWantsToGrabLedge; }
	UFUNCTION(BlueprintCallable) bool GetIsClimbing() const;
	UFUNCTION() void ClimbMove(float AxisValue)
	{
		MoveDirectionAxis = AxisValue;
	}

	float MoveDirectionAxis = 0.0f;
	
	// Movement Functions to handle User Input END

	bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;
	bool IsMovementMode(EMovementMode InMovementMode) const;
	
	// --- VARIABLES ---
	// Delegates
	// Dash Start Delegate
	UPROPERTY(BlueprintAssignable)
	FDashStartDelegate DashStartDelegate;
protected:
	// --- FUNCTIONS ---
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;

	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	
	virtual void PostLoad() override;
	virtual void BeginPlay() override;
	
	// --- VARIABLES ---
	UPROPERTY(Transient)
	TObjectPtr<ADreamCharacter> DreamCharacterOwner;
private:
	// --- FUNCTIONS ---
	// Dash Mechanic
	void OnDashCooldownFinished();
	bool CanDash() const;
	void PerformDash();

	// Slide Mechanic
	void EnterSlide(EMovementMode PrevMode, ECustomMovementMode PrevCustomMode);
	void ExitSlide();
	bool CanSlide() const;
	void PhysSlide(float DeltaTime, int32 Iterations);

	// Vault
	bool TryVault();
	FVector GetVaultStartLocation(const FHitResult& FrontHit, const FHitResult& SurfaceHit, const EVaultSelection& VaultSelection) const;

	// Wall Run
	bool TryWallRun();
	void PhysWallRun(float DeltaTime, int32 Iterations);

	// Climb
	bool TryClimb();
	bool TryClimbJump();
	void PhysClimbMove(float DeltaTime, int32 Iterations);

	// RootMotion Transition Functions
	void PlayRootMotionTransitionMontage(
		const FString& TransitionMontageName,
		const FVector& TransitionTarget,
		UAnimMontage* TransitionMontage,
		UAnimMontage* MainMontage,
		const ERootMotionAccumulateMode& RootMotionAccumulateMode = ERootMotionAccumulateMode::Override
	);
	
	// Helpers
	UFUNCTION() FORCEINLINE float CapsuleRadius() const
	{
		return DreamCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();
	}
	UFUNCTION() FORCEINLINE float CapsuleHalfHeight() const
	{
		return DreamCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	}

	// --- VARIABLES ---
	// Dash
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|Dash", meta = (AllowPrivateAccess = "true"))
	float DashImpulse = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|Dash", meta = (AllowPrivateAccess = "true"))
	float DashCooldownDuration = 1.f;

	// Slide
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|Slide", meta = (AllowPrivateAccess = "true"))
	float SlideMinStartSpeed = 601.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|Slide", meta = (AllowPrivateAccess = "true"))
	float SlideMinStopSpeed = 401.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|Slide", meta = (AllowPrivateAccess = "true"))
	float SlideEnterImpulse = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|Slide", meta = (AllowPrivateAccess = "true"))
	float SlideGravityForce = 5000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|Slide", meta = (AllowPrivateAccess = "true"))
	float SlideFrictionFactor = 0.06f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|Slide", meta = (AllowPrivateAccess = "true"))
	float SlideBrakingDeceleration = 1000.f;

	// Vault
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|Vault", meta = (AllowPrivateAccess = "true"))
	float CanVaultMaxDistance = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|Vault", meta = (AllowPrivateAccess = "true"))
	float VaultMaxDistance = 90.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|Vault", meta = (AllowPrivateAccess = "true"))
	float VaultReachHeight = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|Vault", meta = (AllowPrivateAccess = "true"))
	float MinVaultDepth = 30.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|Vault", meta = (AllowPrivateAccess = "true"))
	float TallVaultHeightFactor = 1.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|Vault", meta = (AllowPrivateAccess = "true"))
	float VaultMinWallSteepnessAngle = 75.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|Vault", meta = (AllowPrivateAccess = "true"))
	float VaultMaxSurfaceAngle = 15.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|Vault", meta = (AllowPrivateAccess = "true"))
	float VaultMaxAlignmentAngle = 45.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|Vault", meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> VaultAnimations;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|Vault", meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> VaultTransitionAnimations;
	
	// Wall Run
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|WallRun", meta = (AllowPrivateAccess = "true"))
	float WallRunMinSpeed = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|WallRun", meta = (AllowPrivateAccess = "true"))
	float WallRunMaxSpeed = 800.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|WallRun", meta = (AllowPrivateAccess = "true"))
	float WallRunMaxVerticalSpeed = 500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|WallRun", meta = (AllowPrivateAccess = "true"))
	float WallRunPullAwayAngle = 75.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|WallRun", meta = (AllowPrivateAccess = "true"))
	float WallAttractionForce = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|WallRun", meta = (AllowPrivateAccess = "true"))
	float WallRunMinHeight = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|WallRun", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* WallRunGravityScaleCurve;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|WallRun", meta = (AllowPrivateAccess = "true"))
	float WallJumpOffForce = 300.f;

	// Climb
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|Climb", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* TransitionClimbMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|Climb", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* WallJumpMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|Climb", meta = (AllowPrivateAccess = "true"))
	float ClimbJumpOffForce = 400.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|Climb", meta = (AllowPrivateAccess = "true"))
	float ClimbMaxSpeed = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|Climb", meta = (AllowPrivateAccess = "true"))
	float ClimbMaxHeight = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|Climb", meta = (AllowPrivateAccess = "true"))
	float ClimbReach = 100.f;
	
	// WantsTo Variables
	bool bWantsToSprint;
	bool bWantsToDash;
	bool bWantsToWalk;
	bool bWantsToGrabLedge = true;

	// Previous Value of wants to crouch
	bool bPrevWantsToCrouch;
	bool bPressedDreamJump;
	bool bHadAnimRootMotion;

	// Transition for Animations
	bool bTransitionFinished;
	TSharedPtr<FRootMotionSource_MoveToForce> TransitionRMS;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Dream|Movement|Transition", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* TransitionQueueMontage;
	FString TransitionName;
	float TransitionQueuedMontageSpeed;
	int TransitionRMS_ID;
	
	// RootMotion Transition Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|WallRun", meta = (AllowPrivateAccess = "true"))
	float TransitionDistanceMax = 500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|WallRun", meta = (AllowPrivateAccess = "true"))
	float TransitionDurationMin = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement|WallRun", meta = (AllowPrivateAccess = "true"))
	float TransitionDurationMax = 0.25f;

	
	
	// Is wall to the right of the character
	bool bWallRunIsRight;

	// Transient Variables (Counter)
	float DashStartTime;
	FTimerHandle TimerHandle_DashCooldown;

	// Speed while Standing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement", meta = (AllowPrivateAccess = "true"))
	float MaxSprintSpeed = 900.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement", meta = (AllowPrivateAccess = "true"))
	float MaxJogSpeed = 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement", meta = (AllowPrivateAccess = "true"))
	float Walk_MaxWalkSpeed = 300.f;

	// Speed while Crouched
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement", meta = (AllowPrivateAccess = "true"))
	float MaxJogSpeedCrouched = 400.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dream|Movement", meta = (AllowPrivateAccess = "true"))
	float Walk_MaxWalkSpeedCrouched = 200.f;
	
	// --- CLASSES ---
};
