// Fill out your copyright notice in the Description page of Project Settings.


#include "Debug.h"

Debug::Debug()
{
}

void Debug::Log(const FString* message)
{
	UE_LOG(LogTemp, Display, TEXT("%s"), message);
}

void Debug::Log(const char* message)
{
	UE_LOG(LogTemp, Display, TEXT("%s"), &message);
}

Debug::~Debug()
{
}
