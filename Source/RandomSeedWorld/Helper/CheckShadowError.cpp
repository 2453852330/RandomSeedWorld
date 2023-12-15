// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckShadowError.h"

#include "ProceduralMeshComponent.h"

// Sets default values
ACheckShadowError::ACheckShadowError()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComp"));
	RootComponent = ProceduralMeshComponent;
}

// Called when the game starts or when spawned
void ACheckShadowError::BeginPlay()
{
	Super::BeginPlay();

	Buffer_Vertex.Reset();
	Buffer_Vertex.Reserve(100);

	Buffer_Triangle.Reset();
	Buffer_Triangle.Reserve(100);

	Buffer_UV0.Reset();
	Buffer_UV0.Reserve(100);

	Buffer_VertexColor.Reset();
	Buffer_VertexColor.Reserve(100);

	Buffer_Normal.Reset();
	Buffer_Normal.Reserve(100);
	
	TArray<FVector> RealPosList; RealPosList.Reserve(8);

	for (int32 i = 0; i < Vertex.Num(); ++i)
	{
		RealPosList.Add(CF_GetRealPointWithXYZ(Vertex[i]));
	}

	TArray<int32> Triangle = {
		0,2,3,0,1,2,
		4,7,6,4,6,5,
		7,3,2,7,2,6,
		5,1,0,5,0,4,
		6,2,1,6,1,5,
		4,0,3,4,3,7
	};

	for (int32 i = 0; i < Triangle.Num() / 3; ++i)
	{
		FVector p0 = RealPosList[Triangle[i * 3]];
		FVector p1 = RealPosList[Triangle[i * 3 + 1]];
		FVector p2 = RealPosList[Triangle[i * 3 + 2]];

		FVector normal = FVector::CrossProduct( (p2 - p0),( p1 - p0 ) ).GetSafeNormal();
		
		Buffer_Triangle.Add(Buffer_Vertex.Add(p0));
		Buffer_UV0.Add(FVector2D(0.f,0.f));
		Buffer_VertexColor.Add(FLinearColor::White);
		Buffer_Normal.Add(normal);

		Buffer_Triangle.Add(Buffer_Vertex.Add(p1));
		Buffer_UV0.Add(FVector2D(1.f,0.f));
		Buffer_VertexColor.Add(FLinearColor::White);
		Buffer_Normal.Add(normal);
		
		Buffer_Triangle.Add(Buffer_Vertex.Add(p2));
		Buffer_UV0.Add(FVector2D(0.f,1.f));
		Buffer_VertexColor.Add(FLinearColor::White);
		Buffer_Normal.Add(normal);
		
	}

	ProceduralMeshComponent->CreateMeshSection_LinearColor(0,Buffer_Vertex,Buffer_Triangle,Buffer_Normal,Buffer_UV0,Buffer_VertexColor,{},false);
	
}

FVector ACheckShadowError::CF_GetRealPointWithXYZ(FVector InPos) const
{
	return InPos * CubeSize;
}
