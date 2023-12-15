// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RandomSeedWorldType.h"

#include "GameFramework/Actor.h"
#include "InfiniteWorldGenerator3D.generated.h"


UCLASS()
class RANDOMSEEDWORLD_API AInfiniteWorldGenerator3D : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInfiniteWorldGenerator3D();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

	UPROPERTY(EditAnywhere,Category="InfiniteWorldGenerator3D")
	UMaterialInterface* ShowMaterial = nullptr;

	UPROPERTY(EditAnywhere,Category="InfiniteWorldGenerator3D")
	FFastNoiseLiteConfig NoiseLiteConfig;

	UPROPERTY(EditAnywhere,Category="InfiniteWorldGenerator3D")
	FIntVector ChunkSize;

	UPROPERTY(EditAnywhere,Category="InfiniteWorldGenerator3D")
	float CubeSize = 500.f;

	UPROPERTY(EditAnywhere,Category="InfiniteWorldGenerator3D")
	float Timer_CheckSpan = 0.1f;
	UPROPERTY(EditAnywhere,Category="InfiniteWorldGenerator3D")
	float Timer_SpawnSpan = 0.3f;	
	
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
	FIntVector CF_HandleChunkPos(FVector InRealPos,FVector InRealSize);
};
