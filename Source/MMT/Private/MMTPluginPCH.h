//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

//Engine
#include "CoreUObject.h"
#include "Engine.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Pawn.h"
#include "Components/SceneComponent.h"
#include "Components/ActorComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicsPublic.h"  //FPhysScene
#include "PhysXPublic.h"    //PtoU conversions
#include "Runtime/Engine/Private/PhysicsEngine/PhysXSupport.h" //For Scene Locking using Epic's awesome helper macros like SCOPED_SCENE_READ_LOCK
//Module
#include "MMT.h"