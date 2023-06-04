// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Interactable.h"

// Sets default values
AInteractable::AInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	RootComponent = BaseMesh;
}

void AInteractable::InteractFunction(ADreamCharacter* DreamCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("Interacted"));
}

// Called every frame
void AInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsWithinRadius && bCanInteract)
	{
		// Show Widgets
	}
	else if (bIsWithinRadius)
	{
		// Show Widgets
	}
}

// Called when the game starts or when spawned
void AInteractable::BeginPlay()
{
	Super::BeginPlay();
	bIsWithinRadius = false;
	bCanInteract = false;
}

