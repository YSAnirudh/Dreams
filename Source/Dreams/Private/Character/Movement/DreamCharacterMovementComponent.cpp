// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Movement/DreamCharacterMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#if 1
float MacroDuration = 2.f;
#define LOG_SCREEN(Str) GEngine->AddOnScreenDebugMessage(-1, MacroDuration ? MacroDuration : -1.f, FColor::Yellow, Str);
#define LOG_TEMP(Str) UE_LOG(LogTemp, Warning, TEXT(Str));
#define VLOG_TEMP(Vector) UE_LOG(LogTemp, Warning, TEXT("X: %f, Y: %f, Z: %f"), Vector.X, Vector.Y, Vector.Z);
#define DEBUG_LINE(Start, End, Color)  DrawDebugLine(GetWorld(),Start,End,Color,!MacroDuration, MacroDuration, 0, 2);
#define DEBUG_POINT(Start, Color)  DrawDebugPoint(GetWorld(),Start,10,Color,!MacroDuration, MacroDuration);
#define DEBUG_CAPSULE(Start, Color)  DrawDebugCapsule(GetWorld(),Start,CapsuleHalfHeight(), CapsuleRadius(), FQuat::Identity,Color,!MacroDuration, MacroDuration);
#else
#define LOG_SCREEN(Str)
#define LOG_TEMP(Str)
#define VLOG_TEMP(Vector)
#define DEBUG_LINE(Start, End, Color)
#define DEBUG_POINT(Start, Color)
#define DEBUG_CAPSULE(Start, Color)
#endif

UDreamCharacterMovementComponent::UDreamCharacterMovementComponent()
{
	NavAgentProps.bCanCrouch = true;
	MaxWalkSpeedCrouched = Walk_MaxWalkSpeedCrouched;
	MaxWalkSpeed = Walk_MaxWalkSpeed;
}

void UDreamCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	DreamCharacterOwner = Cast<ADreamCharacter>(GetOwner());

}

void UDreamCharacterMovementComponent::PostLoad()
{
	Super::PostLoad();
}

void UDreamCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

#pragma region InputButtons

bool UDreamCharacterMovementComponent::GetIsJogging() const
{
	return DreamCharacterOwner && DreamCharacterOwner->bIsJogging;
}

void UDreamCharacterMovementComponent::SprintPressed()
{
	bWantsToSprint = true;
}

void UDreamCharacterMovementComponent::SprintReleased()
{
	bWantsToSprint = false;
}

bool UDreamCharacterMovementComponent::GetIsSprinting() const
{
	return DreamCharacterOwner && DreamCharacterOwner->bIsSprinting;
}

void UDreamCharacterMovementComponent::CrouchPressed()
{
	bWantsToCrouch = true;
}

void UDreamCharacterMovementComponent::CrouchReleased()
{
	bWantsToCrouch = false;
}

bool UDreamCharacterMovementComponent::GetIsSliding() const
{
	return DreamCharacterOwner->bIsSliding;
}

void UDreamCharacterMovementComponent::WalkPressed()
{
	bWantsToWalk = true;
}

bool UDreamCharacterMovementComponent::GetIsWalking() const
{
	return DreamCharacterOwner && DreamCharacterOwner->bIsWalking;
}

void UDreamCharacterMovementComponent::WalkReleased()
{
	bWantsToWalk = false;
}

void UDreamCharacterMovementComponent::DashPressed()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();

	// If Cooldown is over, set bWantsToDash
	if (CurrentTime - DashStartTime >= DashCooldownDuration)
	{
		bWantsToDash = true;
	}
	// If cooldown is not over, (Dash Hold) Start a timer to perform Dash after it is over
	else
	{
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle_DashCooldown,
			this,
			&UDreamCharacterMovementComponent::OnDashCooldownFinished,
			DashCooldownDuration - (CurrentTime - DashStartTime)
		);
	}
}

void UDreamCharacterMovementComponent::DashReleased()
{
	// Clear timer if any, If the player releases 
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_DashCooldown);
	bWantsToDash = false;
}

bool UDreamCharacterMovementComponent::GetIsDashing() const
{
	return false;
}

void UDreamCharacterMovementComponent::GrabLedgePressed()
{
	bWantsToGrabLedge = true;
}

void UDreamCharacterMovementComponent::GrabLedgeReleased()
{
	bWantsToGrabLedge = false;
}

bool UDreamCharacterMovementComponent::GetIsClimbing() const
{
	return IsMovementMode(MOVE_Custom) && IsCustomMovementMode(CMOVE_Climb);
}

bool UDreamCharacterMovementComponent::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

bool UDreamCharacterMovementComponent::IsMovementMode(EMovementMode InMovementMode) const
{
	return MovementMode == InMovementMode;
}

#pragma endregion 

#pragma region MovementUpdate
void UDreamCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	// SLIDE
	if (MovementMode == MOVE_Walking && bWantsToCrouch)
	{
		if (CanSlide())
		{
			SetMovementMode(MOVE_Custom, CMOVE_Slide);
			DreamCharacterOwner->bIsSprinting = false;
		}
	}
	if (IsCustomMovementMode(CMOVE_Slide) && !bWantsToCrouch)
	{
		SetMovementMode(MOVE_Walking);
	}

	// Vault
	if (DreamCharacterOwner->bPressedDreamJump)
	{
		if (TryVault())
		{
			DreamCharacterOwner->StopJumping();
		}
		else if (TryClimb())
		{
			DreamCharacterOwner->StopJumping();
		}
		else
		{
			//LOG_TEMP("Failed Vault");
			DreamCharacterOwner->bPressedDreamJump = false;
			CharacterOwner->bPressedJump = false;
			//CharacterOwner->CheckJumpInput(DeltaSeconds);
			bOrientRotationToMovement = true;
		}
	}

	if (GetIsClimbing() && !bWantsToGrabLedge)
	{
		SetMovementMode(MOVE_Falling);
	}

	if (GetIsClimbing())
	{
		
	}

	if (bTransitionFinished)
	{
		if (TransitionName == "Vault")
		{
			if (IsValid(TransitionQueueMontage))
			{
				SetMovementMode(MOVE_Flying);
				CharacterOwner->PlayAnimMontage(TransitionQueueMontage, TransitionQueuedMontageSpeed);
				TransitionQueuedMontageSpeed = 0.f;
				TransitionQueueMontage = nullptr;
			}
			else
			{
				SetMovementMode(MOVE_Walking);
			}
		}
		else if (TransitionName == "Climb")
		{
			SetMovementMode(MOVE_Custom, CMOVE_Climb);
			Velocity = FVector::ZeroVector;
		}
		TransitionName = "";
		bTransitionFinished = false;
	}

	// WALL RUN
	if (IsFalling())
	{
		TryWallRun();
	}
	
	// DASH
	if (bWantsToDash && CanDash())
	{
		PerformDash();
		bWantsToDash = false;
	}
	
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

