// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TetrisPiece.h"
#include "CompoundPiece.h"
#include "SubPiece.h"
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
	UPROPERTY(EditAnywhere) AActor* spawnPoint;
	UPROPERTY(EditAnywhere) TSubclassOf<AActor> unitPiece;


	TArray<TArray<FIntVector2>> prototypePieces;
	TMap<FIntVector2, FIntVector2> rotationTransform;
	CompoundPiece* currentPiece;
	TArray<SubPiece*> spawnedPieces;
	bool gameIsOn = false;
	bool movingAPiece = false;
	int lastSecondTicked = 0;
	float timeSinceStartUp = 0;

#pragma endregion

#pragma region Private Functions
private:
	void InitializeData();
	void SpawnNewPiece();

	bool CanMove(CompoundPiece* compPiece, FIntVector2 newPosition);
	int GetFirstCollisionY(CompoundPiece* compoundPiece);
	void ReorganizePieces();
	TArray<int>* GetCompleteRows();
	TArray<FIntVector2>* FindOccupied();
	TArray<FIntVector2> GetRandomProtoype();
	void RotatePiece();

	void RightRecieved();
	void LeftRecieved();
	void JumpRecieved();
	void RotateRecieved();
#pragma endregion

};
