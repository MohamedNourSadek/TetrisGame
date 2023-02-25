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

#pragma region Unreal Functions
public:
	ATetrisController();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:
	virtual void BeginPlay() override;
#pragma endregion 

#pragma region Private Variables
private:
	UPROPERTY(EditAnywhere) bool LogActions = false;
	UPROPERTY(EditAnywhere) float tickEvery = 1;
	UPROPERTY(EditAnywhere) AActor* spawnPoint;
	UPROPERTY(EditAnywhere) TArray<TSubclassOf<AActor>> pieces;
	UPROPERTY(EditAnywhere)	TArray<ATetrisPiece*> spawnedPieces;
	UPROPERTY(EditAnywhere) ATetrisPiece* currentPiece;
	bool gameIsOn = false;
	bool movingAPiece = false;
	int lastSecondTicked = 0;
	float timeSinceStartUp = 0;
#pragma endregion

#pragma region Private Functions
private:
	int LastCollision();
	void InitializePiece(ATetrisPiece& piece);
	void SpawnNewPiece();
	void ReorganizePieces();
	void MovePiece(ATetrisPiece* piece,int newX,int newY);
	void RightRecieved();
	void LeftRecieved();
	void JumpRecieved();
	void SplitPiece(ATetrisPiece* piece);
	TArray<FIntVector2>* FindOccupied(const ATetrisPiece* myPiece);
#pragma endregion

};
