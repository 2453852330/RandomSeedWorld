// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FastNoiseLite.h"
#include "RandomSeedWorldType.h"

#include "GameFramework/Actor.h"
#include "MatchingCube2D_Smooth.generated.h"

class UProceduralMeshComponent;

UCLASS()
class RANDOMSEEDWORLD_API AMatchingCube2D_Smooth : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMatchingCube2D_Smooth();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere,Category="MatchingCube2D")
	FIntVector2 ChunkSize = FIntVector2(10);

	// create mesh
	UPROPERTY(EditAnywhere,Category="MatchingCube2D")
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

	// the too small lerp value check
	UPROPERTY(EditAnywhere,Category="MatchingCube2D")
	float TooSmallValueCheck = 0.1f;
	
private:
	FastNoiseLite NoiseLite;
	TArray<BYTE> Map;
	TArray<float> Map_Value;
	int32 CF_GetIndexByXY(int32 X,int32 Y) const;
	int32 CF_GetIndexByXY(FIntVector2 InPos) const;
	
	TArray<FVector> Buffer_Vertex;
	TArray<int32> Buffer_Triangle;
	void CF_Init();
	void CF_FillAndCreate();


	void CF_CheckCubeToFillBuffer(int32 X,int32 Y);

	FVector CF_GetRealPosByXY(float X,float Y);
	FVector CF_GetRealPosByXY(FIntVector2 InPos);
	void CF_AddVertex(FVector InPos);

	int32 CF_GetRandomSeed() const;

	float CF_GetLerpValueFromTwoPos(FIntVector2 A,FIntVector2 B) const;
};
