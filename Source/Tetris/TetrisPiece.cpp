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
}

// Called every frame
void ATetrisPiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATetrisPiece::MovePiece(int newX, int newY)
{
	x = newX;
	y = newY; 

	FVector currentWorldLocation = GetActorLocation();
	FVector newLocation =
		FVector(
			100 * newX,
			100 * newY,
			currentWorldLocation.Z
		);

	SetActorLocation(newLocation);
}