void UDreamCharacterMovementComponent::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateAfterMovement(DeltaSeconds);

	if (!HasAnimRootMotion() && bHadAnimRootMotion && IsMovementMode(MOVE_Flying))
	{
		LOG_TEMP("Ending Root Motion Anim")
		SetMovementMode(MOVE_Walking);
	}
	if (GetRootMotionSourceByID(TransitionRMS_ID) && GetRootMotionSourceByID(TransitionRMS_ID)->Status.HasFlag(ERootMotionSourceStatusFlags::Finished))
	{
		RemoveRootMotionSourceByID(TransitionRMS_ID);
		bTransitionFinished = true;
	}

	bHadAnimRootMotion = HasAnimRootMotion();
}

void UDreamCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
                                                         const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	switch (MovementMode)
	{
	case MOVE_Walking:
		if (FMath::IsNearlyZero(Velocity.Size()))
		{
			if (DreamCharacterOwner)
			{
				DreamCharacterOwner->bIsSprinting = false;
				DreamCharacterOwner->bIsWalking = false;
				DreamCharacterOwner->bIsJogging = false;
			}
			break;
		}

		if (IsFalling())
		{
			break;
		}
		
		if (GetIsDashing())
		{
			break;
		}

		if (GetIsSliding())
		{
			break;
		}

		if (!DreamCharacterOwner)
		{
			break;
		}

		DreamCharacterOwner->bIsSprinting = bWantsToSprint && !IsCrouching();
		DreamCharacterOwner->bIsWalking = bWantsToWalk && !GetIsSprinting();
		DreamCharacterOwner->bIsJogging = !GetIsSprinting() && !GetIsWalking();
		
		break;
	case MOVE_Custom:
		break;
	default:
		break;
	}
}

void UDreamCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode,
	uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	//LOG_TEMP("Movement Changed")
	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == CMOVE_Slide)
	{
		ExitSlide();
	}

	if (IsCustomMovementMode(CMOVE_Slide))
	{
		EnterSlide(MOVE_Custom, CMOVE_Slide);
	}
}

void UDreamCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	switch (CustomMovementMode)
	{
	case CMOVE_Slide:
		PhysSlide(deltaTime, Iterations);
		break;
	case CMOVE_WallRun:
		PhysWallRun(deltaTime, Iterations);
		break;
	case CMOVE_Climb:
		PhysClimbMove(deltaTime, Iterations);
		break;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"));
	}
}
#pragma endregion 

#pragma region DefaultMovement
bool UDreamCharacterMovementComponent::IsMovingOnGround() const
{
	return Super::IsMovingOnGround() || IsCustomMovementMode(CMOVE_Slide);
}

bool UDreamCharacterMovementComponent::CanCrouchInCurrentState() const
{
	return Super::CanCrouchInCurrentState() && IsMovingOnGround();
}

float UDreamCharacterMovementComponent::GetMaxSpeed() const
{
	if (IsMovementMode(MOVE_Walking) && GetIsJogging() && IsCrouching())
	{
		return MaxJogSpeedCrouched;
	}
	if (IsMovementMode(MOVE_Walking) && GetIsWalking() && IsCrouching())
	{
		return MaxWalkSpeedCrouched;
	}
	if (IsMovementMode(MOVE_Walking) && GetIsJogging())
	{
		return MaxJogSpeed;
	}
	if (IsMovementMode(MOVE_Walking) && GetIsWalking())
	{
		return MaxWalkSpeed;
	}
	if (IsMovementMode(MOVE_Walking) && GetIsSprinting())
	{
		return MaxSprintSpeed;
	}

	if (MovementMode != MOVE_Custom)
	{
		return Super::GetMaxSpeed();
	}

	switch (CustomMovementMode)
	{
	case CMOVE_Slide:
		return SlideMinStartSpeed;
	case CMOVE_WallRun:
		return WallRunMaxSpeed;
	case CMOVE_Climb:
		return ClimbMaxSpeed;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"));
		return -1.f;
	}
	
}

float UDreamCharacterMovementComponent::GetMaxBrakingDeceleration() const
{
	if (MovementMode != MOVE_Custom)
	{
		return Super::GetMaxBrakingDeceleration();
	}
	
	switch (CustomMovementMode)
	{
	case CMOVE_Slide:
		return SlideBrakingDeceleration;
	case CMOVE_WallRun:
		return 0.0f;
	case CMOVE_Climb:
		return 500.f;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"));
		return -1.f;
	}
}

bool UDreamCharacterMovementComponent::CanAttemptJump() const
{
	return Super::CanAttemptJump() || IsWallRunning() || IsClimbing();
}

