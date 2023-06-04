// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "DreamRepeater.generated.h"

UCLASS()
class DREAMS_API ADreamRepeater : public AActor
{
	GENERATED_BODY()
	
public:
	// --- FUNCTIONS ---

	// Sets default values for this actor's properties
	ADreamRepeater();

	virtual void OnConstruction(const FTransform& Transform) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// --- VARIABLES ---

protected:
	// --- FUNCTIONS ---
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// --- VARIABLES ---

private:
	// --- FUNCTIONS ---
	float GetSplineSpacing() const;
	void SpawnInstancedMeshes();

	// --- VARIABLES ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Repeater|Mesh", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMesh> BaseMeshToSpawn = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Repeater", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInstancedStaticMeshComponent> InstancedMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Repeater", meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneRootComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Repeater|SplineSettings", meta = (AllowPrivateAccess = "true"))
	USplineComponent* RepeatSplineComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Repeater|SplineSettings", meta = (AllowPrivateAccess = "true"))
	float SplineMeshOffset = 0.0f;

	float SplineSpacing = 0.0f;

};
