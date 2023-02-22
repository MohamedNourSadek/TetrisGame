// Fill out your copyright notice in the Description page of Project Settings.

#include "TetrisController.h"


struct Vector2
{
	int x = 1;
	int y = 1;

	Vector2(int X, int Y)
	{
		x = X;
		y = Y;
	}
};

#pragma region Unreal Functions
ATetrisController::ATetrisController()
{
	PrimaryActorTick.bCanEverTick = true;
}
void ATetrisController::BeginPlay()
{
	Super::BeginPlay();
	SpawnNewPiece();
}
void ATetrisController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	timeSinceStartUp += DeltaTime;
	
	int timeDescrete = (int)(timeSinceStartUp/ tickEvery);

	if (timeDescrete != lastSecondTicked)
	{
		lastSecondTicked = timeDescrete;

		int collisionLimit = LastCollision();

		if (collisionLimit > 20)
			UE_LOG(LogTemp, Display, TEXT("Game is Over"))
		else if (currentPiece->y > collisionLimit)
			currentPiece->MovePiece(currentPiece->x, currentPiece->y - 1);
		else
			SpawnNewPiece();
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
int ATetrisController::LastCollision()
{
	if (spawnedPieces.Num() == 1)
		return 1;

	TArray<Vector2> occupied;
	for (ATetrisPiece* piece : spawnedPieces)
	{
		if (piece != currentPiece)
		{
			occupied.Add(Vector2(piece->x, piece->y));
			occupied.Add(Vector2(piece->x + 1, piece->y));
			occupied.Add(Vector2(piece->x - 1, piece->y));
		}
	}

	if (occupied.Num() == 0)
		return 1;

	int startPoint = currentPiece->x - 1;
	int endPoint = currentPiece->x + 1;

	for (int y = 20; y >= 1; y--)
	{
		bool isHorizontalLineOccuipied = false;

		for (int x = startPoint; x <= endPoint; x++)
		{
			for (Vector2 point : occupied)
			{
				if ((point.x == x) && (point.y == y))
				{
					isHorizontalLineOccuipied = true;
					break;
				}
			}
		}

		if(isHorizontalLineOccuipied)
			return y + 1;
	}

	return 1;
}
void ATetrisController::InitializePiece(ATetrisPiece& piece)
{
	piece.x = spawnPoint->GetActorLocation().X / 100;
	piece.y = spawnPoint->GetActorLocation().Y / 100;

	spawnedPieces.Add(&piece);
	currentPiece = &piece;
}
void ATetrisController::SpawnNewPiece()
{
	ATetrisPiece* piece = Cast<ATetrisPiece>(GetWorld()->SpawnActor(pieceBP, &spawnPoint->GetTransform()));
	InitializePiece(*piece);
}
void ATetrisController::JumpRecieved()
{
	UE_LOG(LogTemp, Display, TEXT("Jump"));
}
void ATetrisController::LeftRecieved()
{
	currentPiece->MovePiece(currentPiece->x + 1, currentPiece->y);
}
void ATetrisController::RightRecieved()
{
	currentPiece->MovePiece(currentPiece->x - 1, currentPiece->y);
}
#pragma endregion