bool UDreamCharacterMovementComponent::DoJump(bool bReplayingMoves)
{
	const bool bWasWallRunning = IsWallRunning();
	const bool bWasOnWall = IsClimbing();

	if (Super::DoJump(bReplayingMoves))
	{
		if (bWasWallRunning)
		{
			UE_LOG(LogTemp, Warning, TEXT("Jump"))	
			FVector Start = UpdatedComponent->GetComponentLocation();
			FVector CastDelta = UpdatedComponent->GetRightVector() * CapsuleRadius() * 2;
			FVector End = bWallRunIsRight ? Start + CastDelta : Start - CastDelta;

			FHitResult WallHit;
			GetWorld()->LineTraceSingleByProfile(
				WallHit,
				Start,
				End,
				TEXT("BlockAll"),
				DreamCharacterOwner->GetIgnoreCharacterParams()
			);
			Velocity += WallHit.Normal * WallJumpOffForce;
		}
		else if (bWasOnWall)
		{
			if (!bReplayingMoves)
			{
				//CharacterOwner->PlayAnimMontage(WallJumpMontage);
			}
			//Velocity += FVector::UpVector * ClimbJumpOffForce * .5f;
			//Velocity += Acceleration.GetSafeNormal2D() * ClimbJumpOffForce * .5f;
			
		}
		return true;
	}
	return false;
}
#pragma endregion

#pragma region Dash
void UDreamCharacterMovementComponent::OnDashCooldownFinished()
{
	bWantsToDash = true;
}

bool UDreamCharacterMovementComponent::CanDash() const
{
	return IsWalking() && !IsCrouching();
}

void UDreamCharacterMovementComponent::PerformDash()
{
	DashStartTime = GetWorld()->GetTimeSeconds();

	FVector DashDirection = (Acceleration.IsNearlyZero() ? UpdatedComponent->GetForwardVector() : Acceleration).GetSafeNormal2D();
	// TWEAK TWEAK
	Velocity = DashImpulse * (DashDirection + FVector::UpVector * .1f);

	FQuat NewRotation = FRotationMatrix::MakeFromXZ(DashDirection, FVector::UpVector).ToQuat();
	FHitResult Hit;
	SafeMoveUpdatedComponent(FVector::ZeroVector, NewRotation, false, Hit);

	SetMovementMode(MOVE_Falling);

	DashStartDelegate.Broadcast();
}
#pragma endregion 

#pragma region Slide
void UDreamCharacterMovementComponent::EnterSlide(EMovementMode PrevMode, ECustomMovementMode PrevCustomMode)
{
	DreamCharacterOwner->bIsSliding = true;
	bWantsToCrouch = true;
	
	Velocity += Velocity.GetSafeNormal2D() * SlideEnterImpulse;

	FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, true, nullptr);
}

void UDreamCharacterMovementComponent::ExitSlide()
{
	DreamCharacterOwner->bIsSliding = false;
}

bool UDreamCharacterMovementComponent::CanSlide() const
{
	const FVector Start = UpdatedComponent->GetComponentLocation();
	const FVector End = Start + CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.5f * FVector::DownVector;
	const FName ProfileName = TEXT("BlockAll");
	const bool bValidSurface = GetWorld()->LineTraceTestByProfile(Start, End, ProfileName, DreamCharacterOwner->GetIgnoreCharacterParams());
	const bool bEnoughSpeed = Velocity.SizeSquared() > pow(SlideMinStartSpeed, 2);
	
	return bValidSurface && bEnoughSpeed;
}

void UDreamCharacterMovementComponent::PhysSlide(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}

	RestorePreAdditiveRootMotionVelocity();

	// Make sure there is a surface to slide on, otherwise exit slide
	if (!CanSlide())
	{
		SetMovementMode(MOVE_Walking);
		StartNewPhysics(DeltaTime, Iterations);
		return;
	}

	// Add a force in the downward direction. This is on slopes. Give an extra boost for sliding.
	Velocity += SlideGravityForce * FVector::DownVector * DeltaTime;

	// Check if the acceleration is enough to strafe.
	// Acceleration is modified Input Vector
	if (FMath::Abs(FVector::DotProduct(Acceleration.GetSafeNormal(), UpdatedComponent->GetRightVector())) > 0.5f)
	{
		Acceleration = Acceleration.ProjectOnTo(UpdatedComponent->GetRightVector());
	}
	else
	{
		Acceleration = FVector::ZeroVector;
	}

	// If there is no root motion then calc velocity using friction
	// Applying acceleration to velocity based on friction
	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		CalcVelocity(DeltaTime, GroundFriction * SlideFrictionFactor, false, GetMaxBrakingDeceleration());
	}

	// If there is root motion
	ApplyRootMotionToVelocity(DeltaTime);

	// Performing the move
	Iterations++;
	bJustTeleported = false;

	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	FQuat OldRotation = UpdatedComponent->GetComponentRotation().Quaternion();

	// Hit for checking collisions while sliding
	FHitResult Hit(1.f);

	FHitResult SurfaceHit;
	const FVector Start = UpdatedComponent->GetComponentLocation();
	const FVector End = Start + CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.5f * FVector::DownVector;
	const FName ProfileName = TEXT("BlockAll");
	GetWorld()->LineTraceSingleByProfile(
		SurfaceHit, Start, End, ProfileName, DreamCharacterOwner->GetIgnoreCharacterParams()
	);

	// Update position using velocity and delta time
	FVector AdjustedPosition = Velocity * DeltaTime;
	// Get the direction of the slide.
	FVector VelPlaneDir = FVector::VectorPlaneProject(Velocity, SurfaceHit.Normal).GetSafeNormal();
	// Rotation of the character while sliding
	FQuat NewRotation = FRotationMatrix::MakeFromXZ(VelPlaneDir, SurfaceHit.Normal).ToQuat();

	// Move checking for collisions. bSweep is true to make sure the character collides with the wall
	// rather than going into the wall as Adjusted position might do so
	SafeMoveUpdatedComponent(AdjustedPosition, NewRotation, true, Hit);

	// If there is a collision, Handle the impact,
	// and make sure the character slides along any extra surface rather that coming to an abrupt stop
	if (Hit.Time < 1.f)
	{
		HandleImpact(Hit, DeltaTime, AdjustedPosition);
		SlideAlongSurface(AdjustedPosition, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}

	// Check for Slide Surface and Exit if there is no slide surface or the velocity is low
	FHitResult NewSurfaceHit;
	if (!CanSlide())
	{
		ExitSlide();
	}

	// We set the velocity again here, because when the character collides with a surface during a sweep
	// the final velocity should actually be lower than the velocity before collision
	if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / DeltaTime;
	}
}
#pragma endregion

