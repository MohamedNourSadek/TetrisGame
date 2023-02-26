// Fill out your copyright notice in the Description page of Project Settings.

#include "TetrisController.h"


#pragma region Unreal Functions
ATetrisController::ATetrisController()
{
	PrimaryActorTick.bCanEverTick = true;
}
void ATetrisController::BeginPlay()
{
	Super::BeginPlay();
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
	
		if (timeDescrete != lastSecondTicked && !movingAPiece)
		{
			lastSecondTicked = timeDescrete;
			
			int collisionLimit = LastCollision(currentPiece[0]);;
			UE_LOG(LogTemp, Display, TEXT("%d"), collisionLimit);
			
			if (collisionLimit > 20)
			{
				UE_LOG(LogTemp, Display, TEXT("Game is Over"))
				gameIsOn = false;
			}
			else if ((currentPiece)[0]->y > collisionLimit)
			{
				for(ATetrisPiece* piece : currentPiece)
					MovePiece(piece, piece->x, piece->y - 1);
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
}
#pragma endregion

#pragma region Gameplay Functions
int ATetrisController::LastCollision(ATetrisPiece* piece)
{
	if (spawnedPieces.Num() == 1)
		return 1;

	TArray<FIntVector2>* occupied = FindOccupied(piece);
	
	if (occupied->Num() == 0)
	{
		delete occupied;
		return 1;
	}

	for(int y = piece->y - 1; y >= 1; y--)
	{
		for(FIntVector2 bodyPiece : piece->geometry)
		{
			int posX = piece->x + bodyPiece.X;
			int posY = y + bodyPiece.Y;

			for (FIntVector2 point : *occupied)
			{
				if(point.X == posX && point.Y == posY)
				{
					delete occupied;
					return posY + 1;
				}
			}
		}		
	}
	
	delete occupied;
	return 1;
}
void ATetrisController::InitializePiece(ATetrisPiece& piece)
{
	piece.x = spawnPoint->GetActorLocation().X / 100;
	piece.y = spawnPoint->GetActorLocation().Y / 100;
	
	spawnedPieces.Add(&piece);
	currentPiece = *SplitPiece(&piece);
}
void ATetrisController::SpawnNewPiece()
{
	ReorganizePieces();
	int randomPiece = FMath::RandRange(0,pieces.Num() - 1);
	ATetrisPiece* piece = Cast<ATetrisPiece>(GetWorld()->SpawnActor(pieces[randomPiece], &spawnPoint->GetTransform()));
	InitializePiece(*piece);
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

		TArray<FIntVector2>* occupiedPos = FindOccupied(piece);

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
		ATetrisPiece* newPiece = Cast<ATetrisPiece>(GetWorld()->SpawnActor(pieces[0], &spawnPoint->GetTransform()));

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
TArray<FIntVector2>* ATetrisController::FindOccupied(const ATetrisPiece* myPiece)
{
	TArray<FIntVector2>* occupied = new TArray<FIntVector2>();
	
	for (ATetrisPiece* piece : spawnedPieces)
	{
		bool oneOfMyPieces = false;

		for(ATetrisPiece* aPiece : currentPiece)
		{
			if (piece == aPiece)
				oneOfMyPieces = true;
		}
		
		if (!oneOfMyPieces)
		{
			for(FIntVector2 bodyPiece : piece->geometry)
			{
				occupied->Add(FIntVector2(piece->x + bodyPiece.X, piece->y + bodyPiece.Y));
			}
		}
	}

	return occupied;
}
#pragma endregion

#pragma region Input CallBacks
void ATetrisController::JumpRecieved()
{
	if(gameIsOn)
	{
		int collisionLimit = 1;

		for(ATetrisPiece* piece : currentPiece)
		{
			int pieceLimit = LastCollision(piece);

			if(pieceLimit > collisionLimit)
				collisionLimit = pieceLimit;
		}
			
		for(ATetrisPiece* piece : currentPiece)
			MovePiece(piece, piece->x, collisionLimit);

		SpawnNewPiece();
	}
}
void ATetrisController::LeftRecieved()
{
	if(gameIsOn)
	{
		for(ATetrisPiece* piece : currentPiece)
			MovePiece(piece, piece->x + 1, piece->y);
	}
}
void ATetrisController::RightRecieved()
{
	if(gameIsOn)
	{
		for(ATetrisPiece* piece : currentPiece)
			MovePiece(piece, piece->x - 1, piece->y);
	}
}
#pragma endregion