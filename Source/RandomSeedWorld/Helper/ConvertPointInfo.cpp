// Fill out your copyright notice in the Description page of Project Settings.


#include "ConvertPointInfo.h"

// Sets default values
AConvertPointInfo::AConvertPointInfo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AConvertPointInfo::BeginPlay()
{
	Super::BeginPlay();

	// "C:\Users\CYC\Desktop\ExampleDemo532\Plugins\RandomSeedWorld\BackupFile\3d_edge_list.txt"

	FString Path = TEXT("C:\\Users\\CYC\\Desktop\\ExampleDemo532\\Plugins\\RandomSeedWorld\\BackupFile\\3d_edge_list.txt");
	FString WritePath = TEXT("C:\\Users\\CYC\\Desktop\\ExampleDemo532\\Plugins\\RandomSeedWorld\\BackupFile\\3d_edge_Handle_list.txt");
	if (FPaths::FileExists(Path))
	{
		TArray<FString> Result;
		TArray<FString> WriteBuffer;
		if (FFileHelper::LoadFileToStringArray(Result,*Path))
		{
			for (int32 i = 0; i < Result.Num(); ++i)
			{
				if (Result[i].EndsWith(TEXT(",")))
				{
					Result[i].RemoveAt(Result[i].Len(),1);
				}

				WriteBuffer.Add(FString::Printf(TEXT("FEdgeTable({%s})"),*Result[i]));
			}

			FFileHelper::SaveStringArrayToFile(WriteBuffer,*WritePath);	
		}
	}
}