#pragma region Vault
bool UDreamCharacterMovementComponent::TryVault()
{
	//LOG_SCREEN("Tried Vault");

	if (!(IsMovementMode(MOVE_Walking) && !IsCrouching()))// && !IsMovementMode(MOVE_Falling))
	{
		return false;
	}

	FVector BaseLoc = UpdatedComponent->GetComponentLocation() + FVector::DownVector * CapsuleHalfHeight();
	FVector Forward = UpdatedComponent->GetForwardVector().GetSafeNormal2D();

	auto Params = DreamCharacterOwner->GetIgnoreCharacterParams();

	float MaxHeight = (CapsuleHalfHeight() * 2) - 1.f;
	float CosMinWallSteepnessAngle = FMath::DegreesToRadians(VaultMinWallSteepnessAngle);
	float CosMaxSurfaceAngle = FMath::DegreesToRadians(VaultMaxSurfaceAngle);
	float CosMaxAlignmentAngle = FMath::DegreesToRadians(VaultMaxAlignmentAngle);

	FHitResult FrontHit;

	float CheckDistance = FMath::Clamp(FVector::DotProduct(Velocity, Forward), CapsuleRadius() + 30.f, CanVaultMaxDistance);
	FVector FrontStart = BaseLoc + FVector::UpVector * (MaxStepHeight - 1);

	for (int i = 0; i < 6; i++)
	{
		//DEBUG_LINE(FrontStart, FrontStart + (Forward * CheckDistance), FColor::Red);
		if (GetWorld()->LineTraceSingleByProfile(
			FrontHit,
			FrontStart,
			FrontStart + (Forward * CheckDistance),
			TEXT("BlockAll"),
			Params
		)){
			break;
		}
		FrontStart += FVector::UpVector * (2.f * CapsuleHalfHeight() - (MaxStepHeight - 1)) / 5;
	}

	if (!FrontHit.IsValidBlockingHit())
	{
		return false;
	}

	float CosWallSteepnessAngle = FVector::DotProduct(FrontHit.Normal, FVector::UpVector);
	if (FMath::Abs(CosWallSteepnessAngle) > CosMinWallSteepnessAngle || FVector::DotProduct(Forward, -FrontHit.Normal) < CosMaxAlignmentAngle)
	{
		return false;
	}

	//DEBUG_POINT(FrontHit.Location, FColor::Red);

	TArray<FHitResult> HeightHits;
	FHitResult SurfaceHit;

	FVector WallUp = FVector::VectorPlaneProject(FVector::UpVector, FrontHit.Normal).GetSafeNormal();
	float WallCos = FVector::DotProduct(FVector::UpVector, FrontHit.Normal);
	float WallSin = FMath::Sqrt(1 - WallCos * WallCos);

	FVector TraceStart = FrontHit.Location + Forward + WallUp * (MaxHeight - (MaxStepHeight - 1)) / WallSin;
	if (!GetWorld()->LineTraceMultiByProfile(
		HeightHits,
		TraceStart,
		FrontHit.Location + Forward,
		TEXT("BlockAll"),
		Params
	))
	{
		return false;
	}

	for (const FHitResult& Hit : HeightHits)
	{
		if (Hit.IsValidBlockingHit())
		{
			SurfaceHit = Hit;
			break;
		}
	}

	if (!SurfaceHit.IsValidBlockingHit() || (FVector::DotProduct(SurfaceHit.Normal, FVector::UpVector) < CosMaxSurfaceAngle))
	{
		return false;
	}

	float Height = FVector::DotProduct(SurfaceHit.Location - BaseLoc, FVector::UpVector);

	//DEBUG_POINT(SurfaceHit.Location, FColor::Red)
	if (Height > MaxHeight)
	{
		return false;
	}

	float SurfaceCos = FVector::DotProduct(FVector::UpVector, SurfaceHit.Normal);
	float SurfaceSin = FMath::Sqrt(1 - SurfaceCos * SurfaceCos);

	FVector ClearanceCapsuleLocation = SurfaceHit.Location + Forward * CapsuleRadius() + FVector::UpVector * (CapsuleHalfHeight() + 1 + CapsuleRadius() * 2 * SurfaceSin);
	FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(CapsuleRadius(), CapsuleHalfHeight());
	if (GetWorld()->OverlapAnyTestByProfile(
		ClearanceCapsuleLocation,
		FQuat::Identity,
		TEXT("BlockALl"),
		CapsuleShape,
		Params
	))
	{
		//DEBUG_CAPSULE(ClearanceCapsuleLocation, FColor::Red);
		return false;
	}
	else
	{
		//DEBUG_CAPSULE(ClearanceCapsuleLocation, FColor::Green);
	}

	EVaultSelection VaultSelection;

	bool bVaultCross = false;
	FHitResult VaultCheckHit;
	FVector VaultSurfaceVector = FVector::VectorPlaneProject(UpdatedComponent->GetForwardVector(), SurfaceHit.Normal);
	VaultSurfaceVector = VaultSurfaceVector.ProjectOnTo(-FrontHit.Normal).GetSafeNormal();
	FVector VaultCheckStart = SurfaceHit.Location + (VaultSurfaceVector * VaultMaxDistance) + FVector::DownVector * 2.f;
	FVector VaultCheckEnd = SurfaceHit.Location + FVector::DownVector * 2.f;
	GetWorld()->LineTraceSingleByProfile(
		VaultCheckHit,
		VaultCheckStart,
		VaultCheckEnd,
		TEXT("BlockAll"),
		Params
	);
	if (VaultCheckHit.IsValidBlockingHit())
	{
		bVaultCross = true;
	}

	//DEBUG_LINE(VaultCheckStart, SurfaceHit.Location, FColor::Red);

	if (IsMovementMode(MOVE_Walking))
	{
		if (Height > CapsuleHalfHeight() * TallVaultHeightFactor)
		{
			VaultSelection = bVaultCross ? EVaultSelection::VAULT_TallCross : EVaultSelection::VAULT_Tall;
		}
		else if (Height <= CapsuleHalfHeight() * TallVaultHeightFactor && Height > ((MaxStepHeight-1) + (CapsuleHalfHeight() * TallVaultHeightFactor)) * .5f)
		{
			VaultSelection = bVaultCross ? EVaultSelection::VAULT_MediumCross : EVaultSelection::VAULT_Medium;
		}
		else
		{
			VaultSelection = bVaultCross ? EVaultSelection::VAULT_ShortCross : EVaultSelection::VAULT_Short;
		}
	}
	
	FVector TransitionTarget = GetVaultStartLocation(FrontHit, SurfaceHit, VaultSelection);

	// else if (IsMovementMode(MOVE_Falling) && FVector::DotProduct(Velocity, FVector::UpVector) < 0)
	// {
	// 	if (!GetWorld()->OverlapAnyTestByProfile(
	// 		TallVaultTarget,
	// 		FQuat::Identity,
	// 		TEXT("BlockALl"),
	// 		CapsuleShape,
	// 		Params
	// 	))
	// 	{
	// 		bTallVault = true;
	// 	}
	// }

	for (int i = 0; i < 6; i++)
	{
		if (VaultTransitionAnimations.Num() != 3 || VaultAnimations.Num() != 6)
		{
			FHitResult SweepHit;
			SafeMoveUpdatedComponent(ClearanceCapsuleLocation - UpdatedComponent->GetComponentLocation(),
				UpdatedComponent->GetComponentQuat(), false, SweepHit);
			SetMovementMode(MOVE_Walking);
			break;
		}
		if (VaultSelection == i+1)
		{
			if (VaultTransitionAnimations[i / 2] && VaultAnimations[i])
			{
				PlayRootMotionTransitionMontage(TEXT("Vault"), TransitionTarget, VaultTransitionAnimations[i / 2], VaultAnimations[i]);
			}
		}
	}
	
	return true;
}

