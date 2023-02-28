// Fill out your copyright notice in the Description page of Project Settings.


#include "SubPiece.h"

SubPiece::SubPiece()
{
}
SubPiece::~SubPiece()
{
}

void SubPiece::Move(FIntVector2 newAbsPosition)
{
	FVector newLocation =
		FVector(
			100 * newAbsPosition.X,
			100 * newAbsPosition.Y,
			myPiece->GetActorLocation().Z
		);
	
	myPiece->SetActorLocation(newLocation);
}
