// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TetrisPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TETRIS_API ATetrisPlayerController : public APlayerController
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere) TSubclassOf<class UUserWidget> BP_scoreUI;
	UUserWidget* scoreUI;

public:
	void ChangeScore(int newScore);

};
