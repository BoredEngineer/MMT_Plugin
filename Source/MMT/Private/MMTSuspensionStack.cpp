//Copyright(c) 2017 Viktor Kuropiatnyk "BoredEngineer"

#include "MMTPluginPCH.h"
#include "MMTBPFunctionLibrary.h"
#include "MMTSuspensionStack.h"


// Sets default values for this component's properties
UMMTSuspensionStack::UMMTSuspensionStack()
{
	//Bind async trace delegate
	//TraceDelegate.BindRaw(this, &UMMTSuspensionStack::AsyncTraceDone);
	TraceDelegate.BindUObject(this, &UMMTSuspensionStack::AsyncTraceDone);
}

void UMMTSuspensionStack::Initialize()
{
	USceneComponent* TryParent = CastChecked<USceneComponent>(GetOuter());
	if (IsValid(TryParent))
	{
		ParentComponentRef = TryParent;

		GetNamesForComponentAndParent();
		GetNamedComponentsReference();
		PreCalculateParameters();

		//Line Trace default query parameters, called from here to have valid reference to parent
		LineTraceQueryParameters.bTraceAsyncScene = false;
		LineTraceQueryParameters.bTraceComplex = false;
		LineTraceQueryParameters.bReturnFaceIndex = false;
		LineTraceQueryParameters.bReturnPhysicalMaterial = true;
		LineTraceQueryParameters.AddIgnoredActor(ParentComponentRef->GetOwner());
	}
	else
	{
		//Disable component to avoid potential null point reference
		SuspensionSettings.bDisabled = true;

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%Inner Suspension Stack object failed to receive correct parent reference")));
		UE_LOG(LogTemp, Warning, TEXT("Inner Suspension Stack object failed to receive correct parent reference"));
	}
}


void UMMTSuspensionStack::GetNamesForComponentAndParent()
{
		//Get names of component and its parent
		ComponentName = ParentComponentRef->GetName();
		ComponentsParentName = ParentComponentRef->GetOwner()->GetName();
}

//Find and store reference to named components
void UMMTSuspensionStack::GetNamedComponentsReference()
{
	//Sprung mesh reference
	if (!bSprungMeshComponentSetManually)
	{
		if (SuspensionSettings.SprungComponentName != FString("none"))
		{
			SprungMeshComponent = UMMTBPFunctionLibrary::GetMeshComponentReferenceByName(ParentComponentRef, SuspensionSettings.SprungComponentName);
			if (!IsValid(SprungMeshComponent))
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s component failed to find component named '%s' or it's not derived from MeshComponent class"), *ComponentsParentName, *ComponentName, *SuspensionSettings.SprungComponentName));
				UE_LOG(LogTemp, Warning, TEXT("%s->%s component failed to find component named '%s' or it's not derived from MeshComponent class"), *ComponentsParentName, *ComponentName, *SuspensionSettings.SprungComponentName);
			}
		}
		else
		{
			SprungMeshComponent = NULL;
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s component's SprungComponentName property shouldn't be 'none', set proper name for effected component"), *ComponentsParentName, *ComponentName));
			UE_LOG(LogTemp, Warning, TEXT("%s->%s component's EffectedComponentName property shouldn't be 'none', set proper name for effected component"), *ComponentsParentName, *ComponentName);
		}
	}

	//ShapeSweep mesh reference
	if (SuspensionSettings.bCanShapeSweep & !bSweepShapeMeshComponentSetManually)
	{
		if (SuspensionSettings.SweepShapeComponentName != FString("none"))
		{
			SweepShapeMeshComponent = UMMTBPFunctionLibrary::GetMeshComponentReferenceByName(ParentComponentRef, SuspensionSettings.SweepShapeComponentName);
			if (!IsValid(SweepShapeMeshComponent))
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s component failed to find component named '%s' or it's not derived from MeshComponent class"), *ComponentsParentName, *ComponentName, *SuspensionSettings.SweepShapeComponentName));
				UE_LOG(LogTemp, Warning, TEXT("%s->%s component failed to find component named '%s' or it's not derived from MeshComponent class"), *ComponentsParentName, *ComponentName, *SuspensionSettings.SweepShapeComponentName);
			}
		}
		else
		{
			SweepShapeMeshComponent = NULL;
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s component's SweepShapeComponentName property shouldn't be 'none', set proper name for effected component"), *ComponentsParentName, *ComponentName));
			UE_LOG(LogTemp, Warning, TEXT("%s->%s component's EffectedComponentName property shouldn't be 'none', set proper name for effected component"), *ComponentsParentName, *ComponentName);
		}
	}
}

