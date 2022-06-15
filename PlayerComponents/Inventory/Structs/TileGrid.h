// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TileGrid.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FTileGrid
{
	GENERATED_USTRUCT_BODY()

	FORCEINLINE FTileGrid();

	explicit FORCEINLINE FTileGrid(int32 InX, int32 InY);
	
	UPROPERTY(BlueprintReadWrite)
	int32 X;

	UPROPERTY(BlueprintReadWrite)
	int32 Y;
};

FORCEINLINE FTileGrid::FTileGrid()
{
	X = 0;
	Y = 0;
}

FORCEINLINE FTileGrid::FTileGrid(const int32 InX, const int32 InY)
{
	X = InX;
	Y = InY;
}