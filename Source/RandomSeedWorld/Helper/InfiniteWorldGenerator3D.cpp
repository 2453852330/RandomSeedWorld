// Fill out your copyright notice in the Description page of Project Settings.


#include "InfiniteWorldGenerator3D.h"

#include "ProceduralMeshComponent.h"

#include "GameFramework/SpectatorPawn.h"

#include "RandomSeedWorld/MatchingCube/MatchingCube3D_Smooth_WorldSample.h"

// Sets default values
AInfiniteWorldGenerator3D::AInfiniteWorldGenerator3D()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInfiniteWorldGenerator3D::BeginPlay()
{
	Super::BeginPlay();

	CachePawn = GetWorld()->GetFirstPlayerController()->GetPawn();

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,this,&AInfiniteWorldGenerator3D::Timer_CheckAndSpawnChunk,Timer_CheckSpan,true);
	FTimerHandle TimerHandle_Spawn;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Spawn,this,&AInfiniteWorldGenerator3D::Timer_SpawnForOptimization,Timer_SpawnSpan,true);

	CHUNK_MARK_MAP.Reset();
	CHUNK_MARK_MAP.Reserve(1000);
}


void AInfiniteWorldGenerator3D::Timer_CheckAndSpawnChunk()
{
	if (!CachePawn)
	{
		return;
	}
	
	// the cube size is 500.f, the Chunk Size is 50*50*50 , so a chunk = 50 * 500 = 25000; 
	FVector Location = CachePawn->GetActorLocation();
	FVector ChunkRealSize =  FVector(ChunkSize) * CubeSize;
	FIntVector ChunkPos = CF_HandleChunkPos(Location,ChunkRealSize);

	
	// UE_LOG(LogTemp, Warning, TEXT("[InfiniteWorldGenerator3D->Timer_CheckAndSpawnChunk]CurrentPos:%s | ChunkPos:%s"),
	// 	*Location.ToString(),*ChunkPos.ToString());
	if (!CHUNK_MARK_MAP.Find(ChunkPos ))
	{
		CF_SpawnChunk(ChunkPos,ChunkRealSize);
		CHUNK_MARK_MAP.Add(ChunkPos,0);
	}
	
		
	TArray<FIntVector> NeedSpawnChunkList = {
		FIntVector(0,0,1),FIntVector(0,0,-1),
		
		FIntVector(1,0,0),FIntVector(-1,0,0),
		FIntVector(0,1,0),FIntVector(0,-1,0),
		FIntVector(1,1,0),FIntVector(1,-1,0),
		FIntVector(-1,-1,0),FIntVector(-1,1,0),

		FIntVector(1,0,1),FIntVector(-1,0,1),
		FIntVector(0,1,1),FIntVector(0,-1,1),
		FIntVector(1,1,1),FIntVector(1,-1,1),
		FIntVector(-1,-1,1),FIntVector(-1,1,1),

		FIntVector(1,0,-1),FIntVector(-1,0,-1),
		FIntVector(0,1,-1),FIntVector(0,-1,-1),
		FIntVector(1,1,-1),FIntVector(1,-1,-1),
		FIntVector(-1,-1,-1),FIntVector(-1,1,-1),

		// FIntVector(2,0,0),FIntVector(-2,0,0),
		// FIntVector(0,2,0),FIntVector(0,-2,0),
		// FIntVector(2,2,0),FIntVector(2,-2,0),
		// FIntVector(-2,-2,0),FIntVector(-2,2,0),
		//
		// FIntVector(2,0,2),FIntVector(-2,0,2),
		// FIntVector(0,2,2),FIntVector(0,-2,2),
		// FIntVector(2,2,2),FIntVector(2,-2,2),
		// FIntVector(-2,-2,2),FIntVector(-2,2,2),
		//
		// FIntVector(2,0,-2),FIntVector(-2,0,-2),
		// FIntVector(0,2,-2),FIntVector(0,-2,-2),
		// FIntVector(2,2,-2),FIntVector(2,-2,-2),
		// FIntVector(-2,-2,-2),FIntVector(-2,2,-2)
	};

	for (int32 i = 0; i < NeedSpawnChunkList.Num(); ++i)
	{
		// CF_SpawnChunk(ChunkPos+NeedSpawnChunkList[i],ChunkRealSize);
		FIntVector CheckPos = ChunkPos + NeedSpawnChunkList[i];
		if (!CHUNK_MARK_MAP.Find(CheckPos ))
		{
			CHUNK_SPAWN_QUEUE.Enqueue(FChunkSpawnInfo(CheckPos,ChunkRealSize));
			CHUNK_MARK_MAP.Add(CheckPos,0);
		}
	}
	
}

void AInfiniteWorldGenerator3D::Timer_SpawnForOptimization()
{
	if (!CHUNK_SPAWN_QUEUE.IsEmpty())
	{
		FChunkSpawnInfo SpawnInfo;
		CHUNK_SPAWN_QUEUE.Dequeue(SpawnInfo);
		CF_SpawnChunk(SpawnInfo.Pos,SpawnInfo.RealSize);
	}
}


void AInfiniteWorldGenerator3D::CF_SpawnChunk(FIntVector InPos,FVector InRealChunkSize)
{
	FTransform Transform((FVector(InPos) * InRealChunkSize)) ;
	// if cant find , spawn the chunk
	AMatchingCube3D_Smooth_WorldSample* Cube3D =
		GetWorld()->SpawnActorDeferred<AMatchingCube3D_Smooth_WorldSample>(AMatchingCube3D_Smooth_WorldSample::StaticClass(),Transform,this,CachePawn);
	if (Cube3D)
	{
		Cube3D->bRandomSeed = false;
		Cube3D->CubeSize = CubeSize;
		Cube3D->FastNoiseLiteConfig = NoiseLiteConfig;
		Cube3D->bUseFastNoise = true;
		Cube3D->ChunkSize = ChunkSize + FIntVector(1);

		Cube3D->FinishSpawning(Transform);
		Cube3D->ProceduralMeshComponent->SetMaterial(0,ShowMaterial);
		CHUNK_MARK_MAP.Add(InPos,1);
	}
}

FIntVector AInfiniteWorldGenerator3D::CF_HandleChunkPos(FVector InRealPos,FVector InRealSize)
{
 	FVector ChunkPos = InRealPos / InRealSize;
	return FIntVector(FMath::Floor(ChunkPos.X),FMath::Floor(ChunkPos.Y),FMath::Floor(ChunkPos.Z));
}

