// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../TetrisPiece.h"

class TETRIS_API SubPiece
{
public:
	SubPiece();
	~SubPiece();

	void Move(FIntVector2 newAbsPosition);
	
	FIntVector2 relativePosition;
	FIntVector2 absPosition;
	ATetrisPiece* myPiece;
};
