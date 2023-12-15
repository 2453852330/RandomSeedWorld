// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChunkDebug.generated.h"

UCLASS()
class RANDOMSEEDWORLD_API AChunkDebug : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunkDebug();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere,Category="InfiniteWorldGenerator3D")
	FIntVector ChunkSize;

	UPROPERTY(EditAnywhere,Category="InfiniteWorldGenerator3D")
	float CubeSize = 500.f;
	
private:
	TMap<FIntVector,BYTE> CHUNK_MARK_MAP;

	void Timer_CheckAndSpawnChunk();

	void Timer_SpawnForOptimization();
	UPROPERTY()
	APawn* CachePawn = nullptr;

	struct FChunkSpawnInfo
	{
		FIntVector Pos = FIntVector(0);
		FVector RealSize = FVector(0);
		FChunkSpawnInfo(){}
		FChunkSpawnInfo(FIntVector InPos,FVector InSize):Pos(InPos),RealSize(InSize){}
	};
	TQueue<FChunkSpawnInfo> CHUNK_SPAWN_QUEUE;

	void CF_SpawnChunk(FIntVector InPos,FVector InRealChunkSize);

	FIntVector CF_HandleChunkPos(FVector InPos,FVector InRealSize);

	
};
