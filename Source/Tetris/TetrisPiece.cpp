// Fill out your copyright notice in the Description page of Project Settings.


#include "TetrisPiece.h"

// Sets default values
ATetrisPiece::ATetrisPiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATetrisPiece::BeginPlay()
{
	Super::BeginPlay();

	currentPosition = GetActorLocation().X;
}

// Called every frame
void ATetrisPiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	currentPosition += DeltaTime * speed;

	int currentScale = ((int)(currentPosition / incrementValue))* incrementValue;

	SetActorLocation(FVector(currentScale, GetActorLocation().Y, GetActorLocation().Z));
}

