// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class TETRIS_API Debug
{
public:
	Debug();
	~Debug();

	static void Log(const FString* message);
	static void Log(const char* message);
};
