// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SubPiece.h"

/**
 * 
 */
class TETRIS_API CompoundPiece
{
public:
	CompoundPiece();
	~CompoundPiece();
	void Move(FIntVector2 newAbsPosition);
	
	TArray<SubPiece> subPieces;
	FIntVector2 position;
};
