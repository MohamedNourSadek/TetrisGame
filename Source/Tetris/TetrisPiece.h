// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TetrisPiece.generated.h"


UCLASS()
class TETRIS_API ATetrisPiece : public AActor
{
	GENERATED_BODY()

#pragma region Unreal Functions
public:	
	ATetrisPiece();
	void MovePiece(int newX, int newY);

protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
#pragma endregion 
	
#pragma region Public Variables
public:
	UPROPERTY(EditAnywhere) TArray<FIntVector2> geometry;
	UPROPERTY(VisibleAnywhere) int x = 5;
	UPROPERTY(VisibleAnywhere) int y = 20;
};
