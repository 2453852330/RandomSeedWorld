// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RandomSeedWorldType.h"

#include "GameFramework/Actor.h"
#include "MatchingCube3D.generated.h"

class UProceduralMeshComponent;

UCLASS()
class RANDOMSEEDWORLD_API AMatchingCube3D : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMatchingCube3D();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere,Category="MatchingCube2D")
	FIntVector ChunkSize = FIntVector(10);

	// create mesh
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="MatchingCube2D")
	UProceduralMeshComponent* ProceduralMeshComponent = nullptr;

	// cube size
	UPROPERTY(EditAnywhere,Category="MatchingCube2D")
	float CubeSize = 200.f;

	// random seed
	UPROPERTY(EditAnywhere,Category="MatchingCube2D")
	bool bRandomSeed = true;
	
	// noise config
	UPROPERTY(EditAnywhere,Category="MatchingCube2D")
	FFastNoiseLiteConfig FastNoiseLiteConfig;

	UPROPERTY(EditAnywhere,Category="MatchingCube2D")
	float Radius = 100.f;

	UPROPERTY(EditAnywhere,Category="MatchingCube2D")
	bool bUseFastNoise = false;
private:

	
	FastNoiseLite NoiseLite;
	TArray<BYTE> Map;
	int32 CF_GetIndexByXYZ(int32 X,int32 Y,int32 Z) const;
	int32 CF_GetIndexByXYZ(FIntVector InPos) const;
	
	TArray<FVector> Buffer_Vertex;
	TArray<int32> Buffer_Triangle;
	void CF_Init();
	void CF_FillAndCreate();

	int32 CF_GetRandomSeed() const;


	void CF_CheckCubeAndFillData(int32 X,int32 Y,int32 Z);

	int32 CF_CheckPointIsValid(FVector InPos);

	FVector CF_GetRealLocation(float X,float Y,float Z);
};
