// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Movement/DreamCharacterMovementComponent.h"

#include "GameFramework/Character.h"

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

void UDreamCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	Safe_bWantsToSprint = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
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

// Checks Current and the new move and checks if we can combine those two moves (Identical Data)
bool UDreamCharacterMovementComponent::FSavedMove_Main::CanCombineWith(const FSavedMovePtr& NewMove,
	ACharacter* InCharacter, float MaxDelta) const
{
	FSavedMove_Main* NewMainMove = static_cast<FSavedMove_Main*>(NewMove.Get());

	if (Saved_bWantsToSprint != NewMainMove->Saved_bWantsToSprint)
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
}

// Sending minimal data to the server for replicating movement (Every frame)
uint8 UDreamCharacterMovementComponent::FSavedMove_Main::GetCompressedFlags() const
{
	int8 Result = Super::GetCompressedFlags();

	if (Saved_bWantsToSprint)
	{
		Result |= FLAG_Custom_0;
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
}

// Set all the Safe Move variables using the saved moves
void UDreamCharacterMovementComponent::FSavedMove_Main::PrepMoveFor(ACharacter* C)
{
	FSavedMove_Character::PrepMoveFor(C);

	TObjectPtr<UDreamCharacterMovementComponent> CharacterMovement = Cast<UDreamCharacterMovementComponent>(C->GetCharacterMovement());

	CharacterMovement->Safe_bWantsToSprint = Saved_bWantsToSprint;
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