FVector UDreamCharacterMovementComponent::GetVaultStartLocation(const FHitResult& FrontHit, const FHitResult& SurfaceHit,
	const EVaultSelection& VaultSelection) const
{
	const float CosWallSteepnessAngle = FVector::DotProduct(FrontHit.Normal, FVector::UpVector);

	float DownDistance = 0.0f; //bTallVault ? CapsuleHalfHeight() * 2.f : MaxStepHeight - 1;
	switch (VaultSelection)
	{
	case VAULT_Short:
		LOG_TEMP("VAULT_Short")
		DownDistance = MaxStepHeight-1;
		break;
	case VAULT_ShortCross:
		LOG_TEMP("VAULT_ShortCross")
		DownDistance = MaxStepHeight-1;
		break;
	case VAULT_Medium:
		LOG_TEMP("VAULT_Medium")
		DownDistance = ((MaxStepHeight-1) + (CapsuleHalfHeight() * TallVaultHeightFactor)) * .5f;
		break;
	case VAULT_MediumCross:
		LOG_TEMP("VAULT_MediumCross")
		DownDistance = ((MaxStepHeight-1) + (CapsuleHalfHeight() * TallVaultHeightFactor)) * .5f;
		break;
	case VAULT_Tall:
		LOG_TEMP("VAULT_Tall")
		DownDistance = CapsuleHalfHeight() * TallVaultHeightFactor;
		break;
	case VAULT_TallCross:
		LOG_TEMP("VAULT_TallCross")
		DownDistance = CapsuleHalfHeight() * TallVaultHeightFactor;
		break;
	default:
		break;
	}

	const FVector EdgeTangent = FVector::CrossProduct(SurfaceHit.Normal, FrontHit.Normal).GetSafeNormal();
	FVector VaultStart = SurfaceHit.Location;

	VaultStart += FrontHit.Normal.GetSafeNormal2D() * (2.f + CapsuleRadius());
	VaultStart += UpdatedComponent->GetForwardVector().GetSafeNormal2D().ProjectOnTo(EdgeTangent) * CapsuleRadius() * .1f;
	VaultStart += FVector::UpVector * CapsuleHalfHeight();
	VaultStart += FVector::DownVector * DownDistance;
	VaultStart += FrontHit.Normal.GetSafeNormal2D() * CosWallSteepnessAngle * DownDistance;

	return VaultStart;
}
#pragma endregion

#pragma region WallRun
bool UDreamCharacterMovementComponent::TryWallRun()
{
	if (!IsFalling())
	{
		return false;
	}
	if (Velocity.SizeSquared2D() < WallRunMinSpeed * WallRunMinSpeed)
	{
		return false;
	}
	if (Velocity.Z < -WallRunMaxVerticalSpeed)
	{
		return false;
	}

	const FVector Start = UpdatedComponent->GetComponentLocation();
	const FVector LeftEnd = Start - UpdatedComponent->GetRightVector() * CapsuleRadius() * 2;
	const FVector RightEnd = Start + UpdatedComponent->GetRightVector() * CapsuleRadius() * 2;

	const auto Params = DreamCharacterOwner->GetIgnoreCharacterParams();

	FHitResult FloorHit, WallHit;
	GetWorld()->LineTraceSingleByProfile(
		FloorHit,
		Start,
		Start + FVector::DownVector * (CapsuleHalfHeight() + WallRunMinHeight),
		TEXT("BlockAll"),
		Params
	);
	//DEBUG_LINE(Start, Start + FVector::DownVector * (CapsuleHalfHeight() + WallRunMinHeight), FColor::Red)
	if (FloorHit.IsValidBlockingHit())
	{
		return false;
	}

	GetWorld()->LineTraceSingleByProfile(WallHit, Start, LeftEnd, TEXT("BlockAll"), Params);
	if (WallHit.IsValidBlockingHit() && FVector::DotProduct(Velocity, WallHit.Normal) < 0)
	{
		bWallRunIsRight = false;
	}
	else
	{
		GetWorld()->LineTraceSingleByProfile(WallHit, Start, RightEnd, TEXT("BlockAll"), Params);
		if (WallHit.IsValidBlockingHit() && FVector::DotProduct(Velocity, WallHit.Normal) < 0)
		{
			bWallRunIsRight = true;
		}
		else
		{
			//LOG_TEMP("Fall")
			return false;
		}
	}

	FVector ProjectedVelocity = FVector:: VectorPlaneProject(Velocity, WallHit.Normal);
	if (ProjectedVelocity.SizeSquared2D() < WallRunMinSpeed * WallRunMinSpeed)
	{
		return false;
	}

	Velocity = ProjectedVelocity;
	Velocity.Z = WallRunMaxVerticalSpeed;
	Velocity.Z = FMath::Clamp(Velocity.Z, 0.0f, WallRunMaxVerticalSpeed);
	//VLOG_TEMP(Velocity);
	SetMovementMode(MOVE_Custom, CMOVE_WallRun);
	return true;
}

