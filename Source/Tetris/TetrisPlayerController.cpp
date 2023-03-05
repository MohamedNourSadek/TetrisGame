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
	scoreUI->DisplayText =  FString::FromInt(newScore);
}

