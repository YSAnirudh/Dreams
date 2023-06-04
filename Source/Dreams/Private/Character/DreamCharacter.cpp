// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DreamCharacter.h"


#include "Character/Movement/DreamCharacterMovementComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ADreamCharacter::ADreamCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UDreamCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Cast Base Character Movement Component to Our Custom CMC
	DreamCharacterMovementComponent = Cast<UDreamCharacterMovementComponent>(GetCharacterMovement());
	DreamCharacterMovementComponent->SetIsReplicated(true);
	
	// Character Movement settings
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Don't use pawn control rotation
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Create Spring Arm and setup required settings
	if (!CameraBoom)
	{
		CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
		CameraBoom->SetupAttachment(RootComponent);
		CameraBoom->TargetArmLength = 300.f;
		CameraBoom->bUsePawnControlRotation = true;
		CameraBoom->SetRelativeLocation(FVector(0.0f, 50.0f, 90.f));
	}

	// Create Camera and setup required settings
	if (!FollowCamera)
	{
		FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
		FollowCamera->SetupAttachment(CameraBoom);
		FollowCamera->bUsePawnControlRotation = false;
	}
}

// Called when the game starts or when spawned
void ADreamCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADreamCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UE_LOG(LogTemp, Warning, TEXT("%f"), DreamCharacterMovementComponent->MaxWalkSpeed);

	if (!DreamCharacterMovementComponent)
	{
		return;
	}
	//FindInteractableObjectsWithinRadius();
	if (DreamCharacterMovementComponent && DreamCharacterMovementComponent->IsClimbing())
	{
		bUseControllerRotationYaw = false;
	} else
	{
		bUseControllerRotationYaw = true;
	}

	//UE_LOG(LogTemp, Warning, TEXT("Accel: %s"), *DreamCharacterMovementComponent->GetCurrentAcceleration().GetSafeNormal().ToString());
	//UE_LOG(LogTemp, Warning, TEXT("Velo: %s"), *DreamCharacterMovementComponent->Velocity.GetSafeNormal().ToString());
	
}

#pragma region Interaction
void ADreamCharacter::FindInteractable()
{
	FindInteractableObjectsWithinRadius();
	FHitResult Hit;
	const FVector CameraForward = GetFollowCamera()->GetForwardVector();
	const FVector Start = GetFollowCamera()->GetComponentLocation();
	const FVector End = Start + CameraForward * InteractionRange;

	bool bFoundActors = UKismetSystemLibrary::SphereTraceSingleByProfile(
		GetWorld(),
		Start,
		End,
		50.f,
		TEXT("BlockAll"),
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::Persistent,
		Hit,
		true
	);

	if (bFoundActors)
	{
		if (Hit.bBlockingHit)
		{
			AInteractable* Interactable = Cast<AInteractable>(Hit.GetActor());
			if (Interactable && Interactable->GetIsWithinRadius())
			{
				if (CharacterFacingInteractable(Interactable->GetActorLocation()))
				{
					
				}
				//UE_LOG(LogTemp, Warning, TEXT("%s"), *Hit.GetActor()->GetName());
			}
		}
	}
}

void ADreamCharacter::FindInteractableObjectsWithinRadius() const
{
	TArray<FHitResult> CheckInteractHits;

	bool bFoundActors = UKismetSystemLibrary::SphereTraceMultiByProfile(
		GetWorld(),
		GetActorLocation(),
		GetActorLocation(),
		InteractionRadius,
		TEXT("BlockAll"),
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::ForOneFrame,
		CheckInteractHits,
		true
	);

	if (bFoundActors)
	{
		for (FHitResult Hit : CheckInteractHits)
		{
			if (Hit.bBlockingHit)
			{
				AInteractable* Interactable = Cast<AInteractable>(Hit.GetActor());
				if (Interactable)
				{
					Interactable->SetIsWithinRadius(true);
					//UE_LOG(LogTemp, Warning, TEXT("%s"), *Hit.GetActor()->GetName());
				}
			}
		}
	}
}

bool ADreamCharacter::CharacterFacingInteractable(FVector InteractableLocation) const 
{
	float DotSimilarity = FVector::DotProduct(GetActorLocation(), InteractableLocation);
	if (DotSimilarity > 0.75f)
	{
		return true;
	}
	return false;
}
#pragma endregion 

