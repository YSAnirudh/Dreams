// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Movement/DreamCharacterMovementComponent.h"

#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

UDreamCharacterMovementComponent::UDreamCharacterMovementComponent()
{
	NavAgentProps.bCanCrouch = true;
}

void UDreamCharacterMovementComponent::SprintPressed()
{
	bWantsToSprint = true;
}

void UDreamCharacterMovementComponent::SprintReleased()
{
	bWantsToSprint = false;
}

void UDreamCharacterMovementComponent::CrouchPressed()
{
	bWantsToCrouch = true;
}

void UDreamCharacterMovementComponent::CrouchReleased()
{
	bWantsToCrouch = false;
}

void UDreamCharacterMovementComponent::DashPressed()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - DashStartTime >= DashCooldownDuration)
	{
		bWantsToDash = true;
	}
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
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_DashCooldown);
	bWantsToDash = false;
}

void UDreamCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	// DASH
	if (bWantsToDash && CanDash())
	{
		PerformDash();
		bWantsToDash = false;
	}
	
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

void UDreamCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
                                                         const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	switch (MovementMode)
	{
	case MOVE_Walking:
		if (bWantsToSprint)
		{
			MaxWalkSpeed = Sprint_MaxWalkSpeed;
		}
		else
		{
			MaxWalkSpeed = Jog_MaxWalkSpeed;
		}
		break;
	default:
		break;
	}
}

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