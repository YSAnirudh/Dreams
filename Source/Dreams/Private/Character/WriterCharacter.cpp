// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/WriterCharacter.h"

// Constructor - Sets default values if needed
AWriterCharacter::AWriterCharacter(const FObjectInitializer& ObjectInitializer)
	: ADreamCharacter(ObjectInitializer)
{
}

// Called when game starts or object is spawned
void AWriterCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWriterCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

// Called to bind functionality to player's input
void AWriterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