// Called to bind functionality to input
void ADreamCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	AddMappingContexts();
	
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	
	if (Input && BaseInputActionsConfig)
	{
		// Bind JumpAction
		if (BaseInputActionsConfig->JumpAction)
		{
			Input->BindAction(BaseInputActionsConfig->JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
			Input->BindAction(BaseInputActionsConfig->JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		}
		// Bind Movement Action
		if (BaseInputActionsConfig->MovementAction)
		{
			Input->BindAction(BaseInputActionsConfig->MovementAction, ETriggerEvent::Triggered, this, &ADreamCharacter::Move);
		}
		// Bind Look Action
		if (BaseInputActionsConfig->LookAction)
		{
			Input->BindAction(BaseInputActionsConfig->LookAction, ETriggerEvent::Triggered, this, &ADreamCharacter::Look);
		}
		// Bind Sprint Action
		if (BaseInputActionsConfig->SprintAction)
		{
			if (bSprintToggle)
			{
				Input->BindAction(BaseInputActionsConfig->SprintAction, ETriggerEvent::Completed, this, &ADreamCharacter::Sprint);
			}
			else
			{
				Input->BindAction(BaseInputActionsConfig->SprintAction, ETriggerEvent::Triggered, this, &ADreamCharacter::Sprint);
				Input->BindAction(BaseInputActionsConfig->SprintAction, ETriggerEvent::Completed, this, &ADreamCharacter::StopSprint);
			}
		}
		
		// Bind Crouch Action
		if (BaseInputActionsConfig->CrouchAction)
		{
			if (bCrouchToggle)
			{
				Input->BindAction(BaseInputActionsConfig->CrouchAction, ETriggerEvent::Completed, this, &ADreamCharacter::StartCrouch);
			}
			else
			{
				Input->BindAction(BaseInputActionsConfig->CrouchAction, ETriggerEvent::Triggered, this, &ADreamCharacter::StartCrouch);
				Input->BindAction(BaseInputActionsConfig->CrouchAction, ETriggerEvent::Completed, this, &ADreamCharacter::StopCrouch);
			}
		}

		// Bind Walk Action
		if (BaseInputActionsConfig->WalkAction)
		{
			if (bWalkToggle)
			{
				Input->BindAction(BaseInputActionsConfig->WalkAction, ETriggerEvent::Completed, this, &ADreamCharacter::StartWalk);
			}
			else
			{
				Input->BindAction(BaseInputActionsConfig->WalkAction, ETriggerEvent::Triggered, this, &ADreamCharacter::StartWalk);
				Input->BindAction(BaseInputActionsConfig->WalkAction, ETriggerEvent::Completed, this, &ADreamCharacter::StopWalk);
			}
		}

		// Bind Dash Action
		if (BaseInputActionsConfig->DashAction)
		{
			Input->BindAction(BaseInputActionsConfig->DashAction, ETriggerEvent::Triggered, this, &ADreamCharacter::Dash);
			Input->BindAction(BaseInputActionsConfig->DashAction, ETriggerEvent::Completed, this, &ADreamCharacter::StopDash);
		}
	}
}

#pragma region Input
// Add the required Input Mapping Contexts to handle enhanced input
void ADreamCharacter::AddMappingContexts()
{
	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			InputSubsystem->ClearAllMappings();
			if (BaseMappingContext)
			{
				InputSubsystem->AddMappingContext(BaseMappingContext, 0);
			}
		}
	}
}

// Move input using Enhanced Input System
void ADreamCharacter::Move(const FInputActionValue& ActionValue)
{
	const FVector2D InputVector = ActionValue.Get<FVector2D>();
	if (InputVector.X != 0)
	{
		const FVector Right = GetActorRightVector();
		AddMovementInput(Right, InputVector.X);
	}
	if (InputVector.Y != 0)
	{
		const FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, InputVector.Y);
	}
}

// Look input using Enhanced Input System
void ADreamCharacter::Look(const FInputActionValue& ActionValue)
{
	const FVector2D InputVector = ActionValue.Get<FVector2D>();
	if (InputVector.X != 0)
	{
		AddControllerYawInput(InputVector.X * MouseSensitivityX);
	}
	if (InputVector.Y != 0)
	{
		AddControllerPitchInput(InputVector.Y * MouseSensitivityY);
	}
}

// Jump Input
void ADreamCharacter::Jump()
{
	Super::Jump();

	bPressedDreamJump = true;
	bPressedJump = false;
}

void ADreamCharacter::StopJumping()
{
	Super::StopJumping();

	bPressedDreamJump = false;
}

void ADreamCharacter::Sprint(const FInputActionValue& ActionValue)
{
	if (!bSprintToggle)
	{
		DreamCharacterMovementComponent->SprintPressed();
	}
	else
	{
		if (DreamCharacterMovementComponent->GetWantsToSprint())
		{
			DreamCharacterMovementComponent->SprintReleased();
		}
		else
		{
			DreamCharacterMovementComponent->SprintPressed();
		}
	}
}

void ADreamCharacter::StopSprint(const FInputActionValue& ActionValue)
{
	DreamCharacterMovementComponent->SprintReleased();
}

void ADreamCharacter::StartCrouch(const FInputActionValue& ActionValue)
{
	if (!bCrouchToggle)
	{
		DreamCharacterMovementComponent->CrouchPressed();
	}
	else
	{
		if (DreamCharacterMovementComponent->GetWantsToCrouch())
		{
			DreamCharacterMovementComponent->CrouchReleased();
		}
		else
		{
			DreamCharacterMovementComponent->CrouchPressed();
		}
	}
}

void ADreamCharacter::StopCrouch(const FInputActionValue& ActionValue)
{
	DreamCharacterMovementComponent->CrouchReleased();
}

void ADreamCharacter::StartWalk(const FInputActionValue& ActionValue)
{
	if (!bWalkToggle)
	{
		DreamCharacterMovementComponent->WalkPressed();
	}
	else
	{
		if (DreamCharacterMovementComponent->GetWantsToWalk())
		{
			DreamCharacterMovementComponent->WalkReleased();
		}
		else
		{
			DreamCharacterMovementComponent->WalkPressed();
		}
	}
}

void ADreamCharacter::StopWalk(const FInputActionValue& ActionValue)
{
	DreamCharacterMovementComponent->WalkReleased();
}

void ADreamCharacter::Dash(const FInputActionValue& ActionValue)
{
	DreamCharacterMovementComponent->DashPressed();
}

void ADreamCharacter::StopDash(const FInputActionValue& ActionValue)
{
	DreamCharacterMovementComponent->DashReleased();
}
#pragma endregion 

FCollisionQueryParams ADreamCharacter::GetIgnoreCharacterParams() const
{
	FCollisionQueryParams Params;

	TArray<AActor*> CharacterChildren;
	GetAllChildActors(CharacterChildren);
	Params.AddIgnoredActors(CharacterChildren);
	Params.AddIgnoredActor(this);
	
	return Params;
}

