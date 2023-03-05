// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameUI.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class TETRIS_API UGameUI : public UUserWidget
{
	GENERATED_BODY()

public:

	/** FText value displayed by this widget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text")
	FString DisplayText;

};
