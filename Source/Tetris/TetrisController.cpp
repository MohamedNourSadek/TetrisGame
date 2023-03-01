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
			
			int collisionLimit = LastCollision(&currentPiece);
			
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
		
		for(ATetrisPiece* piece : spawnedPieces)
		{
			if(piece != nullptr)
			{
				if(piece->y == i)
					totalSum += 1;
			}
		}

		if(totalSum == 10)
			compelteRows.Add(i);
	}

	//destroying and moving pieces
	for(int completeRow : compelteRows)
	{
		TArray<ATetrisPiece*> piecesToRemove;
		
		for(ATetrisPiece* piece : spawnedPieces)
		{
			if(piece->y == completeRow)
				piecesToRemove.Add(piece);

		}

		for(ATetrisPiece* piece : piecesToRemove)
		{
			spawnedPieces.Remove(piece);
			piece->Destroy();
		}

		for(ATetrisPiece* piece : spawnedPieces)
		{
			if(piece->y > completeRow)
				MovePiece(piece, piece->x, piece->y - 1);
		}
		
	}
}
void ATetrisController::MovePiece(ATetrisPiece* piece,int newX,int newY)
{
	if(movingAPiece != true)
	{
		movingAPiece = true;

		bool reachedLeft = false;
		for(FIntVector2 bodyPiece: piece->geometry)
		{
			int newPos = newX + bodyPiece.X;

			if(newPos > 10)
			{
				reachedLeft = true;
				break;
			}
		
		}
		bool reachedRight = (newX < 1);
		bool newPosOccupied = false;

		TArray<FIntVector2>* occupiedPos = FindOccupied();
		TArray<FIntVector2> conflicted;
		
		for(FIntVector2 bodyPiece: piece->geometry)
			for(FIntVector2 v : *occupiedPos)
				if(v.X == newX + bodyPiece.X && v.Y == newY + bodyPiece.Y)
				{
					conflicted.Add(v);					
					newPosOccupied = true;
				}

		delete occupiedPos;
	
		if (!reachedLeft && !reachedRight && !newPosOccupied)
		{
			piece->MovePiece(newX,newY);
		}

		movingAPiece = false;
	}
}
void ATetrisController::MovePiece(TArray<ATetrisPiece*> piece,int newX,int newY)
{
	if(movingAPiece != true)
	{
		movingAPiece = true;

		bool reachedLeft = false;
		bool reachedRight = false;
		
		TArray<FIntVector2> delta;

		for(int i = 0; i < piece.Num(); i++)
		{
			int deltaX = piece[i]->x - piece[0]->x;
			int deltaY = piece[i]->y - piece[0]->y;

			delta.Add(FIntVector2(deltaX,deltaY));
			
			int newPos = newX + deltaX;
			
			if(newPos > 10)
				reachedLeft = true;
		}
		for(int i = 0; i < piece.Num(); i++)
		{
  			int newPos = newX + delta[i].X;

			if(newPos < 1)
				reachedRight = true;
		}
		
		bool newPosOccupied = false;

		TArray<FIntVector2>* occupiedPos = FindOccupied();

		TArray<FIntVector2> conflicted;
		
		for(int i = 0; i < piece.Num(); i++)
		{
			for(FIntVector2 v : *occupiedPos)
			{
				if(v.X == newX + delta[i].X && v.Y == newY + delta[i].Y)
				{
					conflicted.Add(v);					
					newPosOccupied = true;
				}
			}
		}

		delete occupiedPos;
	
		if (!reachedLeft && !reachedRight && !newPosOccupied)
		{
			for(int i = 0; i < piece.Num(); i++)
			{
				piece[i]->MovePiece(newX + delta[i].X,newY + delta[i].Y);
			}
		}

		movingAPiece = false;
	}
}
void ATetrisController::RotatePiece(TArray<ATetrisPiece*>& piece)
{
	UE_LOG(LogTemp, Display, TEXT("Rotating"));

	FIntVector2 origin (20,20);

	for(ATetrisPiece* subPiece : piece)
		if(subPiece->x < origin.X)
			origin.X = subPiece->x;

	for(ATetrisPiece* subPiece : piece)
		if(subPiece->y < origin.Y)
			origin.Y = subPiece->y;

	TArray<FIntVector2> newPositions;
	
	for(ATetrisPiece* subPiece : piece)
	{
		FIntVector2 relativePos = FIntVector2(subPiece->x - origin.X, subPiece->y - origin.Y);
		FIntVector2 newRelativePos = rotationTransform.FindRef(relativePos);
		FIntVector2 newPosition = FIntVector2(origin.X + newRelativePos.X, origin.Y + newRelativePos.Y);

		newPositions.Add(newPosition);
	}

	for(FIntVector2 v : newPositions)
	{
		if(v.X > 10 || v.X < 1)
		{
			return;
		}
	}

	for(int i = 0; i < newPositions.Num(); i++)
		MovePiece(piece[i], newPositions[i].X, newPositions[i].Y);
}
TArray<ATetrisPiece*>* ATetrisController::SplitPiece(ATetrisPiece* piece)
{
	TArray<ATetrisPiece*>* splitPiece = new TArray<ATetrisPiece*>();
	
	spawnedPieces.Remove(piece);
	
	TArray<UActorComponent*> children;
	piece->GetComponents(children);

	UMaterialInterface* material = nullptr;
	
	for(auto child : children)
	{
		if(child->GetName() == "Cube")
		{
			UStaticMeshComponent* body = piece->FindComponentByClass<UStaticMeshComponent>();
			material =  body->GetMaterial(0);
		}
	}
	
	for(FIntVector2 bodyPiece: piece->geometry)
	{
		ATetrisPiece* newPiece = Cast<ATetrisPiece>(GetWorld()->SpawnActor(unitPiece, &spawnPoint->GetTransform()));

		MovePiece(
			newPiece,
			piece->x + bodyPiece.X,
			piece->y + bodyPiece.Y);

		splitPiece->Add(newPiece);
		
		spawnedPieces.Add(newPiece);

		TArray<UActorComponent*> newChildren;
		newPiece->GetComponents(newChildren);

		for(auto child : newChildren)
		{
			if(child->GetName() == "Cube")
			{
				UStaticMeshComponent* body = newPiece->FindComponentByClass<UStaticMeshComponent>();

				if(material != nullptr)
					body->SetMaterial(0,material);
			}
		}

	}

	piece->Destroy();
	return splitPiece;
}
#pragma endregion

