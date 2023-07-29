// Fill out your copyright notice in the Description page of Project Settings.


#include "CompoundPiece.h"

CompoundPiece::CompoundPiece()
{
}
CompoundPiece::~CompoundPiece()
{
}

void CompoundPiece::Move(FIntVector2 newAbsPosition)
{
	position = newAbsPosition;
	
	for(SubPiece* subPiece : subPieces)
	{
		subPiece->Move(
			FIntVector2(
				newAbsPosition.X + subPiece->relativePosition.X,
				newAbsPosition.Y + subPiece->relativePosition.Y));
	}
}