void UDreamCharacterMovementComponent::PhysWallRun(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}
	if (!CharacterOwner || (!CharacterOwner->Controller && bRunPhysicsWithNoController && !HasAnimRootMotion() && CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}

	bJustTeleported = false;
	float remainingTime = DeltaTime;

	while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)))
	{
		Iterations++;
		bJustTeleported = false;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;
		
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();

		FVector Start = UpdatedComponent->GetComponentLocation();
		FVector CastDelta = UpdatedComponent->GetRightVector() * CapsuleRadius() * 2;
		FVector End = bWallRunIsRight ? Start + CastDelta : Start - CastDelta;
		auto Params = DreamCharacterOwner->GetIgnoreCharacterParams();
		float SinPullAwayAngle = FMath::Sin(FMath::DegreesToRadians(WallRunPullAwayAngle));
		FHitResult WallHit;
		GetWorld()->LineTraceSingleByProfile(WallHit, Start, End, "BlockAll", Params);
		bool bWantsToPullAway = WallHit.IsValidBlockingHit() && !Acceleration.IsNearlyZero() && (Acceleration.GetSafeNormal() | WallHit.Normal) > SinPullAwayAngle;
		if (!WallHit.IsValidBlockingHit() || bWantsToPullAway)
		{
			LOG_TEMP("1")
			SetMovementMode(MOVE_Falling);
			StartNewPhysics(remainingTime, Iterations);
			return;
		}

		Acceleration = FVector::VectorPlaneProject(Acceleration, WallHit.Normal);
		//Acceleration.Z = 0.0f;

		CalcVelocity(timeTick, 0.1f, false, GetMaxBrakingDeceleration());

		Velocity = FVector::VectorPlaneProject(Velocity, WallHit.Normal);
		//VLOG_TEMP(Velocity)
		float TangentAcceleration = FVector::DotProduct(Acceleration.GetSafeNormal(), Velocity.GetSafeNormal2D());
		bool bVelocityUp = Velocity.Z > 0.0f;
		//UE_LOG(LogTemp, Warning, TEXT("1 %f %f"), Velocity.Z, TangentAcceleration);
		//UE_LOG(LogTemp, Warning, TEXT("%f"), GetGravityZ() * WallRunGravityScaleCurve->GetFloatValue(bVelocityUp ? 0.f : TangentAcceleration) * timeTick);
		//UE_LOG(LogTemp, Warning, TEXT("Tick: %f"), timeTick);
		//UE_LOG(LogTemp, Warning, TEXT("Gravity: %f"), GetGravityZ() * timeTick);
		//UE_LOG(LogTemp, Warning, TEXT("Scale: %f"), WallRunGravityScaleCurve->GetFloatValue(bVelocityUp ? 0.f : TangentAcceleration));
		//LOG_TEMP("-------------")
		Velocity.Z += GetGravityZ() * WallRunGravityScaleCurve->GetFloatValue(bVelocityUp ? 0.f : TangentAcceleration) * timeTick;
		//UE_LOG(LogTemp, Warning, TEXT("2 %f %f"), Velocity.Z, TangentAcceleration);
		//VLOG_TEMP(Velocity)
		//VLOG_TEMP(Acceleration)
		if (Velocity.SizeSquared2D() < WallRunMinSpeed * WallRunMinSpeed || Velocity.Z < -WallRunMaxVerticalSpeed)
		{
			LOG_TEMP("2")
			SetMovementMode(MOVE_Falling);
			StartNewPhysics(remainingTime, Iterations);
			return;
		}

		const FVector Delta = timeTick * Velocity;
		const bool bZeroDelta = Delta.IsNearlyZero();
		if (bZeroDelta)
		{
			remainingTime = 0.0f;
		}
		else
		{
			FHitResult Hit;
			SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);
			FVector WallAttractionDelta = -WallHit.Normal * WallAttractionForce * timeTick;
			SafeMoveUpdatedComponent(WallAttractionDelta, UpdatedComponent->GetComponentQuat(), true, Hit);
			if (UpdatedComponent->GetComponentLocation() == OldLocation)
			{
				remainingTime = 0.0f;
				break;
			}
			Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick;
		}
	}
	//VLOG_TEMP(Velocity)
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector CastDelta = UpdatedComponent->GetRightVector() * CapsuleRadius() * 2;
	FVector End = bWallRunIsRight ? Start + CastDelta : Start - CastDelta;
	auto Params = DreamCharacterOwner->GetIgnoreCharacterParams();
	FHitResult WallHit, FloorHit;
	GetWorld()->LineTraceSingleByProfile(WallHit, Start, End, "BlockAll", Params);
	GetWorld()->LineTraceSingleByProfile(
		FloorHit,
		Start,
		Start + FVector::DownVector * (CapsuleHalfHeight() + (WallRunMinHeight * .5f)),
		TEXT("BlockAll"),
		Params
	);
	if (FloorHit.IsValidBlockingHit() || !WallHit.IsValidBlockingHit() || Velocity.SizeSquared2D() < WallRunMinSpeed * WallRunMinSpeed)
	{
		LOG_TEMP("3")
		if (FloorHit.IsValidBlockingHit())
		{
			LOG_TEMP("4")
		}
		SetMovementMode(MOVE_Falling);
		return;
	}
}
#pragma endregion

