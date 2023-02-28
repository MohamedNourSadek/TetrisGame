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
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
#pragma endregion 
	
};
