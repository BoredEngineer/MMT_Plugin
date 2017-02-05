//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#pragma once 
//Engine
#include "CoreUObject.h"
#include "Engine.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Pawn.h"
#include "Components/SceneComponent.h"
#include "Runtime/Engine/Public/CollisionQueryParams.h"
#include "Components/ActorComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicsPublic.h"  //FPhysScene
#include "PhysXPublic.h"    //PtoU conversions
#include "Runtime/Engine/Private/PhysicsEngine/PhysXSupport.h" //For Scene Locking using Epic's awesome helper macros like SCOPED_SCENE_READ_LOCK
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Components/MeshComponent.h"
#include "Containers/UnrealString.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectBaseUtility.h"

//Module
#include "MMT.h"

//For UE4 Profiler ~ Stat Group
DECLARE_STATS_GROUP(TEXT("MMT"), STATGROUP_MMT, STATCAT_Advanced);