// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Camera/DreamPlayerCameraManager.h"

#include "Character/DreamCharacter.h"
#include "Character/Movement/DreamCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

ADreamPlayerCameraManager::ADreamPlayerCameraManager()
{
}

void ADreamPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);

	if (const TObjectPtr<ADreamCharacter> DreamCharacter = Cast<ADreamCharacter>(GetOwningPlayerController()->GetPawn()))
	{
		const TObjectPtr<UDreamCharacterMovementComponent> DreamCMC = DreamCharacter->GetDreamCharacterMovement();
		const FVector TargetCrouchOffset = FVector(
			0.0f, 0.0f,
			DreamCMC->GetCrouchedHalfHeight() -
			DreamCharacter->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()
		);

		FVector Offset = FMath::Lerp(FVector::ZeroVector, TargetCrouchOffset, FMath::Clamp(CrouchBlendTime/CrouchBlendDuration, 0.0f, 1.0f));

		if (DreamCMC->IsCrouching())
		{
			CrouchBlendTime = FMath::Clamp(CrouchBlendTime + DeltaTime, 0.0f, CrouchBlendDuration);
			Offset -= TargetCrouchOffset;
		}
		else
		{
			CrouchBlendTime = FMath::Clamp(CrouchBlendTime - DeltaTime, 0.0f, CrouchBlendDuration);
		}

		if (DreamCMC->IsMovingOnGround())
		{
			OutVT.POV.Location += Offset;
		}
	}
}