//Recalculate parameters to save performance
void UMMTSuspensionStack::PreCalculateParameters() 
{
	//Shift spring offsets if custom position of the stack is used
	SpringOffsetTopAdjusted = SuspensionSettings.bUseCustomPosition ? SuspensionSettings.StackLocalPosition + SuspensionSettings.SpringTopOffset : SuspensionSettings.SpringTopOffset;
	SpringOffsetBottomAdjusted = SuspensionSettings.bUseCustomPosition ? SuspensionSettings.StackLocalPosition + SuspensionSettings.SpringBottomOffset : SuspensionSettings.SpringBottomOffset;

	//Calculate spring direction in local coordinate system
	SpringDirectionLocal = SpringOffsetBottomAdjusted - SpringOffsetTopAdjusted;
	SpringMaxLenght = SpringDirectionLocal.Size();
	
	//Normalize vector and check if normalization was successful
	if (!SpringDirectionLocal.Normalize())
	{
		SpringDirectionLocal = FVector::UpVector;
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s distance between Top and Bottom offsets of the spring shouldn't be zero"), *ComponentsParentName, *ComponentName));
		UE_LOG(LogTemp, Warning, TEXT("%s->%s distance between Top and Bottom offsets of the spring shouldn't be zero"), *ComponentsParentName, *ComponentName);
	}
	
	//Calculate line trace points in local space, taking into account road wheel radius and tread thickness
	LineTraceOffsetTopLS = SpringOffsetTopAdjusted - SpringDirectionLocal * SuspensionSettings.RoadWheelRadius;
	LineTraceOffsetBottomLS = SpringOffsetBottomAdjusted + SpringDirectionLocal * (SuspensionSettings.RoadWheelRadius + SuspensionSettings.TrackThickness);
}


//Updates position of the road-wheel, calculates and applies spring force to sprung component
void UMMTSuspensionStack::PhysicsUpdate(const float& DeltaTime)
{
	if (!SuspensionSettings.bDisabled)
	{

		//Update world space transform of parent component
		ReferenceFrameTransform = UMMTBPFunctionLibrary::MMTGetTransformComponent(ParentComponentRef, NAME_None);

		UpdateWheelHubPosition();

		CalculateAndApplySuspensionForce(DeltaTime);
	}
}


