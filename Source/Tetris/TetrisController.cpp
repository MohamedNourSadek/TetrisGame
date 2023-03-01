// Fill out your copyright notice in the Description page of Project Settings.

#include "TetrisController.h"


#pragma region Unreal Functions
ATetrisController::ATetrisController()
{
	PrimaryActorTick.bCanEverTick = true;
}
void ATetrisController::InitializeData()
{
	//Initialize Transformation Matrix
	rotationTransform.Add(FIntVector2(0,0), FIntVector2(2,0));
	rotationTransform.Add(FIntVector2(1,0), FIntVector2(2,1));
	rotationTransform.Add(FIntVector2(2,0), FIntVector2(2,2));
	rotationTransform.Add(FIntVector2(0,1), FIntVector2(1,0));
	rotationTransform.Add(FIntVector2(1,1), FIntVector2(1,1));
	rotationTransform.Add(FIntVector2(2,1), FIntVector2(1,2));
	rotationTransform.Add(FIntVector2(0,2), FIntVector2(0,0));
	rotationTransform.Add(FIntVector2(1,2), FIntVector2(0,1));
	rotationTransform.Add(FIntVector2(2,2), FIntVector2(0,2));

	//Initialize possible pieces

	prototypePieces.Add(TArray<FIntVector2> {FIntVector2(0,0)});
	prototypePieces.Add(TArray<FIntVector2> {FIntVector2(0,0), FIntVector2(1,0),FIntVector2(-1,0)});
}
void ATetrisController::BeginPlay()
{
	Super::BeginPlay();
	InitializeData();
	SpawnNewPiece();
	gameIsOn =true;
}
void ATetrisController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(gameIsOn)
	{
		timeSinceStartUp += DeltaTime;
	
		int timeDescrete = (int)(timeSinceStartUp/ tickEvery);
	
		if ((timeDescrete != lastSecondTicked) && (!movingAPiece))
		{
			lastSecondTicked = timeDescrete;
			
			int collisionLimit = GetFirstCollisionY(currentPiece);
			
			UE_LOG(LogTemp, Display, TEXT("%d"), collisionLimit);
			
			if (collisionLimit > 20)
			{
				UE_LOG(LogTemp, Display, TEXT("Game is Over"))
				gameIsOn = false;
			}
			else if (currentPiece.position.Y > collisionLimit)
			{
				FIntVector2 newPosition = FIntVector2(currentPiece.position.X, currentPiece.position.Y - 1);
				currentPiece.Move(newPosition);
			}
			else
			{
				SpawnNewPiece();
			}
		}
	}
}
void ATetrisController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Left", IE_Pressed, this, &ATetrisController::LeftRecieved);
	PlayerInputComponent->BindAction("Right", IE_Pressed, this, &ATetrisController::RightRecieved);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ATetrisController::JumpRecieved);
	PlayerInputComponent->BindAction("Rotate", IE_Pressed, this, &ATetrisController::RotateRecieved);
}
#pragma endregion

#pragma region Gameplay Functions
void ATetrisController::SpawnNewPiece()
{
	ReorganizePieces();

	TArray<FIntVector2> newPiecePrototype = GetRandomProtoype();
	
	CompoundPiece compoundPiece;

	compoundPiece.position = FIntVector2(((spawnPoint->GetActorLocation().X / 100),spawnPoint->GetActorLocation().Y / 100));
	
	for(FIntVector2 newSubPiecePrototype : newPiecePrototype)
	{
		ATetrisPiece* piece = Cast<ATetrisPiece>(GetWorld()->SpawnActor(unitPiece, &spawnPoint->GetTransform()));

		FVector deltaNewLocation =
			FVector(
				100 * newSubPiecePrototype.X,
				100 * newSubPiecePrototype.Y,
				piece->GetActorLocation().Z);

		piece->SetActorLocation(piece->GetActorLocation() + deltaNewLocation);

		SubPiece subPiece;

		subPiece.relativePosition = newSubPiecePrototype;
		subPiece.absPosition = FIntVector2(subPiece.relativePosition.X + compoundPiece.position.X,
											subPiece.relativePosition.Y + compoundPiece.position.Y);
		subPiece.myPiece = piece;

		compoundPiece.subPieces.Add(subPiece);
		spawnedPieces.Add(&subPiece);
	}
}
TArray<FIntVector2> ATetrisController::GetRandomProtoype()
{
	int randomPiece = FMath::RandRange(0,prototypePieces.Num() - 1);
	TArray<FIntVector2> newPiecePrototype = prototypePieces[randomPiece];
	return newPiecePrototype;
}

