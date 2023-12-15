// Fill out your copyright notice in the Description page of Project Settings.


#include "MatchingCube3D_Smooth.h"

#include "CubeTable.h"
#include "ProceduralMeshComponent.h"

// Sets default values
AMatchingCube3D_Smooth::AMatchingCube3D_Smooth()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComp"));
	RootComponent = ProceduralMeshComponent;
}

// Called when the game starts or when spawned
void AMatchingCube3D_Smooth::BeginPlay()
{
	Super::BeginPlay();

	double start_time = FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64());

	CF_Init();
	CF_FillAndCreate();
	
	UE_LOG(LogTemp, Warning, TEXT("[MatchingCube2D->BeginPlay]Cost %f ms"),FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64()) - start_time);

}

int32 AMatchingCube3D_Smooth::CF_GetIndexByXYZ(int32 X, int32 Y, int32 Z) const
{
	return Z * ChunkSize.X * ChunkSize.Y + Y * ChunkSize.X + X;
}

int32 AMatchingCube3D_Smooth::CF_GetIndexByXYZ(FIntVector InPos) const
{
	return InPos.Z * ChunkSize.X * ChunkSize.Y + InPos.Y * ChunkSize.X + InPos.X;

}

void AMatchingCube3D_Smooth::CF_Init()
{
	Map.Reset();
	Map.AddZeroed(ChunkSize.X * ChunkSize.Y * ChunkSize.Z);

	Map_Value.Reset();
	Map_Value.AddZeroed(ChunkSize.X * ChunkSize.Y * ChunkSize.Z);
	
	Buffer_Vertex.Reset();
	Buffer_Vertex.Reserve(ChunkSize.X * ChunkSize.Y * ChunkSize.Z * 10);

	Buffer_Triangle.Reset();
	Buffer_Triangle.Reserve(ChunkSize.X * ChunkSize.Y * ChunkSize.Z * 10);

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

void AMatchingCube3D_Smooth::CF_FillAndCreate()
{
	FVector Center = FVector(ChunkSize) / 2.f;
	
	
	for (int32 i = 0; i < ChunkSize.X; ++i)
	{
		for (int32 j = 0; j < ChunkSize.Y; ++j)
		{
			for (int32 k = 0; k < ChunkSize.Z; ++k)
			{
				// use fast noise
				// float value = NoiseLite.GetNoise<float>(float(i),float(j),float(k));
				// Map[CF_GetIndexByXYZ(i,j,k)] = value >= 0 ? 1 : 0;
				
				if (bUseFastNoise)
				{
					float value = NoiseLite.GetNoise<float>(float(i),float(j),float(k));
					int32 GetIndex = CF_GetIndexByXYZ(i,j,k);
					Map_Value[GetIndex] = value;
					Map[GetIndex] = value >= 0 ? 1 : 0;
				}
				else
				{
					Map[CF_GetIndexByXYZ(i,j,k)] = FVector::DistSquared(FVector(i,j,k),Center) <= Radius ? 1 : 0;
				}
			}
		}
	}

	for (int32 i = 0; i < ChunkSize.X - 1; ++i)
	{
		for (int32 j = 0; j < ChunkSize.Y - 1; ++j)
		{
			for (int32 k = 0; k < ChunkSize.Z - 1; ++k)
			{
				CF_CheckCubeAndFillData(i,j,k);
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[MatchingCube2D->CF_FillAndCreate]Find %d Vertex + %d Triangle"),Buffer_Vertex.Num(),Buffer_Triangle.Num());

	ProceduralMeshComponent->CreateMeshSection_LinearColor(0,Buffer_Vertex,Buffer_Triangle,{},{},{},{},true);

}

int32 AMatchingCube3D_Smooth::CF_GetRandomSeed() const
{
	return FMath::Rand();
}

void AMatchingCube3D_Smooth::CF_CheckCubeAndFillData(int32 X, int32 Y, int32 Z)
{
	TArray<FVector> PointList;
	PointList.Reserve(8);
	
	// this is all vertex
	for (int32 i = 0; i < CUBE_TABLE_3D::Vertex.Num(); ++i)
	{
		PointList.Add(FVector(X,Y,Z) + CUBE_TABLE_3D::Vertex[i]);
	}

	int32 EdgeIndex =	CF_CheckPointIsValid(PointList[0]) * 1 +
						CF_CheckPointIsValid(PointList[1]) * 2 +
						CF_CheckPointIsValid(PointList[2]) * 4 +
						CF_CheckPointIsValid(PointList[3]) * 8 +
						CF_CheckPointIsValid(PointList[4]) * 16 +
						CF_CheckPointIsValid(PointList[5]) * 32 +
						CF_CheckPointIsValid(PointList[6]) * 64 +
						CF_CheckPointIsValid(PointList[7]) * 128;
	
	TArray<FVector> RealPointList;
	RealPointList.Reserve(CUBE_TABLE_3D::Edge.Num());

	
	
	// RealPointList.Add(CF_GetRealLocation( X + 0.5f, Y , Z));
	// RealPointList.Add(CF_GetRealLocation( X + 1.f, Y + 0.5f , Z));
	// RealPointList.Add(CF_GetRealLocation( X + 0.5f, Y + 1.f, Z));
	// RealPointList.Add(CF_GetRealLocation( X , Y + 0.5f , Z));
	//
	// RealPointList.Add(CF_GetRealLocation( X + 0.5f, Y , Z + 1.f));
	// RealPointList.Add(CF_GetRealLocation( X + 1.f, Y + 0.5f , Z + 1.f));
	// RealPointList.Add(CF_GetRealLocation( X + 0.5f, Y + 1.f, Z + 1.f));
	// RealPointList.Add(CF_GetRealLocation( X , Y + 0.5f , Z + 1.f));
	//
	// RealPointList.Add(CF_GetRealLocation( X , Y , Z + 0.5f));
	// RealPointList.Add(CF_GetRealLocation( X + 1.f , Y , Z + 0.5f));
	// RealPointList.Add(CF_GetRealLocation( X + 1.f , Y + 1.f , Z + 0.5f));
	// RealPointList.Add(CF_GetRealLocation( X , Y + 1.f, Z + 0.5f));

	float l1 = CF_GetLerpValueFromTwoPos(FIntVector(PointList[0]),FIntVector(PointList[1]));
	float l2 = CF_GetLerpValueFromTwoPos(FIntVector(PointList[1]),FIntVector(PointList[2]));
	float l3 = CF_GetLerpValueFromTwoPos(FIntVector(PointList[2]),FIntVector(PointList[3]));
	float l4 = CF_GetLerpValueFromTwoPos(FIntVector(PointList[3]),FIntVector(PointList[0]));
	
	float l5 = CF_GetLerpValueFromTwoPos(FIntVector(PointList[4]),FIntVector(PointList[5]));
	float l6 = CF_GetLerpValueFromTwoPos(FIntVector(PointList[5]),FIntVector(PointList[6]));
	float l7 = CF_GetLerpValueFromTwoPos(FIntVector(PointList[6]),FIntVector(PointList[7]));
	float l8 = CF_GetLerpValueFromTwoPos(FIntVector(PointList[7]),FIntVector(PointList[4]));
	
	float l9 = CF_GetLerpValueFromTwoPos(FIntVector(PointList[0]),FIntVector(PointList[4]));
	float l10= CF_GetLerpValueFromTwoPos(FIntVector(PointList[1]),FIntVector(PointList[5]));
	float l11 = CF_GetLerpValueFromTwoPos(FIntVector(PointList[2]),FIntVector(PointList[6]));
	float l12 = CF_GetLerpValueFromTwoPos(FIntVector(PointList[3]),FIntVector(PointList[7]));


	RealPointList.Add(CF_GetRealLocation( X + l1, Y , Z));
	RealPointList.Add(CF_GetRealLocation( X + 1.f, Y + l2 , Z));
	RealPointList.Add(CF_GetRealLocation( X + (1.f - l3 ), Y + 1.f, Z));
	RealPointList.Add(CF_GetRealLocation( X , Y + (1.f - l4 ) , Z));
	
	RealPointList.Add(CF_GetRealLocation( X + l5, Y , Z + 1.f));
	RealPointList.Add(CF_GetRealLocation( X + 1.f, Y + l6 , Z + 1.f));
	RealPointList.Add(CF_GetRealLocation( X + (1.f - l7), Y + 1.f, Z + 1.f));
	RealPointList.Add(CF_GetRealLocation( X , Y + (1.f - l8) , Z + 1.f));
	
	RealPointList.Add(CF_GetRealLocation( X , Y , Z + l9));
	RealPointList.Add(CF_GetRealLocation( X + 1.f , Y , Z + l10));
	RealPointList.Add(CF_GetRealLocation( X + 1.f , Y + 1.f , Z + l11));
	RealPointList.Add(CF_GetRealLocation( X , Y + 1.f, Z + l12));



	
	
	TArray<int32> EdgeList = CUBE_TABLE_3D::EdgeTable[EdgeIndex].Edge;

	for (int32 i = 0; i < EdgeList.Num() / 3; ++i)
	{
		Buffer_Triangle.Add(Buffer_Vertex.Add(RealPointList[EdgeList[i * 3 + 2]]));
		Buffer_Triangle.Add(Buffer_Vertex.Add(RealPointList[EdgeList[i * 3 + 1]]));
		Buffer_Triangle.Add(Buffer_Vertex.Add(RealPointList[EdgeList[i * 3]]));
	}

}

int32 AMatchingCube3D_Smooth::CF_CheckPointIsValid(FVector InPos)
{
	return Map[CF_GetIndexByXYZ(InPos.X,InPos.Y,InPos.Z)];
}

FVector AMatchingCube3D_Smooth::CF_GetRealLocation(float X, float Y, float Z)
{
	return FVector(X,Y,Z) * CubeSize;
}

float AMatchingCube3D_Smooth::CF_GetLerpValueFromTwoPos(FIntVector A, FIntVector B) const
{
	float a = Map_Value[CF_GetIndexByXYZ(A)];
	float b = Map_Value[CF_GetIndexByXYZ(B)];
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