void UMMTSuspensionStack::LineTraceForContact()
{
	//Clean results
	FHitResult LineTraceOutHit = FHitResult(ForceInit);

	//Transform points into world space using component's transform
	FVector LineTraceStart = ReferenceFrameTransform.TransformPosition(LineTraceOffsetTopLS);
	FVector LineTraceEnd = ReferenceFrameTransform.TransformPosition(LineTraceOffsetBottomLS);

	//Do line trace
	bContactPointActive = ParentComponentRef->GetWorld()->LineTraceSingleByChannel(LineTraceOutHit, LineTraceStart, LineTraceEnd, SuspensionSettings.RayCheckTraceChannel,
		LineTraceQueryParameters, LineTraceResponseParameters);
		//ParentComponentRef->GetWorld()->LineTraceSingleByChannel(LineTraceOutHit, LineTraceStart, LineTraceEnd, SuspensionSettings.RayCheckTraceChannel,
			//				LineTraceQueryParameters, LineTraceResponseParameters);
	
	//Dirty but assumption is that contact information is never used if bContactPointActive is false.
	if (bContactPointActive)
	{
		ContactPointLocation = LineTraceOutHit.ImpactPoint;
		ContactPointNormal = LineTraceOutHit.ImpactNormal;
		ContactPhysicalMaterial = LineTraceOutHit.PhysMaterial.Get();

		if (SuspensionSettings.bGetContactBodyVelocity)
		{
			if (LineTraceOutHit.Component->IsSimulatingPhysics())
			{
				ContactInducedVelocity = LineTraceOutHit.Component->GetPhysicsLinearVelocityAtPoint(ContactPointLocation);
			}
			else
			{
				ContactInducedVelocity = LineTraceOutHit.Component->ComponentVelocity;
			}
			//DrawDebugString(ParentComponentRef->GetWorld(), ParentComponentRef->GetComponentLocation(), ContactInducedVelocity.ToString(), 0, FColor::Cyan, 0.0f, false);
		}
	}

	//Draw debug
	if (SuspensionSettings.bEnableDebugMode)
	{
		DrawDebugLineTrace(bContactPointActive, LineTraceStart, LineTraceEnd, LineTraceOutHit.ImpactPoint, ParentComponentRef->GetWorld());
		//DrawDebugString(ParentComponentRef->GetWorld(), ParentComponentRef->GetComponentLocation(), SpringDirectionLocal.ToString(), 0, FColor::Cyan, 0.0f, false);
	}
}

void UMMTSuspensionStack::AsyncLineTraceForContact()
{
	//Transform points into world space using component's transform
	FVector LineTraceStart = ReferenceFrameTransform.TransformPosition(LineTraceOffsetTopLS);
	FVector LineTraceEnd = ReferenceFrameTransform.TransformPosition(LineTraceOffsetBottomLS);

	//Request async trace
	ParentComponentRef->GetWorld()->AsyncLineTraceByChannel(EAsyncTraceType::Single ,LineTraceStart, LineTraceEnd, SuspensionSettings.RayCheckTraceChannel, 
		LineTraceQueryParameters, LineTraceResponseParameters, &TraceDelegate, 0);
}

//Process async line trace results
void UMMTSuspensionStack::AsyncTraceDone(const FTraceHandle& TraceHandle, FTraceDatum & TraceData)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Got Async trace callback")));
	//UE_LOG(LogTemp, Warning, TEXT("Got Async trace callback"));

	if (TraceData.OutHits.Num() > 0)
	{
		if (SuspensionSettings.bEnableDebugMode)
		{
			DrawDebugLineTrace(TraceData.OutHits[0].bBlockingHit, TraceData.Start, TraceData.End, TraceData.OutHits[0].ImpactPoint, ParentComponentRef->GetWorld());
		}
	}
	else{}
}


//Find new position of road-wheel according to current simulation mode
void UMMTSuspensionStack::UpdateWheelHubPosition()
{
	if (SuspensionSettings.SimulationMode == ESuspensionSimMode::RayCheck)
	{
		if (!TryAsyncTraceMode)
		{
			LineTraceForContact();
		}
		else
		{
			AsyncLineTraceForContact();
		}

		if (bContactPointActive)
		{
			//Shift traced contact point by the radius of the wheel and thickness of track
			WheelHubPositionLS = ReferenceFrameTransform.InverseTransformPosition(ContactPointLocation) -
				SpringDirectionLocal * (SuspensionSettings.RoadWheelRadius + SuspensionSettings.TrackThickness);
		}
		else
		{
			//If trace failed wheel hub is assumed to be in lowest possible position of suspension
			WheelHubPositionLS = SpringOffsetBottomAdjusted;
		}

		if (SuspensionSettings.bEnableDebugMode)
		{
			DrawDebugSphere(ParentComponentRef->GetWorld(), ReferenceFrameTransform.TransformPosition(WheelHubPositionLS), 10.0, 9, FColor::Blue, false, 0.0f, 0, 1.0f);
		}
	}
	else
	{
		
		if (SuspensionSettings.SimulationMode == ESuspensionSimMode::SphereCheck)
		{
			//DoOnce([]() { UE_LOG(...); });
			if (!bWarningMessageDisplayed)
			{
				bWarningMessageDisplayed = true;
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s Sphere Check mode is not implemented"), *ComponentsParentName, *ComponentName));
				UE_LOG(LogTemp, Warning, TEXT("%s->%s Sphere Check mode is not implemented"), *ComponentsParentName, *ComponentName);
			}
		}
		else
		{
			if (SuspensionSettings.SimulationMode == ESuspensionSimMode::ShapeSweep)
			{
				if (!bWarningMessageDisplayed)
				{
					bWarningMessageDisplayed = true;
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s Shape Sweep simulation mode is not implemented"), *ComponentsParentName, *ComponentName));
					UE_LOG(LogTemp, Warning, TEXT("%s->%s Shape Sweep simulation mode is not implemented"), *ComponentsParentName, *ComponentName);
				}

			}
		}
	}
}


