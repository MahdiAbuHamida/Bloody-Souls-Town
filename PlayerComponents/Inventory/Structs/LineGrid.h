// M&M Games 2021 - Free University Project

#pragma once

#include "LineGrid.generated.h"

USTRUCT(BlueprintType)
struct FLineGrid
{
	GENERATED_USTRUCT_BODY()

	FORCEINLINE FLineGrid();

	explicit FORCEINLINE FLineGrid(FVector2D InStart, FVector2D InEnd);

	UPROPERTY(BlueprintReadWrite)
	FVector2D Start;

	UPROPERTY(BlueprintReadWrite)
	FVector2D End;
};

FORCEINLINE FLineGrid::FLineGrid()
{
	Start = FVector2D(0.f, 0.f);
	End = FVector2D(0.f, 0.f);
}

FORCEINLINE FLineGrid::FLineGrid(FVector2D InStart, FVector2D InEnd)
{
	Start = InStart;
	End = InEnd;
}