// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/DreamAnimInstance.h"

#include "Character/Movement/DreamCharacterMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UDreamAnimInstance::UDreamAnimInstance()
{
}

void UDreamAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// Get Dream Character Reference
	if (!DreamCharacterRef)
	{
		DreamCharacterRef = Cast<ADreamCharacter>(TryGetPawnOwner());
	}
}

void UDreamAnimInstance::UpdateAnimationProperties(float DeltaSeconds)
{
	// Get Dream Character Reference, if NULL
	if (!DreamCharacterRef)
	{
		DreamCharacterRef = Cast<ADreamCharacter>(TryGetPawnOwner());
	}
	if (DreamCharacterRef)
	{
	    Speed = DreamCharacterRef->GetVelocity().Size();

		bIsJogging = DreamCharacterRef->bIsJogging;
		bIsCrouching = DreamCharacterRef->bIsCrouched;
		bIsSprinting = DreamCharacterRef->bIsSprinting;
		bIsWalking = DreamCharacterRef->bIsWalking;
		
		if (DreamCharacterRef && DreamCharacterRef->GetDreamCharacterMovement())
		{
			//bIsInAir = DreamCharacterRef->GetDreamCharacterMovement()->IsFalling();
		}
	}
}
