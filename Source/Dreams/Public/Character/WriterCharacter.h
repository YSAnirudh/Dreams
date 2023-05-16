// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/DreamCharacter.h"
#include "WriterCharacter.generated.h"

/**
 * 
 */
UCLASS()
class DREAMS_API AWriterCharacter : public ADreamCharacter
{
	GENERATED_BODY()
public:
	// --- FUNCTIONS ---

	// Basic Constructor - Sets Default Values if needed
	AWriterCharacter(const FObjectInitializer& ObjectInitializer);

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called to bind functionality to player's input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	// --- VARIABLES ---

protected:
	// --- FUNCTIONS ---

	// Called when game starts or object is spawned
	virtual void BeginPlay() override;

	// --- VARIABLES ---

private:
	// --- FUNCTIONS ---

	// --- VARIABLES ---
};
