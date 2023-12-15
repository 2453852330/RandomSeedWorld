// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkDebug.h"

// Sets default values
AChunkDebug::AChunkDebug()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AChunkDebug::BeginPlay()
{
	Super::BeginPlay();
	
	
	CachePawn = GetWorld()->GetFirstPlayerController()->GetPawn();

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,this,&AChunkDebug::Timer_CheckAndSpawnChunk,1.f,true);
	FTimerHandle TimerHandle_Spawn;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Spawn,this,&AChunkDebug::Timer_SpawnForOptimization,0.1f,true);

	CHUNK_MARK_MAP.Reset();
	CHUNK_MARK_MAP.Reserve(1000);
}

void AChunkDebug::Timer_CheckAndSpawnChunk()
{
	if (!CachePawn)
	{
		return;
	}
	
	// the cube size is 500.f, the Chunk Size is 50*50*50 , so a chunk = 50 * 500 = 25000; 
	FVector Location = CachePawn->GetActorLocation();
	FVector ChunkRealSize =  FVector(ChunkSize) * CubeSize;
	FIntVector ChunkPos = CF_HandleChunkPos(Location,ChunkRealSize);

	



	
	CF_SpawnChunk(ChunkPos,ChunkRealSize);
		
	UE_LOG(LogTemp, Warning, TEXT("[InfiniteWorldGenerator3D->Timer_CheckAndSpawnChunk]CurrentPos:%s | ChunkPos:%s | ChunkRealSize:%s"),
	*Location.ToString(),*ChunkPos.ToString(),*ChunkRealSize.ToString());
}

void AChunkDebug::Timer_SpawnForOptimization()
{
	
}

void AChunkDebug::CF_SpawnChunk(FIntVector InPos, FVector InRealChunkSize)
{
	if (CHUNK_MARK_MAP.Find(InPos))
	{
		return;
	}
	
	FVector Pos = FVector(InPos) * InRealChunkSize;
	FVector Center =  Pos + InRealChunkSize / 2.f;
	
	DrawDebugBox(GetWorld(),Center,InRealChunkSize / 2.f,FColor::Red,true,-1,0,50.f);
	
}

FIntVector AChunkDebug::CF_HandleChunkPos(FVector InPos,FVector InRealSize)
{
	auto check_value = [](float Pos,float Size)
	{
		float value = Pos / Size;
		return FMath::Floor(value);
	};

	
	
	FIntVector Tmp;
	
	Tmp.X =  check_value(InPos.X,InRealSize.X);
	Tmp.Y =  check_value(InPos.Y,InRealSize.Y);
	Tmp.Z =  check_value(InPos.Z,InRealSize.Z);

	return Tmp;
}
