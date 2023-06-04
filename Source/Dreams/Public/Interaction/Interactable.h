// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Interactable.generated.h"

UCLASS()
class DREAMS_API AInteractable : public AActor
{
	GENERATED_BODY()
	
public:
	// --- FUNCTIONS ---
	// Sets default values for this actor's properties
	AInteractable();

	// Function used by the Dream Character to interact with this object.
	virtual void InteractFunction(class ADreamCharacter* DreamCharacter);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// INLINE FUNCTIONS
	// Getters for Private variables
	FORCEINLINE bool GetCanInteract() const { return bCanInteract; }
	FORCEINLINE bool GetIsWithinRadius() const { return bIsWithinRadius; }
	FORCEINLINE bool GetIsInteractable() const { return bIsInteractable; }

	// Setters for Private Variables
	FORCEINLINE void SetCanInteract(bool CanInteract) { bCanInteract = CanInteract; }
	FORCEINLINE void SetIsWithinRadius(bool WithinRadius) { bIsWithinRadius = WithinRadius; }

	// --- VARIABLES ---
	
protected:
	// --- FUNCTIONS ---
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// --- VARIABLES ---

private:
	// --- FUNCTIONS ---
	// Base Mesh variable for the interactable
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Interaction|Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BaseMesh = nullptr;

	// Sphere around the interactable to detect Character Interaction better
	// UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Interaction|Components", meta = (AllowPrivateAccess = "true"))
	// USphereComponent* InteractionSphere = nullptr;

	// --- VARIABLES ---
	// If the actual actor is Interactable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Settings", meta = (AllowPrivateAccess = "true"))
	bool bIsInteractable = false;

	// If the Character is within the radius of the Interaction Sphere
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction|Settings", meta = (AllowPrivateAccess = "true"))
	bool bIsWithinRadius = false;

	// If the Character can actually interact with the interactable
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction|Settings", meta = (AllowPrivateAccess = "true"))
	bool bCanInteract = false;
};
