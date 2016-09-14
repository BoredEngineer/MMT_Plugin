//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#include "MMTPluginPCH.h"
#include "MMTBPFunctionLibrary.h"
#include "MMTTrackAnimationComponent.h"

//Set default values
UMMTTrackAnimationComponent::UMMTTrackAnimationComponent()
{
	TrackSplineComponentName = FString("none");
	TreadsInstancedMeshComponentName = FString("none");
	TreadsOnSide = 30;
	IsFlipAnimation = false;
	IsDebugMode = false;
}

// Called when the game starts or when spawned
void UMMTTrackAnimationComponent::BeginPlay()
{
	// Get references to named components in case BuildTrackMeshAndSpline() wasn't called from constructor
	GetComponentsReference();

	Super::BeginPlay();
}

// Called when the game starts or when spawned
void UMMTTrackAnimationComponent::PostLoad()
{
	Super::PostLoad();
}

// Called when the game starts or when spawned
void UMMTTrackAnimationComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

//Find and store reference to named components
void UMMTTrackAnimationComponent::GetComponentsReference()
{
	//Get mesh components references
	if (SprocketsIdlersRoadwheelsNames.Num() > 0)
	{
		for (int32 i = 0; i < SprocketsIdlersRoadwheelsNames.Num(); i++)
		{
			if (SprocketsIdlersRoadwheelsNames[i] != FString("none"))
			{
				UMeshComponent* ComponentReference = UMMTBPFunctionLibrary::GetMeshComponentReferenceByName(this, SprocketsIdlersRoadwheelsNames[i]);
				if (IsValid(ComponentReference))
				{
					SprocketsIdlersRoadwheelsComponents.Add(ComponentReference);
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s component failed to find component named '%s' or it's not derived from MeshComponent class"), *GetOwner()->GetName(), *GetName(), *SprocketsIdlersRoadwheelsNames[i]));
					UE_LOG(LogTemp, Warning, TEXT("%s->%s component failed to find component named '%s' or it's not derived from MeshComponent class"), *GetOwner()->GetName(), *GetName(), *SprocketsIdlersRoadwheelsNames[i]);
				}
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s component's array of mesh names references shouldn't be 'none', set proper name for effected component"), *GetOwner()->GetName(), *GetName()));
				UE_LOG(LogTemp, Warning, TEXT("%s->%s component's array of mesh names references shouldn't be 'none', set proper name for effected component"), *GetOwner()->GetName(), *GetName());
			}
		}
	}

	//Get track spline component reference
	if (TrackSplineComponentName != FString("none"))
	{
		TrackSplineComponent = UMMTBPFunctionLibrary::GetSplineComponentReferenceByName(this, TrackSplineComponentName);
		if (!IsValid(TrackSplineComponent))
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s component failed to find component named '%s' or it's not derived from SplineComponent class"), *GetOwner()->GetName(), *GetName(), *TrackSplineComponentName));
			UE_LOG(LogTemp, Warning, TEXT("%s->%s component failed to find component named '%s' or it's not derived from SplineComponent class"), *GetOwner()->GetName(), *GetName(), *TrackSplineComponentName);
		}
	}
	else
	{
		TrackSplineComponent = NULL;
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s component's Track Spline Component Name property shouldn't be 'none', set proper name for effected component"), *GetOwner()->GetName(), *GetName()));
		UE_LOG(LogTemp, Warning, TEXT("%s->%s component's Track Spline Component Name property shouldn't be 'none', set proper name for effected component"), *GetOwner()->GetName(), *GetName());
	}

	//Get treads instanced mesh component reference
	if (TreadsInstancedMeshComponentName != FString("none"))
	{
		TreadsInstancedMeshComponent = UMMTBPFunctionLibrary::GetInstancedStaticMeshComponentReferenceByName(this, TreadsInstancedMeshComponentName);
		if (!IsValid(TreadsInstancedMeshComponent))
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s component failed to find component named '%s' or it's not derived from InstancedStaticMeshComponent class"), *GetOwner()->GetName(), *GetName(), *TreadsInstancedMeshComponentName));
			UE_LOG(LogTemp, Warning, TEXT("%s->%s component failed to find component named '%s' or it's not derived from InstancedStaticMeshComponent class"), *GetOwner()->GetName(), *GetName(), *TreadsInstancedMeshComponentName);
		}
	}
	else
	{
		TreadsInstancedMeshComponent = NULL;
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s component's Treads Instanced Mesh Component Name property shouldn't be 'none', set proper name for effected component"), *GetOwner()->GetName(), *GetName()));
		UE_LOG(LogTemp, Warning, TEXT("%s->%s component's Treads Instanced Mesh Component Name property shouldn't be 'none', set proper name for effected component"), *GetOwner()->GetName(), *GetName());
	}

	//Get references for animated spline points
	if (AnimatedTrackSplinePoints.Num() > 0)
	{
		for (int32 i = 0; i < AnimatedTrackSplinePoints.Num(); i++)
		{
			if (AnimatedTrackSplinePoints[i].StaticMeshName != FString("none"))
			{
				UMeshComponent* ComponentReference = UMMTBPFunctionLibrary::GetMeshComponentReferenceByName(this, AnimatedTrackSplinePoints[i].StaticMeshName);
				if (IsValid(ComponentReference))
				{
					AnimatedTrackSplinePoints[i].StaticMesh = ComponentReference;
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s component failed to find component named '%s' or it's not derived from MeshComponent class"), *GetOwner()->GetName(), *GetName(), *SprocketsIdlersRoadwheelsNames[i]));
					UE_LOG(LogTemp, Warning, TEXT("%s->%s component failed to find component named '%s' or it's not derived from MeshComponent class"), *GetOwner()->GetName(), *GetName(), *AnimatedTrackSplinePoints[i].StaticMeshName);
				}
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s component's array of animated track spline points mesh names references shouldn't be 'none', set proper name for effected component"), *GetOwner()->GetName(), *GetName()));
				UE_LOG(LogTemp, Warning, TEXT("%s->%s component's array of animated track spline points mesh names references shouldn't be 'none', set proper name for effected component"), *GetOwner()->GetName(), *GetName());
			}
		}
	}
}

