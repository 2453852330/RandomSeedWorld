// Fill out your copyright notice in the Description page of Project Settings.


#include "MatchingCube2D_Smooth.h"

#include "CubeTable.h"
#include "ProceduralMeshComponent.h"


// Sets default values
AMatchingCube2D_Smooth::AMatchingCube2D_Smooth()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComponent"));
	RootComponent = ProceduralMeshComponent;
}

// Called when the game starts or when spawned
void AMatchingCube2D_Smooth::BeginPlay()
{
	Super::BeginPlay();

	double start_time = FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64());
	
	CF_Init();

	CF_FillAndCreate();

	UE_LOG(LogTemp, Warning, TEXT("[MatchingCube2D->BeginPlay]Cost %f ms"),FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64()) - start_time);
}

int32 AMatchingCube2D_Smooth::CF_GetIndexByXY(int32 X, int32 Y) const
{
	return Y * ChunkSize.X + X;
}

int32 AMatchingCube2D_Smooth::CF_GetIndexByXY(FIntVector2 InPos) const
{
	return InPos.Y * ChunkSize.X + InPos.X;
}

void AMatchingCube2D_Smooth::CF_Init()
{
	Map.Reset();
	Map.AddZeroed(ChunkSize.X * ChunkSize.Y);
	
	Map_Value.Reset();
	Map_Value.AddZeroed(ChunkSize.X * ChunkSize.Y);
	
	Buffer_Vertex.Reset();
	Buffer_Vertex.Reserve(ChunkSize.X * ChunkSize.Y * 10);

	Buffer_Triangle.Reset();
	Buffer_Triangle.Reserve(ChunkSize.X * ChunkSize.Y * 10);

	// general
	NoiseLite.SetNoiseType(FastNoiseLiteConfig.GetNoiseType());
	NoiseLite.SetFrequency(FastNoiseLiteConfig.Frequency);
	NoiseLite.SetSeed(bRandomSeed ? CF_GetRandomSeed() : FastNoiseLiteConfig.Seed);
	NoiseLite.SetRotationType3D(FastNoiseLiteConfig.GetRotationType3D());

	// Fractal
	NoiseLite.SetFractalType(FastNoiseLiteConfig.GetFractalType());
	NoiseLite.SetFractalOctaves(FastNoiseLiteConfig.FractalOctaves);
	NoiseLite.SetFractalLacunarity(FastNoiseLiteConfig.FractalLacunarity);
	NoiseLite.SetFractalGain(FastNoiseLiteConfig.FractalGain);
	NoiseLite.SetFractalWeightedStrength(FastNoiseLiteConfig.FractalWeightedStrength);
	NoiseLite.SetFractalPingPongStrength(FastNoiseLiteConfig.FractalPingPongStrength);

	// Cellular
	NoiseLite.SetCellularDistanceFunction(FastNoiseLiteConfig.GetCellularDistanceFunction());
	NoiseLite.SetCellularReturnType(FastNoiseLiteConfig.GetCellularReturnType());
	NoiseLite.SetCellularJitter(FastNoiseLiteConfig.CellularJitter);

	// Domain Warp
	NoiseLite.SetDomainWarpType(FastNoiseLiteConfig.GetDomainType());
	NoiseLite.SetDomainWarpAmp(FastNoiseLiteConfig.DomainWarpAmp);
}

void AMatchingCube2D_Smooth::CF_FillAndCreate()
{
	for (int32 i = 0; i < ChunkSize.X; ++i)
	{
		for (int32 j = 0; j < ChunkSize.Y; ++j)
		{
			float Value = NoiseLite.GetNoise<float>(float(i),float(j));
			int32 Index = CF_GetIndexByXY(i,j);
			Map_Value[Index] = Value;
			Map[Index] = Value >= 0 ? 1 : 0 ;
		}
	}

	for (int32 i = 0; i < ChunkSize.X - 1; ++i)
	{
		for (int32 j = 0; j < ChunkSize.Y - 1; ++j)
		{
			CF_CheckCubeToFillBuffer(i,j);
		}
	}


	UE_LOG(LogTemp, Warning, TEXT("[MatchingCube2D->CF_FillAndCreate]Find %d Vertex + %d Triangle"),Buffer_Vertex.Num(),Buffer_Triangle.Num());

	ProceduralMeshComponent->CreateMeshSection_LinearColor(0,Buffer_Vertex,Buffer_Triangle,{},{},{},{},true);
}

