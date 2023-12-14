// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldCreator.h"

#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
AWorldCreator::AWorldCreator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InstancedMeshComp = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedMeshComponent"));
	RootComponent = InstancedMeshComp;
}

// Called when the game starts or when spawned
void AWorldCreator::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWorldCreator::BP_CreateWorld()
{
	CF_Init();
	CF_FillWorld();
	CF_CreateMesh();
}

void AWorldCreator::CF_Init()
{
	// general
	NoiseLite.SetNoiseType(FastNoiseLiteConfig.GetNoiseType());
	NoiseLite.SetFrequency(FastNoiseLiteConfig.Frequency);
	NoiseLite.SetSeed(WorldCreateConfig.bRandomSeed ? CF_GetRandomSeed() : FastNoiseLiteConfig.Seed);
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

void AWorldCreator::CF_FillWorld()
{
	Map.Reset();
	Map.AddZeroed(WorldCreateConfig.GetTotalCount());

	for (int32 i = 0; i < WorldCreateConfig.WorldSize.X; ++i)
	{
		for (int32 j = 0; j < WorldCreateConfig.WorldSize.Y; ++j)
		{
			for (int32 k = 0; k < WorldCreateConfig.WorldSize.Z; ++k)
			{
				// scale the sample Z value ;
				float noise_value = NoiseLite.GetNoise<float>(float(i),float(j),float(k) * WorldCreateConfig.SampleZScale);
				Map[CF_GetIndexByXYZ(i,j,k)] = noise_value >= WorldCreateConfig.CreateMeshThreshold ? 1 : 0;
			}
		}
	}
	
}

int32 AWorldCreator::CF_GetIndexByXYZ(int32 X, int32 Y, int32 Z) const
{
	return WorldCreateConfig.GetIndexByXYZ(X,Y,Z);
}

void AWorldCreator::CF_CreateMesh()
{
	for (int32 i = 0; i < WorldCreateConfig.WorldSize.X; ++i)
	{
		for (int32 j = 0; j < WorldCreateConfig.WorldSize.Y; ++j)
		{
			for (int32 k = 0; k < WorldCreateConfig.WorldSize.Z; ++k)
			{
				if (Map[CF_GetIndexByXYZ(i,j,k)] == 1)
				{
					InstancedMeshComp->AddInstance(CF_GetTransformByXYZ(i,j,k));
				}
			}
		}
	}
}

FTransform AWorldCreator::CF_GetTransformByXYZ(int32 X, int32 Y, int32 Z) const
{
	FTransform Transform;
	Transform.SetRotation(FQuat::Identity);
	Transform.SetScale3D(FVector(1.f));
	FVector Pos = FVector(X,Y,Z * HeightScale) * MeshSpawn;
	
	Transform.SetLocation(Pos);

	return Transform;
}

int32 AWorldCreator::CF_GetRandomSeed() const
{
	return FMath::RandRange(0,999999999);
}