//Set angular velocity of track parts
void UMMTTrackAnimationComponent::SetTrackPartsAngularAndLinearVelocity(const float& AngularVelocity, const float& LinearVelocity)
{
	TrackPartsAngularVelocity = FMath::RadiansToDegrees(AngularVelocity);
	TreadsLinearVelocity = LinearVelocity;
}

//Updates animation of the track parts
void UMMTTrackAnimationComponent::UpdateTrackAnimation(const float& DeltaTime)
{

	//Rotate sprockets, idlers and roadwheels
	FRotator DeltaRotation = FRotator(DeltaTime * TrackPartsAngularVelocity * (IsFlipAnimation ? 1.0f : -1.0f), 0.0f, 0.0f);
	if (SprocketsIdlersRoadwheelsNames.Num() > 0)
	{
		for (int32 i = 0; i < SprocketsIdlersRoadwheelsComponents.Num(); i++)
		{
			if (IsValid(SprocketsIdlersRoadwheelsComponents[i]))
			{
				SprocketsIdlersRoadwheelsComponents[i]->AddLocalRotation(DeltaRotation, false);
			}
		}
	}

	//Animate spline points
	if (AnimatedTrackSplinePoints.Num() > 0)
	{
		if (IsValid(TrackSplineComponent))
		{
			for (int32 i = 0; i < AnimatedTrackSplinePoints.Num(); i++)
			{
				TrackSplineComponent->SetLocationAtSplinePoint(AnimatedTrackSplinePoints[i].SplinePointIndex, 
					AnimatedTrackSplinePoints[i].StaticMesh->GetSocketTransform(AnimatedTrackSplinePoints[i].StaticMeshSocketName, ERelativeTransformSpace::RTS_Actor).GetLocation(),
					ESplineCoordinateSpace::Local, ((i + 1) == AnimatedTrackSplinePoints.Num()) ? true : false);
			}
		}
	}

	//Calculate new position of tread instances
	if (IsValid(TreadsInstancedMeshComponent) && IsValid(TrackSplineComponent))
	{
		float SplineLength = TrackSplineComponent->GetSplineLength();

		//avoid looping around the spline to maintain precision
		TreadMeshPositionOffset = FMath::Fmod(TreadMeshPositionOffset + TreadsLinearVelocity * DeltaTime, SplineLength);

		
		for (int32 i = 0; i < TreadsOnSide; i++)
		{
			float DistanceAlongSpline = FMath::Fmod((float)i * (SplineLength / (float)TreadsOnSide) + TreadMeshPositionOffset, SplineLength);
			DistanceAlongSpline = DistanceAlongSpline < 0.0 ? SplineLength + DistanceAlongSpline : DistanceAlongSpline;

			TreadsInstancedMeshComponent->UpdateInstanceTransform(i, GetAllignedTransformAlongSpline(DistanceAlongSpline), false, (i + 1 == TreadsOnSide) ? true : false, false);
		}

	}
}

//Set default pose for spline and add tread meshes
void UMMTTrackAnimationComponent::BuildTrackMeshAndSpline()
{
	// Get references to named components
	GetComponentsReference();

	if (IsValid(TrackSplineComponent))
	{
		//Set spline points and their tangents
		if (TrackSplinePointLocations.Num() > 0)
		{
			TrackSplineComponent->SetSplinePoints(TrackSplinePointLocations, ESplineCoordinateSpace::Local, true);
					
			if (TrackSplinePointTangents.Num() > 0)
			{
				for (int32 i = 0; i < TrackSplinePointLocations.Num(); i++)
				{
					TrackSplineComponent->SetTangentAtSplinePoint(i, TrackSplinePointTangents[i], ESplineCoordinateSpace::Local, true);
				}
			}
		}

		//Add tread instances
		if (IsValid(TreadsInstancedMeshComponent))
		{
			for (int32 i = 0; i < TreadsOnSide; i++)
			{
				float DistanceAlongSpline = TrackSplineComponent->GetSplineLength() / (float)TreadsOnSide * (float)i;

				//TreadsInstancedMeshComponent->AddInstance(FTransform(TreadRotator, TransformAlongSPline.GetTranslation(), TransformAlongSPline.GetScale3D()));
				TreadsInstancedMeshComponent->AddInstance(GetAllignedTransformAlongSpline(DistanceAlongSpline));
			}
		}
	}
}

//Set default pose for spline and add tread meshes
FTransform UMMTTrackAnimationComponent::GetAllignedTransformAlongSpline(const float& Distance)
{
	FTransform TransformAlongSPline = TrackSplineComponent->GetTransformAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);

	//Check if right vector is negative, as it happens when spline flips up-side down. If it's negative then roll instance 180 degrees
	FRotator TreadRotator = TransformAlongSPline.Rotator();
	TreadRotator.Roll = FQuat(TreadRotator).GetRightVector().Y < 0.0 ? 180.0f : TreadRotator.Roll;
	
	return FTransform(TreadRotator, TransformAlongSPline.GetTranslation(), TransformAlongSPline.GetScale3D());
}
