// Fill out your copyright notice in the Description page of Project Settings.


#include "TetrisPiece.h"

ATetrisPiece::ATetrisPiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ATetrisPiece::BeginPlay()
{
	Super::BeginPlay();
}

void ATetrisPiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

