// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class TETRIS_API PiecePrototype
{
public:
	PiecePrototype();
	~PiecePrototype();

	TArray<FIntVector2> subPieces;
	UMaterialInterface* material;
};