int ATetrisController::GetFirstCollisionY(CompoundPiece compoundPiece)
{
	TArray<FIntVector2>* occupied = FindOccupied();
	
	if (occupied->Num() == 0)
	{
		delete occupied;
		return 1;
	}

	int startY = 20;
	for(SubPiece &subPiece : compoundPiece.subPieces)
	{
		if(subPiece.absPosition.Y < startY)
			startY = subPiece.absPosition.Y;
	}

	for(int y = startY; y >= 1; y--)
	{
		for(SubPiece &subPiece : compoundPiece.subPieces)
		{
			for (FIntVector2 point : *occupied)
			{
				if(subPiece.absPosition.X == point.X && subPiece.absPosition.Y == point.Y)
				{
					delete occupied;
					return subPiece.absPosition.Y + 1;
				}
			}
		}
	}
	
	delete occupied;
	return 1;
}
TArray<FIntVector2>* ATetrisController::FindOccupied()
{
	TArray<FIntVector2>* occupied = new TArray<FIntVector2>();
	
	for (SubPiece* subPiece : spawnedPieces)
	{
		bool oneOfMyPieces = false;

		for(SubPiece &mySubPiece : currentPiece.subPieces)
		{
			if (mySubPiece.absPosition == subPiece->absPosition)
				oneOfMyPieces = true;
		}
		
		if (!oneOfMyPieces)
		{
			occupied->Add(subPiece->absPosition);
		}
	}

	return occupied;
}
void ATetrisController::ReorganizePieces()
{
	//finding complete rows
	TArray<int> compelteRows;
	for(int i = 1; i <= 20; i++)
	{
		int totalSum = 0;
		
		for(SubPiece* subPiece : spawnedPieces)
		{
			if(subPiece != nullptr)
			{
				if(subPiece->absPosition.Y == i)
					totalSum += 1;
			}
		}

		if(totalSum == 10)
			compelteRows.Add(i);
	}

	//destroying and moving pieces
	for(int completeRow : compelteRows)
	{
		TArray<SubPiece*> piecesToRemove;
		
		for(SubPiece* subPiece : spawnedPieces)
		{
			if(subPiece->absPosition.Y == completeRow)
				piecesToRemove.Add(subPiece);
		}

		for(SubPiece* subPiece : spawnedPieces)
		{
			spawnedPieces.Remove(subPiece);
			subPiece->myPiece->Destroy();
		}

		for(SubPiece* subPiece : spawnedPieces)
		{
			if(subPiece->absPosition.Y > completeRow)
				subPiece->Move(FIntVector2(subPiece->absPosition.X, subPiece->absPosition.Y - 1));
		}
	}
}
bool ATetrisController::CanMove(CompoundPiece compundPiece, FIntVector2 newPosition)
{
	if(movingAPiece != true)
	{
		bool reachedLeft = false;
		bool reachedRight = false;
		bool newPosOccupied = false;

		TArray<FIntVector2>* occupiedPos = FindOccupied();

		for(SubPiece &subPiece: compundPiece.subPieces)
		{
			FIntVector2 newSubPiecePosition = FIntVector2(newPosition.X + subPiece.relativePosition.X,
													newPosition.Y + subPiece.relativePosition.Y);

			if(newSubPiecePosition.X > 10)
			{
				reachedLeft = true;
			}
			else if(newSubPiecePosition.X < 1)
			{
				reachedRight = true;
			}

			for (FIntVector2 occupied : occupiedPos)
			{
				if((subPiece.absPosition.X == occupied.X) && (subPiece.absPosition.Y == occupied.Y))
				{
					newPosOccupied = true;
				}
			}
		}

		if(!reachedLeft && !reachedRight && !newPosOccupied)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}
void ATetrisController::RotatePiece()
{
	TArray<FIntVector2> newAbsPositions;
	TArray<FIntVector2> newRelativePositions;
	
	for(SubPiece& subPiece : currentPiece)
	{
		FIntVector2 newRelativePos = rotationTransform.FindRef(subPiece.relativePosition);
		FIntVector2 newAbsPosition = FIntVector2(subPiece.absPosition.X + newRelativePos.X, subPiece.absPosition.X + newRelativePos.Y);
		
		newRelativePositions.Add(newRelativePos);
		newAbsPositions.Add(newAbsPosition);
	}

	for(int i = 0; i < newAbsPositions.Num(); i++)
	{
		currentPiece.subPieces[i].relativePosition = newRelativePositions[i];
		currentPiece.subPieces[i].Move(newAbsPositions[i]);
	}
}
#pragma endregion

#pragma region Input CallBacks
void ATetrisController::JumpRecieved()
{
	if(gameIsOn)
	{
		int collisionLimit = GetFirstCollisionY(currentPiece);
		currentPiece.Move(FIntVector2(currentPiece.position.X, collisionLimit));
		SpawnNewPiece();
	}
}
void ATetrisController::RotateRecieved()
{
	if(gameIsOn)
		RotatePiece();
}
void ATetrisController::LeftRecieved()
{
	if(gameIsOn)
	{
		FIntVector2 newPosition = FIntVector2(currentPiece.position.X - 1, currentPiece.position.Y);

		if(CanMove(currentPiece, newPosition))
			currentPiece.Move(newPosition);
	}
}
void ATetrisController::RightRecieved()
{
	if(gameIsOn)
	{
		FIntVector2 newPosition = FIntVector2(currentPiece.position.X + 1, currentPiece.position.Y);

		if(CanMove(currentPiece, newPosition))
			currentPiece.Move(newPosition);
	}
}
#pragma endregion