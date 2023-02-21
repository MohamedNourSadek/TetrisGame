// Fill out your copyright notice in the Description page of Project Settings.


#include "TetrisController.h"

ATetrisController::ATetrisController()
{
	PrimaryActorTick.bCanEverTick = true;
}
void ATetrisController::BeginPlay()
{
	Super::BeginPlay();
	
	ATetrisPiece* piece = Cast<ATetrisPiece>(GetWorld()->SpawnActor(pieceBP, &spawnPoint->GetTransform()));
	spawnedPieces.Add(piece);
}
void ATetrisController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void ATetrisController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

