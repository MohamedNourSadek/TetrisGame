// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TetrisPiece.h"
#include "CompoundPiece.h"
#include "SubPiece.h"
#include "PiecePrototype.h"
#include "TetrisPlayerController.h"
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
	UPROPERTY(EditAnywhere) float tickEvery = 1;
	UPROPERTY(EditAnywhere) float acceleration = 1;
	UPROPERTY(EditAnywhere) AActor* spawnPoint;
	UPROPERTY(EditAnywhere) TSubclassOf<AActor> unitPiece;
	UPROPERTY(EditAnywhere) UMaterialInterface* red;
	UPROPERTY(EditAnywhere) UMaterialInterface* blue;
	UPROPERTY(EditAnywhere) UMaterialInterface* green;
	UPROPERTY(EditAnywhere) UMaterialInterface* orange;

	ATetrisPlayerController* controller;
	TArray<PiecePrototype> prototypePieces;
	CompoundPiece* currentPiece;
	TArray<SubPiece*> spawnedPieces;
	bool gameIsOn = false;
	bool movingAPiece = false;
	int lastSecondTicked = 0;
	float timeSinceStartUp = 0;
	int totalScore = 0;

#pragma endregion

#pragma region Private Functions
private:
	void InitializeData();
	void SpawnNewPiece();

	FIntVector2 TransformSpace(FIntVector2 input);
	bool CanMove(CompoundPiece* compPiece, FIntVector2 newPosition);
	bool IsPositionPossible(FIntVector2 newPosition);
	int GetFirstCollisionY(CompoundPiece* compoundPiece);
	void ReorganizePieces();
	TArray<int>* GetCompleteRows();
	TArray<FIntVector2>* FindOccupied();
	PiecePrototype GetRandomProtoype();
	void RotatePiece();

	void RightRecieved();
	void LeftRecieved();
	void JumpRecieved();
	void RotateRecieved();
#pragma endregion

};
