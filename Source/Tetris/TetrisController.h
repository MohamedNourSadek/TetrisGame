// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TetrisPiece.h"
#include "TetrisController.generated.h"


UCLASS()
class TETRIS_API ATetrisController : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATetrisController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

//private variables
private:
	UPROPERTY(EditAnywhere)
		float tickEvery = 1;

	UPROPERTY(EditAnywhere)
		AActor* spawnPoint;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> pieceBP;

	UPROPERTY(EditAnywhere)
		TArray<ATetrisPiece*> spawnedPieces;

	UPROPERTY(EditAnywhere)
		ATetrisPiece* currentPiece;


	int lastSecondTicked = 0;
	float timeSinceStartUp = 0;

//private functions
private:
	int LastCollision();
	void InitializePiece(ATetrisPiece& piece);
	void SpawnNewPiece();
};