void UMMTSuspensionStack::CalculateAndApplySuspensionForce(const float& DeltaTime)
{
	//Calculate spring compression
	float NewSpringLenght = FVector(SpringOffsetTopAdjusted - WheelHubPositionLS).Size();
	float SpringDelta = FMath::Clamp(SpringMaxLenght - NewSpringLenght, 0.0f, SpringMaxLenght);

	float CompressionRatio = FMath::Clamp((NewSpringLenght - 1.0f) / SpringMaxLenght, 0.0f, 1.0f);

	float SpringForce;

	//Spring force curve is defined by two segments. First segment is very steep, it goes from 0 to 1cm of suspension travel and force changes from 0 to full amount of SpringStiffness
	//Second segment starts from 1cm and goes to SpringMaxLenght, on this segment force changes from SpringStiffness to SpringStiffness * SpringMaxOutputRatio
	if (SpringDelta > 1.0f)
	{
		SpringForce = FMath::Lerp(SuspensionSettings.SpringStiffness * SuspensionSettings.SpringMaxOutputRatio, SuspensionSettings.SpringStiffness, CompressionRatio);
	}
	else
	{
		SpringForce = SuspensionSettings.SpringStiffness * SpringDelta;
	}
	//Limit damping to the magnitude of the spring force
	float SpringDamping = FMath::Clamp(SuspensionSettings.SpringDampening * ((NewSpringLenght - PreviousSpringLenght) / DeltaTime), -SpringForce, SpringForce);

	//Limit magnitude of suspension force
	//SuspensionForceMagnitude = FMath::Clamp(SpringForce - Dampening, 0.0f, SuspensionSettings.SpringStiffness * SuspensionSettings.SpringMaxOutputRatio);
	SuspensionForceMagnitude = (SpringForce - SpringDamping) * SuspensionForceScale;

	SuspensionForceLS = SuspensionForceMagnitude * (-1) * SpringDirectionLocal;

	SuspensionForceWS = ReferenceFrameTransform.TransformVector(SuspensionForceLS);

	ContactForceAtPoint = SuspensionForceWS;

	//Apply suspension force
	UMMTBPFunctionLibrary::MMTAddForceAtLocationComponent(SprungMeshComponent, SuspensionForceWS, ReferenceFrameTransform.GetLocation());
		
	if (SuspensionSettings.bEnableDebugMode)
	{
		DrawDebugLine(ParentComponentRef->GetWorld(), ReferenceFrameTransform.GetLocation(), ReferenceFrameTransform.GetLocation() + SuspensionForceWS * 0.0001f, FColor::Blue, false, 0.0f, 50, 1.0f);
		DrawDebugString(ParentComponentRef->GetWorld(), ReferenceFrameTransform.GetLocation(), FString::SanitizeFloat(SuspensionForceMagnitude), 0, FColor::Blue, 0.0f, false);
	}

	//Store last spring length
	PreviousSpringLenght = NewSpringLenght;
}