#pragma region Input CallBacks
void ATetrisController::JumpRecieved()
{
	if(gameIsOn)
	{
		int collisionLimit = LastCollision(&currentPiece);
		
		UE_LOG(LogTemp, Display, TEXT("%d"), collisionLimit);

		FIntVector2 origin (20,20);

		for(ATetrisPiece* subPiece : currentPiece)
			if(subPiece->x < origin.X)
				origin.X = subPiece->x;

		for(ATetrisPiece* subPiece : currentPiece)
			if(subPiece->y < origin.Y)
				origin.Y = subPiece->y;

		TArray<int> deltaYs;
		
		for(ATetrisPiece* piece : currentPiece)
		{
			int deltaY = deltaY = piece->y - origin.Y;
			deltaYs.Add(deltaY);
		}

		for(int i = 0; i < deltaYs.Num(); i++)
			MovePiece(currentPiece[i], currentPiece[i]->x, collisionLimit + deltaYs[i]);


		SpawnNewPiece();
	}
}
void ATetrisController::RotateRecieved()
{
	if(gameIsOn)
		RotatePiece(currentPiece);
}
void ATetrisController::LeftRecieved()
{
	if(gameIsOn)
		MovePiece(currentPiece, currentPiece[0]->x + 1, currentPiece[0]->y);
}
void ATetrisController::RightRecieved()
{
	if(gameIsOn)
		MovePiece(currentPiece, currentPiece[0]->x - 1, currentPiece[0]->y);
}
#pragma endregion