#pragma region Climb
bool UDreamCharacterMovementComponent::TryClimb()
{
	if (!IsMovementMode(MOVE_Walking) && !IsClimbing())
	{
		return false;
	}
	
	FVector BaseLoc = UpdatedComponent->GetComponentLocation() + FVector::DownVector * CapsuleHalfHeight();
	FVector Forward = UpdatedComponent->GetForwardVector().GetSafeNormal2D();

	auto Params = DreamCharacterOwner->GetIgnoreCharacterParams();

	float MaxHeight = (CapsuleHalfHeight() * 4) - 1.f;
	FHitResult WallHit;
	
	float CheckDistance = FMath::Clamp(FVector::DotProduct(Velocity, Forward), CapsuleRadius() + 30.f, CanVaultMaxDistance);
	FVector StartWallTrace = BaseLoc + FVector::UpVector * (MaxStepHeight - 1);
	
	//DEBUG_LINE(StartWallTrace, StartWallTrace + Forward * CheckDistance, FColor::Red)
	if (!GetWorld()->LineTraceSingleByProfile(
		WallHit,
		StartWallTrace,
		StartWallTrace + Forward * CheckDistance,
		TEXT("BlockAll"),
		DreamCharacterOwner->GetIgnoreCharacterParams()
	))
	{
		return false;
	}

	bool bIsAttached = false;
	float LeftRightAxis = 0.0f;
	bool bDetectedLedge = false;
	float LedgeHeightLocation = 0.0f;
	TArray<FHitResult> HeightHits;
	FHitResult SurfaceHit;

	FVector WallUp = FVector::VectorPlaneProject(FVector::UpVector, WallHit.Normal).GetSafeNormal();
	float WallCos = FVector::DotProduct(FVector::UpVector, WallHit.Normal);
	float WallSin = FMath::Sqrt(1 - WallCos * WallCos);

	ClimbMaxHeight = CapsuleHalfHeight() * 4.f;
	FVector TraceStart = WallHit.Location + Forward + WallUp * (MaxHeight - (MaxStepHeight - 1)) / WallSin;

	//DEBUG_LINE(TraceStart, WallHit.Location, FColor::Red);
	if (!GetWorld()->LineTraceMultiByProfile(
		HeightHits,
		TraceStart,
		WallHit.Location + Forward,
		TEXT("BlockAll"),
		Params
	))
	{
		return false;
	}


	for (const FHitResult& Hit : HeightHits)
	{
		if (Hit.IsValidBlockingHit())
		{
			SurfaceHit = Hit;
			break;
		}
	}
	if (!SurfaceHit.IsValidBlockingHit())
	{
		DEBUG_POINT(SurfaceHit.Location, FColor::Red);
		return false;
	}

	float HeightUp = (SurfaceHit.Location.Z - WallHit.Location.Z);
	DEBUG_POINT((WallHit.Location + WallUp * HeightUp), FColor::Green);

	FVector TargetLocation = (WallHit.Location + WallUp * HeightUp) + WallHit.Normal * CapsuleRadius() * 1.02f + FVector::DownVector * CapsuleHalfHeight();
	FQuat TargetRotation = FRotationMatrix::MakeFromXZ(-WallHit.Normal, FVector::UpVector).ToQuat();
	
	FTransform CurrentTransform = UpdatedComponent->GetComponentTransform();
	FHitResult Hit, ReturnHit;
	SafeMoveUpdatedComponent(TargetLocation - UpdatedComponent->GetComponentLocation(), UpdatedComponent->GetComponentQuat(), true, Hit);

	FVector ResultLocation = UpdatedComponent->GetComponentLocation();
	SafeMoveUpdatedComponent(CurrentTransform.GetLocation() - ResultLocation, TargetRotation, false, ReturnHit);
	if (!ResultLocation.Equals(TargetLocation))
	{
		return false;
	}
	
	bOrientRotationToMovement = false;
	bUseControllerDesiredRotation = false;

	if (!TransitionClimbMontage)
	{
		SafeMoveUpdatedComponent(TargetLocation - UpdatedComponent->GetComponentLocation(), UpdatedComponent->GetComponentQuat(), true, Hit);
		SetMovementMode(MOVE_Custom, CMOVE_Climb);
		return true;
	}

	PlayRootMotionTransitionMontage(TEXT("Climb"), TargetLocation, TransitionClimbMontage, nullptr);

	return true;
}

bool UDreamCharacterMovementComponent::TryClimbJump()
{
	if (!IsClimbing())
	{
		return false;
	}

	FVector ClimbStart;
	bool bReachRight = false;
	bool bTryCanReach = false;

	if (DreamCharacterOwner && DreamCharacterOwner->GetFollowCamera())
	{
		float CameraTurn = FVector::DotProduct(DreamCharacterOwner->GetFollowCamera()->GetForwardVector(), UpdatedComponent->GetRightVector());
		if (CameraTurn > 0.5f)
		{
			bReachRight = true;
			bTryCanReach = true;
		}
		else if (CameraTurn < -0.5f)
		{
			bReachRight = false;
			bTryCanReach = true;
		}
		else
		{
			bTryCanReach = false;
		}
	}

	if (!bTryCanReach)
	{
		return false;
	}

	FHitResult AlongWallTrace;
	FVector ClimbPoint = UpdatedComponent->GetComponentLocation() + UpdatedComponent->GetForwardVector() * (1.f + (CapsuleRadius() * 1.02f)) + FVector::UpVector * (CapsuleHalfHeight());

	//DEBUG_POINT(ClimbPoint, FColor::Red);
		
	FVector StartClimbPoint = ClimbPoint + FVector::UpVector * 5.f;
	FVector EndClimbPoint = ClimbPoint - FVector::UpVector * 5.f;

	GetWorld()->LineTraceSingleByProfile(
		AlongWallTrace,
		StartClimbPoint,
		EndClimbPoint,
		TEXT("BlockAll"),
		DreamCharacterOwner->GetIgnoreCharacterParams()
	);
	if (!AlongWallTrace.IsValidBlockingHit())
	{
		return false;
	}

	FVector StartClimbReachPoint = UpdatedComponent->GetComponentLocation() + 
			UpdatedComponent->GetForwardVector() * CapsuleRadius() +
			FVector::UpVector * CapsuleHalfHeight();
	if (bReachRight)
	{
		StartClimbReachPoint += UpdatedComponent->GetRightVector() * CapsuleRadius();
	}
	else
	{
		StartClimbReachPoint -= UpdatedComponent->GetRightVector() * CapsuleRadius();
	}

	FVector LookDirection;

	if (DreamCharacterOwner && DreamCharacterOwner->GetFollowCamera())
	{
		FVector RightProjection = DreamCharacterOwner->GetFollowCamera()->GetForwardVector().ProjectOnTo(UpdatedComponent->GetRightVector()).GetSafeNormal();
		FVector UpProjection = DreamCharacterOwner->GetFollowCamera()->GetForwardVector().ProjectOnTo(UpdatedComponent->GetUpVector()).GetSafeNormal();

		LookDirection = (RightProjection + UpProjection).GetSafeNormal();
	}
	
	FVector EndClimbReachPoint = StartClimbReachPoint + LookDirection * ClimbReach;

	FHitResult ReachResult;
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(AlongWallTrace.GetActor());
	UKismetSystemLibrary::SphereTraceSingleByProfile(
		GetWorld(),
		StartClimbReachPoint,
		EndClimbReachPoint,
		10.f,
		TEXT("BlockAll"),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForOneFrame,
		ReachResult,
		true
	);
	return false;
}

