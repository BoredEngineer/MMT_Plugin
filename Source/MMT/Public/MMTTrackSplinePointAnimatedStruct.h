//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "Components/MeshComponent.h"
#include "Containers/UnrealString.h"
#include "Engine/EngineTypes.h"
#include "MMTTrackSplinePointAnimatedStruct.generated.h"


USTRUCT(BlueprintType)
struct FTrackSplinePointAnimatedStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMT Track Animation Component")
	int32 SplinePointIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMT Track Animation Component")
	FString StaticMeshName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMT Track Animation Component")
	FName StaticMeshSocketName;

	UPROPERTY()
	UMeshComponent* StaticMesh;

	//Default constructor
	FTrackSplinePointAnimatedStruct()
	{
		SplinePointIndex = 0;
		StaticMeshName = FString("none");
		StaticMeshSocketName = FName("none");
	}

};