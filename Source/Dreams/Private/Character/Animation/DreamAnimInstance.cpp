// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/DreamAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"

UDreamAnimInstance::UDreamAnimInstance()
{
}

void UDreamAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// Get Writer Character Reference
	if (!WriterCharacterRef)
	{
		WriterCharacterRef = Cast<AWriterCharacter>(TryGetPawnOwner());
	}
}

void UDreamAnimInstance::UpdateAnimationProperties(float DeltaSeconds)
{
	// Get Writer Character Reference, if NULL
	if (!WriterCharacterRef)
	{
		WriterCharacterRef = Cast<AWriterCharacter>(TryGetPawnOwner());
		if (!WriterCharacterRef)
		{
			return;
		}
	}
}