void UDreamCharacterMovementComponent::PhysClimbMove(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}

	if (!CharacterOwner || (!CharacterOwner->Controller && bRunPhysicsWithNoController && !HasAnimRootMotion() && CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}

	bJustTeleported = false;
	float remainingTime = DeltaTime;
	while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)))
	{
		Iterations++;
		bJustTeleported = false;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;
		
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();

		FHitResult AlongWallTrace;
		FVector ClimbLocationSide;
		
		FVector ClimbPoint = UpdatedComponent->GetComponentLocation() + UpdatedComponent->GetForwardVector() * (1.f + (CapsuleRadius() * 1.02f)) + FVector::UpVector * (CapsuleHalfHeight());

		//DEBUG_POINT(ClimbPoint, FColor::Red);
		
		FVector StartClimbPoint = ClimbPoint + FVector::UpVector * 5.f;
		FVector EndClimbPoint = ClimbPoint - FVector::UpVector * 5.f;

		GetWorld()->LineTraceSingleByProfile(
			AlongWallTrace,
			StartClimbPoint,
			EndClimbPoint,
			TEXT("BlockAll"),
			DreamCharacterOwner->GetIgnoreCharacterParams()
		);
		if (!AlongWallTrace.IsValidBlockingHit())
		{
			StartNewPhysics(remainingTime, Iterations);
			return;
		}

		FVector VectorAlongWallSurface = FVector::VectorPlaneProject(UpdatedComponent->GetRightVector(), AlongWallTrace.Normal);

		DEBUG_LINE(AlongWallTrace.Location, AlongWallTrace.Location + VectorAlongWallSurface * 100.f, FColor::Green);
		
		FVector MovementVector = (MoveDirectionAxis * VectorAlongWallSurface).GetSafeNormal();
		FVector StartClimbPointSide = ClimbPoint + FVector::UpVector * 5.f + MovementVector * 5.f;
		FVector EndClimbPointSide = ClimbPoint - FVector::UpVector * 5.f + MovementVector * 5.f;
		FHitResult SideTrace;
		GetWorld()->LineTraceSingleByProfile(
			SideTrace,
			StartClimbPointSide,
			EndClimbPointSide,
			TEXT("BlockAll"),
			DreamCharacterOwner->GetIgnoreCharacterParams()
		);
		//DEBUG_LINE(StartClimbPointSide, EndClimbPointSide, FColor::Blue);
		if (!SideTrace.IsValidBlockingHit())
		{
			StartNewPhysics(remainingTime, Iterations);
			return;
		}
		
		Velocity = MovementVector * ClimbMaxSpeed;
		const FVector Delta = timeTick * Velocity;
		const bool bZeroDelta = Delta.IsNearlyZero();
		if (bZeroDelta)
		{
			remainingTime = 0.0f;
		}
		else
		{
			FHitResult Hit;
			SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);

			if (UpdatedComponent->GetComponentLocation() == OldLocation)
			{
				remainingTime = 0.0f;
				break;
			}
			Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick;
		}
		
		//GetWorld()->Get
	}
	return;
}
#pragma endregion 

void UDreamCharacterMovementComponent::PlayRootMotionTransitionMontage(
	const FString& TransitionMontageName, const FVector& TransitionTarget,
	UAnimMontage* TransitionMontage, UAnimMontage* MainMontage,
	const ERootMotionAccumulateMode& RootMotionAccumulateMode)
{
	const float UpSpeed = FVector::DotProduct(Velocity, FVector::UpVector);
	const float TransDistance = FVector::Dist(TransitionTarget, UpdatedComponent->GetComponentLocation());

	TransitionQueuedMontageSpeed = FMath::GetMappedRangeValueClamped(FVector2D(-500, 750), FVector2D(.9f, 1.2f), UpSpeed);
	TransitionRMS.Reset();
	TransitionRMS = MakeShared<FRootMotionSource_MoveToForce>();
	TransitionRMS->AccumulateMode = RootMotionAccumulateMode;

	TransitionRMS->Duration = FMath::Clamp(TransDistance / TransitionDistanceMax, TransitionDurationMin, TransitionDurationMax);
	TransitionRMS->StartLocation = UpdatedComponent->GetComponentLocation();
	TransitionRMS->TargetLocation = TransitionTarget;

	Velocity = FVector::ZeroVector;
	SetMovementMode(MOVE_Flying);
	TransitionRMS_ID = ApplyRootMotionSource(TransitionRMS);
	TransitionQueueMontage = MainMontage;
	TransitionName = TransitionMontageName;
	CharacterOwner->PlayAnimMontage(TransitionMontage, 1 / TransitionRMS->Duration);
}
