// Fill out your copyright notice in the Description page of Project Settings.


#include "MatchingCube3D.h"

#include "CubeTable.h"
#include "ProceduralMeshComponent.h"

// Sets default values
AMatchingCube3D::AMatchingCube3D()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComp"));
	RootComponent = ProceduralMeshComponent;
}

// Called when the game starts or when spawned
void AMatchingCube3D::BeginPlay()
{
	Super::BeginPlay();

	double start_time = FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64());

	CF_Init();
	CF_FillAndCreate();
	
	UE_LOG(LogTemp, Warning, TEXT("[MatchingCube2D->BeginPlay]Cost %f ms"),FPlatformTime::ToMilliseconds64(FPlatformTime::Cycles64()) - start_time);

}

int32 AMatchingCube3D::CF_GetIndexByXYZ(int32 X, int32 Y, int32 Z) const
{
	return Z * ChunkSize.X * ChunkSize.Y + Y * ChunkSize.X + X;
}

int32 AMatchingCube3D::CF_GetIndexByXYZ(FIntVector InPos) const
{
	return InPos.Z * ChunkSize.X * ChunkSize.Y + InPos.Y * ChunkSize.X + InPos.X;

}

void AMatchingCube3D::CF_Init()
{
	Map.Reset();
	Map.AddZeroed(ChunkSize.X * ChunkSize.Y * ChunkSize.Z);
	
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

void AMatchingCube3D::CF_FillAndCreate()
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
					Map[CF_GetIndexByXYZ(i,j,k)] = value >= 0 ? 1 : 0;
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

int32 AMatchingCube3D::CF_GetRandomSeed() const
{
	return FMath::Rand();
}

void AMatchingCube3D::CF_CheckCubeAndFillData(int32 X, int32 Y, int32 Z)
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
	
	RealPointList.Add(CF_GetRealLocation( X + 0.5f, Y , Z));
	RealPointList.Add(CF_GetRealLocation( X + 1.f, Y + 0.5f , Z));
	RealPointList.Add(CF_GetRealLocation( X + 0.5f, Y + 1.f, Z));
	RealPointList.Add(CF_GetRealLocation( X , Y + 0.5f , Z));

	RealPointList.Add(CF_GetRealLocation( X + 0.5f, Y , Z + 1.f));
	RealPointList.Add(CF_GetRealLocation( X + 1.f, Y + 0.5f , Z + 1.f));
	RealPointList.Add(CF_GetRealLocation( X + 0.5f, Y + 1.f, Z + 1.f));
	RealPointList.Add(CF_GetRealLocation( X , Y + 0.5f , Z + 1.f));
	
	RealPointList.Add(CF_GetRealLocation( X , Y , Z + 0.5f));
	RealPointList.Add(CF_GetRealLocation( X + 1.f , Y , Z + 0.5f));
	RealPointList.Add(CF_GetRealLocation( X + 1.f , Y + 1.f , Z + 0.5f));
	RealPointList.Add(CF_GetRealLocation( X , Y + 1.f, Z + 0.5f));
	
	TArray<int32> EdgeList = CUBE_TABLE_3D::EdgeTable[EdgeIndex].Edge;

	for (int32 i = 0; i < EdgeList.Num() / 3; ++i)
	{
		Buffer_Triangle.Add(Buffer_Vertex.Add(RealPointList[EdgeList[i * 3 + 2]]));
		Buffer_Triangle.Add(Buffer_Vertex.Add(RealPointList[EdgeList[i * 3 + 1]]));
		Buffer_Triangle.Add(Buffer_Vertex.Add(RealPointList[EdgeList[i * 3]]));
	}

}

int32 AMatchingCube3D::CF_CheckPointIsValid(FVector InPos)
{
	return Map[CF_GetIndexByXYZ(InPos.X,InPos.Y,InPos.Z)];
}

FVector AMatchingCube3D::CF_GetRealLocation(float X, float Y, float Z)
{
	return FVector(X,Y,Z) * CubeSize;
}


