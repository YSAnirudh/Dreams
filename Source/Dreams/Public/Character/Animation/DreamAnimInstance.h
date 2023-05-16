// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character/WriterCharacter.h"
#include "DreamAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class DREAMS_API UDreamAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	// --- FUNCTIONS ---
    // Base Constructor
    UDreamAnimInstance();

	// Initialize variables required for animation
    virtual void NativeInitializeAnimation() override;

	// Update the properties every frame. This is used to change animations based on variables
	UFUNCTION(BlueprintCallable, Category = "Animation|Character")
	virtual void UpdateAnimationProperties(float DeltaSeconds);
	
	// --- VARIABLES ---
protected:
	// --- FUNCTIONS ---
    
    // --- VARIABLES ---
private:
	// --- FUNCTIONS ---
    
	// --- VARIABLES ---
	// Reference of the BP_WriterCharacter to access data to update animations
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation|Character", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<AWriterCharacter> WriterCharacterRef = nullptr;

	// GROUND LOCOMOTION
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation|Ground Movement", meta=(AllowPrivateAccess = "true"))
	float Speed = 0.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation|Ground Movement", meta=(AllowPrivateAccess = "true"))
	bool bIsInAir = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation|Ground Movement", meta=(AllowPrivateAccess = "true"))
	bool bIsWalking = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation|Ground Movement", meta=(AllowPrivateAccess = "true"))
	bool bIsJogging = false;
};
