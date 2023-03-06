// Fill out your copyright notice in the Description page of Project Settings.


#include "TetrisPlayerController.h"
#include "GameUI.h"
#include "Blueprint/UserWidget.h"

void ATetrisPlayerController::BeginPlay()
{
	Super::BeginPlay();

	scoreUI = CreateWidget<UGameUI>(this, BP_scoreUI);

	if(scoreUI)
		scoreUI->AddToViewport();
}
void ATetrisPlayerController::ChangeScore(int newScore)
{
	scoreUI->displayText =  FString::FromInt(newScore);
}

void ATetrisPlayerController::ChangeUiState(int isGameOn)
{
	scoreUI->isGameOn = isGameOn;

	if(isGameOn == 0)
	{
		bShowMouseCursor = true;
		bEnableClickEvents = true;
		bEnableMouseOverEvents = true;
	}
}