void AMatchingCube2D_Smooth::CF_CheckCubeToFillBuffer(int32 X, int32 Y)
{
	// the x,y is the zero point;

	// get the 4 point
	FIntVector2 P0 = FIntVector2(X,Y);
	FIntVector2 P1 = FIntVector2(X + 1,Y);
	FIntVector2 P2 = FIntVector2(X + 1,Y + 1);
	FIntVector2 P3 = FIntVector2(X,Y + 1);
	

	FVector RP0 = CF_GetRealPosByXY(P0);
	FVector RP1 = CF_GetRealPosByXY(P1);
	FVector RP2 = CF_GetRealPosByXY(P2);
	FVector RP3 = CF_GetRealPosByXY(P3);

	// 中点修改为插值;
	float l4 = CF_GetLerpValueFromTwoPos(P0,P1);
	float l5 = CF_GetLerpValueFromTwoPos(P1,P2);
	float l6 = CF_GetLerpValueFromTwoPos(P2,P3);
	float l7 = CF_GetLerpValueFromTwoPos(P3,P0);
	
	// FVector RP4 = CF_GetRealPosByXY( (float)X + l4, ((float)Y) );
	// FVector RP5 = CF_GetRealPosByXY( ((float)X + 1.f) , ((float)Y + l5));
	// FVector RP6 = CF_GetRealPosByXY( ((float)X + l6), ((float)Y + 1.f));
	// FVector RP7 = CF_GetRealPosByXY( ((float)X ), ((float)Y + l7));

	FVector RP4 = CF_GetRealPosByXY( X + l4		,			Y);
	FVector RP5 = CF_GetRealPosByXY( X + 1.f	,			Y + l5);
	FVector RP6 = CF_GetRealPosByXY( X + (1 - l6 )		,			Y + 1.f);
	FVector RP7 = CF_GetRealPosByXY( X			,			Y + (1 - l7 ));
	
	TArray<FVector> PosList; PosList.Reserve(8);
	
	PosList.Add(RP0); PosList.Add(RP1); PosList.Add(RP2); PosList.Add(RP3);
	PosList.Add(RP4); PosList.Add(RP5); PosList.Add(RP6); PosList.Add(RP7);

	// draw debug point start
	{
		// for (int32 i = 0; i < 4 ; ++i)
		// {
		// 	int32 MapIndex = 0;
		// 	switch (i)
		// 	{
		// 	case 0:
		// 		{
		// 			MapIndex = Map[CF_GetIndexByXY(P0)];
		// 			break;
		// 		}
		// 	case 1:
		// 		{
		// 			MapIndex = Map[CF_GetIndexByXY(P1)];
		// 			break;
		// 		}
		// 	case 2:
		// 		{
		// 			MapIndex = Map[CF_GetIndexByXY(P2)];
		// 			break;
		// 		}
		// 	case 3:
		// 		{
		// 			MapIndex = Map[CF_GetIndexByXY(P3)];
		// 			break;
		// 		}
		// 	}
		// 	
		// 	DrawDebugPoint(GetWorld(),PosList[i],5.f,MapIndex==0 ? FColor::Yellow : FColor::Green,true);
		// }
		// for (int32 i = 4; i < 8 ; ++i)
		// {
		// 	DrawDebugPoint(GetWorld(),PosList[i],5.f,FColor::Red,true);
		// }
	}
	// draw debug point end

	
	int32 IndexValue =  (Map[CF_GetIndexByXY(P0)] == 1) * 8 +
						(Map[CF_GetIndexByXY(P1)] == 1) * 4 +
						(Map[CF_GetIndexByXY(P2)] == 1) * 2 +
						(Map[CF_GetIndexByXY(P3)] == 1) * 1 ;
	
	// int32 IndexValue = 0;
	// if (Map[CF_GetIndexByXY(P0)] == 1)
	// {
	// 	IndexValue += 8;
	// 	DrawDebugPoint(GetWorld(),RP0,3.f,FColor::Green,true);
	// }
	// if (Map[CF_GetIndexByXY(P1)] == 1)
	// {
	// 	IndexValue += 4;
	// 	DrawDebugPoint(GetWorld(),RP1,3.f,FColor::Green,true);
	// }
	// if (Map[CF_GetIndexByXY(P2)] == 1)
	// {
	// 	IndexValue += 2;
	// 	DrawDebugPoint(GetWorld(),RP2,3.f,FColor::Green,true);
	// }
	// if (Map[CF_GetIndexByXY(P3)] == 1)
	// {
	// 	IndexValue += 1;
	// 	DrawDebugPoint(GetWorld(),RP3,3.f,FColor::Green,true);
	// }

	// UE_LOG(LogTemp, Warning, TEXT("[MatchingCube2D->CF_CheckCubeToFillBuffer]Cube Table Index : %d"),IndexValue);
	
	CUBE_TABLE_2D::FCubeTable CubeTable = CUBE_TABLE_2D::CubeTable2D_Triangle[IndexValue];

	// UE_LOG(LogTemp, Warning, TEXT("[MatchingCube2D->CF_CheckCubeToFillBuffer]Find %d Index , %d Triangle"),CubeTable.Index.Num(),CubeTable.Index.Num() / 3);

	// 全量点 / 速度更快 100x100 = 25~35 ms
	for (int32 i = 0; i < CubeTable.Index.Num() / 3; ++i)
	{
		Buffer_Triangle.Add(Buffer_Vertex.Add(PosList[CubeTable.Index[i*3]]));
		Buffer_Triangle.Add(Buffer_Vertex.Add(PosList[CubeTable.Index[i*3+1]]));
		Buffer_Triangle.Add(Buffer_Vertex.Add(PosList[CubeTable.Index[i*3+2]]));
	}

	
	// 重用点 / 速度慢 100x100 = 80~100 ms
	// for (int32 i = 0; i < CubeTable.Index.Num() / 3; ++i)
	// {
	// 	CF_AddVertex(PosList[CubeTable.Index[i*3]]);
	// 	CF_AddVertex(PosList[CubeTable.Index[i*3+1]]);
	// 	CF_AddVertex(PosList[CubeTable.Index[i*3+2]]);
	// }

}

