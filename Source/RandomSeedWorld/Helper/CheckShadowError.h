// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckShadowError.generated.h"

class UProceduralMeshComponent;

/**
 *	if the mesh data NOT have Normal , will has Error Shadow;
 *  
 */
UCLASS()
class RANDOMSEEDWORLD_API ACheckShadowError : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckShadowError();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite)
	UProceduralMeshComponent* ProceduralMeshComponent = nullptr;

	UPROPERTY(EditAnywhere,Category="CheckShadowError")
	float CubeSize = 100.f;
	
private:
	TArray<FVector> Vertex = {
		FVector(0, 0, 0),
		FVector(1, 0, 0),
		FVector(1, 1, 0),
		FVector(0, 1, 0),
		FVector(0, 0, 1),
		FVector(1, 0, 1),
		FVector(1, 1, 1),
		FVector(0, 1, 1),
	};

	TArray<FVector> Buffer_Vertex;
	TArray<int32> Buffer_Triangle;
	TArray<FVector2D> Buffer_UV0;
	TArray<FLinearColor> Buffer_VertexColor;
	TArray<FVector> Buffer_Normal;
	
	FVector CF_GetRealPointWithXYZ(FVector InPos) const;
};
