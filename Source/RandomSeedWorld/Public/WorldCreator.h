// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FastNoiseLite.h"
#include "RandomSeedWorldType.h"

#include "GameFramework/Actor.h"
#include "WorldCreator.generated.h"


class UInstancedStaticMeshComponent;

UCLASS()
class RANDOMSEEDWORLD_API AWorldCreator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldCreator();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere,Category="WorldCreator")
	FWorldCreateConfig WorldCreateConfig;

	UPROPERTY(EditAnywhere,Category="WorldCreator")
	FFastNoiseLiteConfig FastNoiseLiteConfig;

	

	/** the debug start */
	// add a temp comp to spawn debug mesh
	UPROPERTY(EditDefaultsOnly,Category="WorldCreator")
	UInstancedStaticMeshComponent* InstancedMeshComp = nullptr;
	UPROPERTY(EditAnywhere,Category="WorldCreator")
	float MeshSpawn = 100.f;
	UPROPERTY(EditAnywhere,Category="WorldCreator")
	float HeightScale = 2.f;
	/** debug end */

	
	// the function to create world
	UFUNCTION(BlueprintCallable,Category="WorldCreator")
	void BP_CreateWorld();
private:
	FastNoiseLite NoiseLite;

	void CF_Init();


	TArray<BYTE> Map;
	void CF_FillWorld();
	int32 CF_GetIndexByXYZ(int32 X,int32 Y,int32 Z) const;

	void CF_CreateMesh();

	FTransform CF_GetTransformByXYZ(int32 X,int32 Y ,int32 Z) const;
	int32 CF_GetRandomSeed() const;
};