FVector AMatchingCube2D_Smooth::CF_GetRealPosByXY(float X, float Y)
{
	return  FVector(X,Y,0) * CubeSize;
}

FVector AMatchingCube2D_Smooth::CF_GetRealPosByXY(FIntVector2 InPos)
{
	return FVector(InPos.X,InPos.Y,0) * CubeSize;
}

void AMatchingCube2D_Smooth::CF_AddVertex(FVector InPos)
{
	int32 FindIndex = Buffer_Vertex.Find(InPos);
	if (FindIndex != INDEX_NONE)
	{
		Buffer_Triangle.Add(FindIndex);
	}
	else
	{
		FindIndex = Buffer_Vertex.Add(InPos);
		Buffer_Triangle.Add(FindIndex);
	}
}

int32 AMatchingCube2D_Smooth::CF_GetRandomSeed() const
{
	return FMath::Rand();
}

float AMatchingCube2D_Smooth::CF_GetLerpValueFromTwoPos(FIntVector2 A, FIntVector2 B) const
{
	float a = Map_Value[CF_GetIndexByXY(A)];
	float b = Map_Value[CF_GetIndexByXY(B)];
	// to avoid too small value
	if ( FMath::Abs(a - b) <= TooSmallValueCheck)
	{
		return 0.f;
	}
	
	// if the two pos's value >= 0.f , also select center pos
	if ( (a >= 0.f && b >= 0.f)  || (a < 0.f && b < 0.f))
	{
		return 0.5f;
	}
	// if two pos's +/- is not same , need find 0 pos;
	// a = -0.2 | b = 0.5 ->   0.2 / 0.7 = 0.285
	// 1 = 0.8  | b = -0.3   ->  -0.8 / -1.1 =  0.72
	return (0.f - a ) / (b - a);
}
