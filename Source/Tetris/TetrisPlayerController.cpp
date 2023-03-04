// Fill out your copyright notice in the Description page of Project Settings.


#include "TetrisPlayerController.h"
#include "Blueprint/UserWidget.h"

void ATetrisPlayerController::BeginPlay()
{
	Super::BeginPlay();

	scoreUI = CreateWidget<UUserWidget>(this, BP_scoreUI);

	if(scoreUI)
		scoreUI->AddToViewport();
}

void ATetrisPlayerController::ChangeScore(int newScore)
{
	if(newScore % 2 == 0)
		scoreUI->SetVisibility( ESlateVisibility::Hidden);
	else
		scoreUI->SetVisibility(ESlateVisibility::Visible);
}

