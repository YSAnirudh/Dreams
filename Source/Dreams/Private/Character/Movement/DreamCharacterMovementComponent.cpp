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
	Safe_bWantsToSprint = true;
}

void UDreamCharacterMovementComponent::SprintReleased()
{
	Safe_bWantsToSprint = false;
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
		Safe_bWantsToDash = true;
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
	Safe_bWantsToDash = false;
}

void UDreamCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	Safe_bWantsToSprint = (Flags & FSavedMove_Main::FLAG_Sprint) != 0;
	Safe_bWantsToDash = (Flags & FSavedMove_Main::FLAG_Dash) != 0;
}

void UDreamCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	// DASH
	bool bAuthProxy = CharacterOwner->HasAuthority() && !CharacterOwner->IsLocallyControlled();
	if (Safe_bWantsToDash && CanDash())
	{
		if (!bAuthProxy || GetWorld()->GetTimeSeconds() - DashStartTime > AuthDashCooldownDuration)
		{
			PerformDash();
			Safe_bWantsToDash = false;
			Proxy_bDashStart = !Proxy_bDashStart;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Tried To Cheat!"));
		}
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
		if (Safe_bWantsToSprint)
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
	Safe_bWantsToDash = true;
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

void UDreamCharacterMovementComponent::OnRep_DashStart()
{
	DashStartDelegate.Broadcast();
}

#pragma endregion 

void UDreamCharacterMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UDreamCharacterMovementComponent, Proxy_bDashStart, COND_SkipOwner);
}

#pragma region SavedMove

UDreamCharacterMovementComponent::FSavedMove_Main::FSavedMove_Main()
{
	Saved_bWantsToSprint = 0;
	Saved_bWantsToDash = 0;
}

// Checks Current and the new move and checks if we can combine those two moves (Identical Data)
bool UDreamCharacterMovementComponent::FSavedMove_Main::CanCombineWith(const FSavedMovePtr& NewMove,
	ACharacter* InCharacter, float MaxDelta) const
{
	FSavedMove_Main* NewMainMove = static_cast<FSavedMove_Main*>(NewMove.Get());

	if (Saved_bWantsToSprint != NewMainMove->Saved_bWantsToSprint)
	{
		return false;
	}

	if (Saved_bWantsToDash != NewMainMove->Saved_bWantsToDash)
	{
		return false;
	}
	
	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

// Clear data in the saved move for population
void UDreamCharacterMovementComponent::FSavedMove_Main::Clear()
{
	FSavedMove_Character::Clear();

	Saved_bWantsToSprint = 0;
	Saved_bWantsToDash = 0;
}

// Sending minimal data to the server for replicating movement (Every frame)
uint8 UDreamCharacterMovementComponent::FSavedMove_Main::GetCompressedFlags() const
{
	int8 Result = FSavedMove_Character::GetCompressedFlags();

	if (Saved_bWantsToSprint)
	{
		Result |= FLAG_Sprint;
	}
	
	if (Saved_bWantsToDash)
	{
		Result |= FLAG_Dash;
	}
	
	return Result;
}

// Set all the Saved Move variables using the safe moves
void UDreamCharacterMovementComponent::FSavedMove_Main::SetMoveFor(ACharacter* C, float InDeltaTime,
	FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	const TObjectPtr<UDreamCharacterMovementComponent> CharacterMovement = Cast<UDreamCharacterMovementComponent>(C->GetCharacterMovement());

	Saved_bWantsToSprint =	CharacterMovement->Safe_bWantsToSprint;
	Saved_bWantsToDash = CharacterMovement->Safe_bWantsToDash;
}

// Set all the Safe Move variables using the saved moves
void UDreamCharacterMovementComponent::FSavedMove_Main::PrepMoveFor(ACharacter* C)
{
	FSavedMove_Character::PrepMoveFor(C);

	TObjectPtr<UDreamCharacterMovementComponent> CharacterMovement = Cast<UDreamCharacterMovementComponent>(C->GetCharacterMovement());

	CharacterMovement->Safe_bWantsToSprint = Saved_bWantsToSprint;
	CharacterMovement->Safe_bWantsToDash = Saved_bWantsToDash;
}

#pragma endregion 

#pragma region NetworkPrediction

FNetworkPredictionData_Client* UDreamCharacterMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != nullptr)

	if (ClientPredictionData == nullptr)
	{
		TObjectPtr<UDreamCharacterMovementComponent> MutableThis = const_cast<UDreamCharacterMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Main(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}
	
	return ClientPredictionData;
}

UDreamCharacterMovementComponent::FNetworkPredictionData_Client_Main::FNetworkPredictionData_Client_Main(
	const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement)
{
}

// Tell UE5 that we are using our custom saved move to replicate movement
FSavedMovePtr UDreamCharacterMovementComponent::FNetworkPredictionData_Client_Main::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_Main());
}

#pragma endregion

