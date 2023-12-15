// Fill out your copyright notice in the Description page of Project Settings.


#include "TwoLineCheck2D.h"

#include "ProceduralMeshComponent.h"

// Sets default values
ATwoLineCheck2D::ATwoLineCheck2D()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComponent"));
	RootComponent = ProceduralMeshComponent;
}

// Called when the game starts or when spawned
void ATwoLineCheck2D::BeginPlay()
{
	Super::BeginPlay();

	CF_Init();
	CF_FillAndCreate();
	
}

int32 ATwoLineCheck2D::CF_GetIndexByXY(int32 X, int32 Y) const
{
	return Y * ChunkSize.X + X;

}

int32 ATwoLineCheck2D::CF_GetIndexByXY(FIntVector2 InPos) const
{
	return InPos.Y * ChunkSize.X + InPos.X;

}

void ATwoLineCheck2D::CF_Init()
{
	Map.Reset();
	Map.AddZeroed(ChunkSize.X * ChunkSize.Y);
	
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

void ATwoLineCheck2D::CF_FillAndCreate()
{
	for (int32 i = 0; i < ChunkSize.X; ++i)
	{
		for (int32 j = 0; j < ChunkSize.Y; ++j)
		{
			float Value = NoiseLite.GetNoise<float>(float(i),float(j));
			Map[CF_GetIndexByXY(i,j)] = Value >= 0 ? 1 : 0 ;
		}
	}

	for (int32 i = 0; i < ChunkSize.X - 2; ++i)
	{
		for (int32 j = 0; j < ChunkSize.Y - 2; ++j)
		{
			CF_CheckCubeToFillBuffer(i,j);
		}
	}


	UE_LOG(LogTemp, Warning, TEXT("[MatchingCube2D->CF_FillAndCreate]Find %d Vertex + %d Triangle"),Buffer_Vertex.Num(),Buffer_Triangle.Num());

	ProceduralMeshComponent->CreateMeshSection_LinearColor(0,Buffer_Vertex,Buffer_Triangle,{},{},{},{},true);
}

int32 ATwoLineCheck2D::CF_GetRandomSeed() const
{
	return FMath::Rand();
}

void ATwoLineCheck2D::CF_CheckCubeToFillBuffer(int32 X, int32 Y)
{
	// check center pos
	FIntVector2 CenterPos = FIntVector2(X+1,Y+1);
	
	// need create cube
	if (Map[CF_GetIndexByXY(CenterPos)] == 1)
	{
		// p0 --- p1
		// |      |
		// p3 --- p2
		
		FVector p0 = CF_GetRealPosFromXY(X+0.5f,Y+0.5f);
		FVector p1 = CF_GetRealPosFromXY(X+1.5f,Y+0.5f);
		FVector p2 = CF_GetRealPosFromXY(X+1.5f,Y+1.5f);
		FVector p3 = CF_GetRealPosFromXY(X+0.5f,Y+1.5f);

		//
		Buffer_Triangle.Add(Buffer_Vertex.Add(p0));
		Buffer_Triangle.Add(Buffer_Vertex.Add(p3));
		Buffer_Triangle.Add(Buffer_Vertex.Add(p1));

		Buffer_Triangle.Add(Buffer_Vertex.Add(p1));
		Buffer_Triangle.Add(Buffer_Vertex.Add(p3));
		Buffer_Triangle.Add(Buffer_Vertex.Add(p2));
	}
}

FVector ATwoLineCheck2D::CF_GetRealPosFromXY(float X, float Y)
{
	return FVector(X,Y,0) * CubeSize;
}

