// Fill out your copyright notice in the Description page of Project Settings.


#include "TetrisController.h"

// Sets default values
ATetrisController::ATetrisController()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATetrisController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
float totalTimeSinceStart = 0;
int lastInitializedTime = 0;

void ATetrisController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	totalTimeSinceStart += DeltaTime;

	int descretTime = (int)(totalTimeSinceStart);

	if (descretTime != lastInitializedTime)
	{
		lastInitializedTime = descretTime;
		GetWorld()->SpawnActor(pieceBP,&spawnPoint->GetTransform());
	}
}

// Called to bind functionality to input
void ATetrisController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

