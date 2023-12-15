// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RandomSeedWorldType.h"

#include "GameFramework/Actor.h"
#include "TwoLineCheck2D.generated.h"


class UProceduralMeshComponent;
/**
 *	this is NOT dual contouring , but it can work
 *  
 */
UCLASS()
class RANDOMSEEDWORLD_API ATwoLineCheck2D : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATwoLineCheck2D();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere,Category="MatchingCube2D")
	FIntVector2 ChunkSize = FIntVector2(10);

	// create mesh
	UPROPERTY(EditAnywhere,Category="MatchingCube2D")
	UProceduralMeshComponent* ProceduralMeshComponent = nullptr;

	// noise config
	UPROPERTY(EditAnywhere,Category="MatchingCube2D")
	FFastNoiseLiteConfig FastNoiseLiteConfig;

	UPROPERTY(EditAnywhere,Category="MatchingCube2D")
	bool bRandomSeed = true;

	UPROPERTY(EditAnywhere,Category="MatchingCube2D")
	float CubeSize = 100.f;
	
private:
	FastNoiseLite NoiseLite;
	TArray<BYTE> Map;

	
	
	
	int32 CF_GetIndexByXY(int32 X,int32 Y) const;
	int32 CF_GetIndexByXY(FIntVector2 InPos) const;
	
	TArray<FVector> Buffer_Vertex;
	TArray<int32> Buffer_Triangle;
	void CF_Init();
	void CF_FillAndCreate();

	int32 CF_GetRandomSeed() const;

	void CF_CheckCubeToFillBuffer(int32 X,int32 Y);
	FVector CF_GetRealPosFromXY(float X,float Y);
};