void UMMTSuspensionStack::DrawDebugLineTrace(bool bBlockingHit, FVector Start, FVector End, FVector HitPoint, UWorld *WorldRef)
{
	if (bBlockingHit)
	{
		DrawDebugLine(WorldRef, Start, HitPoint, FColor::Red, false, 0.0f, 1, 1.0f);
		DrawDebugLine(WorldRef, HitPoint, End, FColor::Green, false, 0.0f, 1, 1.0f);
		DrawDebugPoint(WorldRef, HitPoint, 10.0, FColor::Yellow, false, 0.0f, 0);
	}
	else
	{
		DrawDebugLine(WorldRef, Start, End, FColor::Yellow, false, 0.0f, 0, 1.0f);
	}
}

void UMMTSuspensionStack::ToggleDebugMode()
{
	SuspensionSettings.bEnableDebugMode = !SuspensionSettings.bEnableDebugMode;
}

void UMMTSuspensionStack::GetSuspensionForce(float &Magnitude, FVector& WorldSpace, FVector& LocalSpace)
{
	Magnitude = SuspensionForceMagnitude;
	WorldSpace = SuspensionForceWS;
	LocalSpace = SuspensionForceLS;
}

	void UMMTSuspensionStack::GetContactData(bool &bPointActive, FVector& ForceAtPoint, FVector& PointLocation, FVector& SurfaceNormal, class UPhysicalMaterial*& SurfacePhysicalMaterial, FVector& SurfaceVelocity)
{
	bPointActive = bContactPointActive;
	ForceAtPoint = ContactForceAtPoint;
	PointLocation = ContactPointLocation;
	SurfaceNormal = ContactPointNormal;
	SurfacePhysicalMaterial = ContactPhysicalMaterial;
	SurfaceVelocity = ContactInducedVelocity;
}

FVector UMMTSuspensionStack::GetWheelHubPosition()
{
	return WheelHubPositionLS;
}


bool UMMTSuspensionStack::SetSprungComponentReference(UMeshComponent* SprungMeshComponentRef)
{
	if (IsValid(SprungMeshComponentRef))
	{
		SprungMeshComponent = SprungMeshComponentRef;
		bSprungMeshComponentSetManually = true;
		return true;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s manual setting of sprung mesh component failed"), *ComponentsParentName, *ComponentName));
		UE_LOG(LogTemp, Warning, TEXT("%s->%s manual setting of sprung mesh component failed"), *ComponentsParentName, *ComponentName);
		return false;
	}
}


bool UMMTSuspensionStack::SetSweepShapeComponentReference(UMeshComponent* SweepShapeMeshComponentRef)
{
	if (IsValid(SweepShapeMeshComponentRef))
	{
		SweepShapeMeshComponent = SweepShapeMeshComponentRef;
		bSweepShapeMeshComponentSetManually = true;
		return true;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s manual setting of sweep shape mesh component failed"), *ComponentsParentName, *ComponentName));
		UE_LOG(LogTemp, Warning, TEXT("%s->%s manual setting of sweep shape mesh component failed"), *ComponentsParentName, *ComponentName);
		return false;
	}
}

void UMMTSuspensionStack::SetSpringStiffness(float NewSpringStiffness)
{
	SuspensionSettings.SpringStiffness = NewSpringStiffness;
}

void UMMTSuspensionStack::SetSpringOffsets(FVector SpringTopOffset, FVector SpringBottomOffset, bool bUpdateAllParameters)
{
	SuspensionSettings.SpringTopOffset = SpringTopOffset;
	SuspensionSettings.SpringBottomOffset = SpringBottomOffset;

	if (bUpdateAllParameters)
	{
		PreCalculateParameters();
	}
}

void UMMTSuspensionStack::SetSuspensionForceScale(float NewSuspensionForceScale)
{
	SuspensionForceScale = NewSuspensionForceScale;
}

float UMMTSuspensionStack::GetSuspensionForceScale()
{
	return SuspensionForceScale;
}