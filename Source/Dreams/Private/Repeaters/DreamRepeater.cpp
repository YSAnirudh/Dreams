// Fill out your copyright notice in the Description page of Project Settings.


#include "Repeaters/DreamRepeater.h"

#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
ADreamRepeater::ADreamRepeater()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root Component"));
	SetRootComponent(SceneRootComponent);
	
	InstancedMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedMesh"));
	
	InstancedMesh->SetupAttachment(RootComponent);

	RepeatSplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("RepeatSpline"));
	RepeatSplineComponent->SetWorldLocation(GetActorLocation());
	RepeatSplineComponent->SetupAttachment(RootComponent);

}

void ADreamRepeater::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	InstancedMesh->ClearInstances();
	if (BaseMeshToSpawn)
	{
		InstancedMesh->SetStaticMesh(BaseMeshToSpawn.Get());
	}
	SpawnInstancedMeshes();
}

// Called when the game starts or when spawned
void ADreamRepeater::BeginPlay()
{
	Super::BeginPlay();
	
}

float ADreamRepeater::GetSplineSpacing() const
{
	if (BaseMeshToSpawn)
	{
		const FVector MeshDimensions = BaseMeshToSpawn->GetBoundingBox().Max - BaseMeshToSpawn->GetBoundingBox().Min;
		//UE_LOG(LogTemp, Warning, TEXT("%f"), MeshDimensions.X + SplineMeshOffset);
		return MeshDimensions.X + SplineMeshOffset;
	}
	return -1.0f;
}

void ADreamRepeater::SpawnInstancedMeshes()
{
	SplineSpacing = GetSplineSpacing();
	if (SplineSpacing < 0.0f)
	{
		return;
	}

	int32 NoOfMeshesToSpawn = static_cast<int32>(FMath::Floor(RepeatSplineComponent->GetSplineLength() / SplineSpacing));

	for (int i = 0; i < NoOfMeshesToSpawn; i++)
	{
		const FVector SplineSpawnLocation = RepeatSplineComponent->GetLocationAtDistanceAlongSpline(
			SplineSpacing * i,
			ESplineCoordinateSpace::Local
		);

		const FRotator SplineSpawnRotation= RepeatSplineComponent->GetRotationAtDistanceAlongSpline(
			SplineSpacing * i,
			ESplineCoordinateSpace::Local
		);
		FTransform SplineSpawnTransform = FTransform(
			SplineSpawnRotation,
			SplineSpawnLocation
		);

		InstancedMesh->AddInstance(SplineSpawnTransform);
	}

}

// Called every frame
void ADreamRepeater::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

