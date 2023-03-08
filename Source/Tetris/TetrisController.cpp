// Fill out your copyright notice in the Description page of Project Settings.

#include "TetrisController.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"


#pragma region Unreal Functions
ATetrisController::ATetrisController()
{
	PrimaryActorTick.bCanEverTick = true;
}
void ATetrisController::InitializeData()
{
	//Adding pieces
	//one sqaure
	PiecePrototype* newPiece = new PiecePrototype();
	newPiece->material = red;
	newPiece->subPieces.Add(FIntVector2(0,0));

	//one horizontal Line
	PiecePrototype* newPiece2 = new PiecePrototype();
	newPiece2->material = green;
	newPiece2->subPieces.Add(FIntVector2(0,0));
	newPiece2->subPieces.Add(FIntVector2(1,0));
	newPiece2->subPieces.Add(FIntVector2(-1,0));

	   //
	//////
	PiecePrototype* newPiece3 = new PiecePrototype();
	newPiece3->material = blue;
	newPiece3->subPieces.Add(FIntVector2(0,0));
	newPiece3->subPieces.Add(FIntVector2(1,0));
	newPiece3->subPieces.Add(FIntVector2(-1,0));
	newPiece3->subPieces.Add(FIntVector2(0,1));

	 //
	//////
	PiecePrototype* newPiece4 = new PiecePrototype();
	newPiece4->material = orange;
	newPiece4->subPieces.Add(FIntVector2(0,0));
	newPiece4->subPieces.Add(FIntVector2(1,0));
	newPiece4->subPieces.Add(FIntVector2(-1,0));
	newPiece4->subPieces.Add(FIntVector2(-1,1));

	prototypePieces.Add(newPiece);
	prototypePieces.Add(newPiece2);
	prototypePieces.Add(newPiece3);
	prototypePieces.Add(newPiece4);
}
void ATetrisController::BeginPlay()
{
	Super::BeginPlay();

	controller = Cast<ATetrisPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	InitializeData();
	SpawnNewPiece();
	gameIsOn =true;
	controller->ChangeUiState(1);
}
void ATetrisController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(gameIsOn)
	{
		timeSinceStartUp += DeltaTime;
		tickEvery = FMath::Clamp(tickEvery - acceleration*DeltaTime, .001, 1000);
		int timeDescrete = static_cast<int>(timeSinceStartUp / tickEvery);
	
		if ((timeDescrete != lastSecondTicked) && (!movingAPiece))
		{
			lastSecondTicked = timeDescrete;
			
			int collisionLimit = GetFirstCollisionY(currentPiece);
			
			if (collisionLimit > 20 && gameIsOn)
			{
				gameIsOn = false;
				controller->ChangeUiState(0);
			}
			else if (currentPiece->position.Y > collisionLimit)
			{
				FIntVector2 newPosition = FIntVector2(currentPiece->position.X, currentPiece->position.Y - 1);
				currentPiece->Move(newPosition);
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

#pragma region Actions
void ATetrisController::SpawnNewPiece()
{
	ReorganizePieces();

	if(currentPiece != nullptr)
	{
		delete currentPiece;
		currentPiece = nullptr;
	}

	PiecePrototype* newPiecePrototype = GetRandomProtoype();
	CompoundPiece* compoundPiece = new CompoundPiece();
	
	compoundPiece->position.X = (spawnPoint->GetActorLocation().X / 100);
	compoundPiece->position.Y = (spawnPoint->GetActorLocation().Y / 100);
	
	for(FIntVector2 newSubPiecePrototype : newPiecePrototype->subPieces)
	{
		ATetrisPiece* piece = Cast<ATetrisPiece>(GetWorld()->SpawnActor(unitPiece, &spawnPoint->GetTransform()));

		piece->FindComponentByClass<UStaticMeshComponent>()->SetMaterial(0,newPiecePrototype->material);
		
		SubPiece* subPiece = new SubPiece();

		subPiece->relativePosition = newSubPiecePrototype;
		subPiece->absPosition = FIntVector2(subPiece->relativePosition.X + compoundPiece->position.X,
											subPiece->relativePosition.Y + compoundPiece->position.Y);
		subPiece->myPiece = piece;

		FVector newLocation 
			(100 * subPiece->absPosition.X,
			 100 * subPiece->absPosition.Y,
				piece->GetActorLocation().Z);

		piece->SetActorLocation(newLocation);

		compoundPiece->subPieces.Add(subPiece);
		spawnedPieces.Add(subPiece);
	}

	currentPiece = compoundPiece;
}
void ATetrisController::RotatePiece()
{
	TArray<FIntVector2> newAbsPositions;
	TArray<FIntVector2> newRelativePositions;
	
	for(SubPiece* subPiece : currentPiece->subPieces)
	{
		FIntVector2 newRelativePos = TransformSpace(&subPiece->relativePosition);
		FIntVector2 newAbsPosition = FIntVector2(currentPiece->position.X + newRelativePos.X,
												 currentPiece->position.Y +newRelativePos.Y);
		newRelativePositions.Add(newRelativePos);
		newAbsPositions.Add(newAbsPosition);
	}

	bool canRotate = true;

	for(FIntVector2 newAbsPos : newAbsPositions)
	{
		if(IsPositionPossible(newAbsPos) == false)
			canRotate = false;
	}

	if(canRotate)
	{
		for(int i = 0; i < newAbsPositions.Num(); i++)
		{
			currentPiece->subPieces[i]->relativePosition = newRelativePositions[i];
			currentPiece->subPieces[i]->absPosition = newAbsPositions[i]; 
			currentPiece->subPieces[i]->Move(newAbsPositions[i]);
		}
	}

}
void ATetrisController::ReorganizePieces()
{
	//finding complete rows
	TArray<int>* compelteRows = GetCompleteRows();

	//destroying and moving pieces
	TArray<SubPiece*> piecesToRemove;
	for(int completeRow : *compelteRows)
	{
		for(SubPiece* subPiece : spawnedPieces)
		{
			if(subPiece->absPosition.Y == completeRow)
				piecesToRemove.Add(subPiece);
		}
	}

	if(compelteRows->Num() == 1)
		totalScore += 1;
	else if(compelteRows->Num() == 2)
		totalScore += 4;
	else if(compelteRows->Num() >= 3)
		totalScore += 8;

	controller->ChangeScore(totalScore);
	
	for(SubPiece* subPiece : piecesToRemove)
	{
		spawnedPieces.Remove(subPiece);
		subPiece->myPiece->Destroy();
		delete subPiece;
	}
	TArray<int> reductionIndices;
	for(int i = 1; i <= 20; i++)
	{
		int numOfCompleteRowsUnderNeathIt = 0;

		for(int completeRow : *compelteRows)
		{
			if(i>completeRow)
			{
				numOfCompleteRowsUnderNeathIt++;
			}
		}

		reductionIndices.Add(numOfCompleteRowsUnderNeathIt);
	}

	for(SubPiece* subPiece : spawnedPieces)
	{
		if(subPiece->absPosition.Y <= 20)
		{
			FIntVector2 newPosition =
				 FIntVector2(
					subPiece->absPosition.X,
					subPiece->absPosition.Y - reductionIndices[subPiece->absPosition.Y - 1]);
		
			subPiece->Move(newPosition);	
		}
	}
	
	delete compelteRows;
}
#pragma endregion

#pragma region Computation Functions
FIntVector2 ATetrisController::TransformSpace(FIntVector2* input)
{
	return FIntVector2( -input->Y ,input->X);
}
PiecePrototype* ATetrisController::GetRandomProtoype()
{
	int randomPiece = FMath::RandRange(0,prototypePieces.Num() - 1);
	PiecePrototype* newPiecePrototype = prototypePieces[randomPiece];
	
	return newPiecePrototype;
}
TArray<FIntVector2>* ATetrisController::FindOccupied()
{
	TArray<FIntVector2>* occupied = new TArray<FIntVector2>();
	
	for (SubPiece* subPiece : spawnedPieces)
	{
		bool oneOfMyPieces = false;

		for(SubPiece* mySubPiece : currentPiece->subPieces)
		{
			if (mySubPiece->absPosition == subPiece->absPosition)
				oneOfMyPieces = true;
		}
		
		if (!oneOfMyPieces)
		{
			occupied->Add(subPiece->absPosition);
		}
	}

	return occupied;
}
TArray<int>* ATetrisController::GetCompleteRows()
{
	TArray<int>* compelteRows = new TArray<int>();
	
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
			compelteRows->Add(i);
	}

	return compelteRows;
}
int ATetrisController::GetFirstCollisionY(CompoundPiece* compoundPiece)
{
	TArray<FIntVector2>* occupied = FindOccupied();
	
	int startY = 20;

	for(SubPiece* subPiece : compoundPiece->subPieces)
	{
		if(subPiece->absPosition.Y < startY)
			startY = subPiece->absPosition.Y;
	}

	for(int y = startY; y >= 1; y--)
	{
		for(SubPiece* subPiece : compoundPiece->subPieces)
		{
			int newYPos = (y + subPiece->relativePosition.Y);
			
			for (FIntVector2 point : *occupied)
			{
				if(subPiece->absPosition.X == point.X && (newYPos == point.Y))
				{
					delete occupied;
					return y + 1;
				}
			}

			if(newYPos < 1)
			{
				delete occupied;
				return y + 1;
			}
		}
	}

	
	
	delete occupied;
	return 1;
}
bool ATetrisController::CanMove(CompoundPiece* compundPiece, FIntVector2 newPosition)
{
	if(movingAPiece != true)
	{
		bool reachedLeft = false;
		bool reachedRight = false;
		bool newPosOccupied = false;

		TArray<FIntVector2>* occupiedPos = FindOccupied();

		for(SubPiece* subPiece: compundPiece->subPieces)
		{
			FIntVector2 newSubPiecePosition = FIntVector2(newPosition.X + subPiece->relativePosition.X,
														newPosition.Y + subPiece->relativePosition.Y);
			if(newSubPiecePosition.X > 10)
			{
				reachedLeft = true;
			}
			else if(newSubPiecePosition.X < 1)
			{
				reachedRight = true;
			}

			for (FIntVector2 occupied : *occupiedPos)
			{
				if((newPosition.X + subPiece->relativePosition.X == occupied.X) &&
				   (newPosition.Y + subPiece->relativePosition.Y == occupied.Y))
				{
					newPosOccupied = true;
				}
			}
		}

		delete occupiedPos;
		
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
bool ATetrisController::IsPositionPossible(FIntVector2 newPosition)
{
	if(movingAPiece != true)
	{
		bool reachedLeft = false;
		bool reachedRight = false;
		bool newPosOccupied = false;

		TArray<FIntVector2>* occupiedPos = FindOccupied();

		if(newPosition.X > 10)
		{
			reachedLeft = true;
		}
		else if(newPosition.X < 1)
		{
			reachedRight = true;
		}

		for (FIntVector2 occupied : *occupiedPos)
		{
			if((newPosition.X == occupied.X) && (newPosition.Y == occupied.Y))
				newPosOccupied = true;
		}

		delete occupiedPos;

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
#pragma endregion 

#pragma endregion

#pragma region Input CallBacks
void ATetrisController::JumpRecieved()
{
	if(gameIsOn)
	{
		int collisionLimit = GetFirstCollisionY(currentPiece);
		currentPiece->Move(FIntVector2(currentPiece->position.X, collisionLimit));
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
		FIntVector2 newPosition = FIntVector2(currentPiece->position.X + 1, currentPiece->position.Y);

		if(CanMove(currentPiece, newPosition))
			currentPiece->Move(newPosition);
	}
}
void ATetrisController::RightRecieved()
{
	if(gameIsOn)
	{
		FIntVector2 newPosition = FIntVector2(currentPiece->position.X - 1, currentPiece->position.Y);

		if(CanMove(currentPiece, newPosition))
			currentPiece->Move(newPosition);
	}
}
#pragma endregion