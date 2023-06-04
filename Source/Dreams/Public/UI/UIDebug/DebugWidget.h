// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/DreamCharacter.h"
#include "DebugWidget.generated.h"

/**
 * 
 */
UCLASS()
class DREAMS_API UDebugWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    // --- FUNCTIONS ---
	// Sets default values for this widget's properties
    virtual void NativeConstruct() override;

    // Debug Toggle
    UFUNCTION(BlueprintCallable)
    void DebugToggle();

    // --- VARIABLES ---
protected:
    // --- FUNCTIONS ---

    // --- VARIABLES ---
private:
    // --- FUNCTIONS ---

    // --- VARIABLES ---
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Character", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<ADreamCharacter> DreamCharacterRef = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings", meta = (AllowPrivateAccess="true"))
    bool bDebug = false;
};
