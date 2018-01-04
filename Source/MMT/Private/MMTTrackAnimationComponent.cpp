//Copyright(c) 2016 Viktor Kuropiatnyk "BoredEngineer"

#include "MMTPluginPCH.h"
#include "MMTBPFunctionLibrary.h"
#include "MMTTrackAnimationComponent.h"

//Set default values
UMMTTrackAnimationComponent::UMMTTrackAnimationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	TrackSplineComponentName = FString("none");
	TreadsInstancedMeshComponentName = FString("none");
	SecondTreadsInstancedMeshComponentName = FString("none");
	TreadsOnSide = 30;
	bFlipAnimation = false;
	bDebugMode = false;
	AngleBetweenSprocketTeeth = 18;
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
		
		//We allow "none" for cases when user wants to manage rendering of treads on his own
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s component's Treads Instanced Mesh Component Name property shouldn't be 'none', set proper name for effected component"), *GetOwner()->GetName(), *GetName()));
		//UE_LOG(LogTemp, Warning, TEXT("%s->%s component's Treads Instanced Mesh Component Name property shouldn't be 'none', set proper name for effected component"), *GetOwner()->GetName(), *GetName());
	}
	
	//Get secondary treads instanced mesh component reference
	if (SecondTreadsInstancedMeshComponentName != FString("none"))
	{
		SecondTreadsInstancedMeshComponent = UMMTBPFunctionLibrary::GetInstancedStaticMeshComponentReferenceByName(this, SecondTreadsInstancedMeshComponentName);
		if (!IsValid(SecondTreadsInstancedMeshComponent))
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%s->%s component failed to find component named '%s' or it's not derived from InstancedStaticMeshComponent class"), *GetOwner()->GetName(), *GetName(), *SecondTreadsInstancedMeshComponentName));
			UE_LOG(LogTemp, Warning, TEXT("%s->%s component failed to find component named '%s' or it's not derived from InstancedStaticMeshComponent class"), *GetOwner()->GetName(), *GetName(), *SecondTreadsInstancedMeshComponentName);
		}
	}
	else
	{
		SecondTreadsInstancedMeshComponent = NULL;
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
void UMMTTrackAnimationComponent::SetTrackPartsAngularVelocity(const float& AngularVelocity)
{
	TrackPartsAngularVelocityDegrees = FMath::RadiansToDegrees(AngularVelocity);
}

//Updates animation of the track parts
void UMMTTrackAnimationComponent::UpdateTrackAnimation(const float& DeltaTime, float& TreadAngularPosition, float& TrackPartsAngularPosition, FRotator& TrackPartsRotator)
{
	//Rotate sprockets, idlers and road-wheels
	float DeltaPitch = DeltaTime * TrackPartsAngularVelocityDegrees * (bFlipAnimation ? -1.0f : 1.0f);

	//Update angular travel of the track around the sprocket
	CalculateIntAndFracRotationOfTrack(DeltaPitch);

	TreadAngularPosition = TreadFractionalTravel + TreadOffsetCount * AngleBetweenSprocketTeeth;
	TrackPartsAngularPosition = TreadFractionalTravel + PartsOffsetCount * AngleBetweenSprocketTeeth;
	
	//Flip sign of rotation for rotator
	FRotator NewTrackElementsRotator = FRotator::MakeFromEuler(FVector(0.0f, TrackPartsAngularPosition * (-1.0f), 0.0f));
	TrackPartsRotator = NewTrackElementsRotator;

	if (SprocketsIdlersRoadwheelsNames.Num() > 0)
	{
		for (int32 i = 0; i < SprocketsIdlersRoadwheelsComponents.Num(); i++)
		{
			if (IsValid(SprocketsIdlersRoadwheelsComponents[i]))
			{
				SprocketsIdlersRoadwheelsComponents[i]->SetRelativeRotation(NewTrackElementsRotator, false);
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

	PlacesInstancesAlongSpline(false);

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

		PlacesInstancesAlongSpline(true);
	}
}

//Calculate transform of the instance using tangent of the spline
FTransform UMMTTrackAnimationComponent::GetAllignedTransformAlongSplineUsingTangent(const float& Distance)
{
	FTransform TransformAlongSPline = TrackSplineComponent->GetTransformAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);

	//Check if right vector is negative, as it happens when spline flips up-side down. If it's negative then roll instance 180 degrees
	FRotator TreadRotator = TransformAlongSPline.Rotator();
	TreadRotator.Roll = FQuat(TreadRotator).GetRightVector().Y < 0.0 ? 180.0f : TreadRotator.Roll;
	
	return FTransform(TreadRotator, TransformAlongSPline.GetTranslation(), TransformAlongSPline.GetScale3D());
}

//Calculate transform of the instance using position of current instance and previous instance
FTransform UMMTTrackAnimationComponent::GetAllignedTransformAlongSplineUsingPosition(const float& Distance, FVector PositionOfPrevInstance, FVector& OutPositionOfCurrentInstance)
{
	OutPositionOfCurrentInstance = TrackSplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);

	//find angle between X axis and direction to position of previous instance
	float InstancePitch = FMath::Atan2(OutPositionOfCurrentInstance.Z - PositionOfPrevInstance.Z, OutPositionOfCurrentInstance.X - PositionOfPrevInstance.X);
	
	FRotator TreadRotator = FRotator(FMath::RadiansToDegrees(InstancePitch), 0.0f, 0.0f);

	return FTransform(TreadRotator, OutPositionOfCurrentInstance);
}


//Update angular travel of the full track. Rotation of the track is not limited to range of 360 degrees but to angle between sprocket teeth multiplied by number of treads
//Perceive it as a angular travel of the tread around a sprocket if full track would be wrapped around sprocket like a snake.
void UMMTTrackAnimationComponent::CalculateIntAndFracRotationOfTrack(float DeltaPitch)
{
	if (!(DeltaPitch == 0.0f))
	{
		float NewPitch = TreadFractionalTravel + DeltaPitch;

		if (NewPitch > AngleBetweenSprocketTeeth)
		{
			//When new pitch is larger than a discrete angle between sprocket teeth, we advance integer offset of rotation forward and re-calculate fractional part

			//Floor division in case of large angular velocity as we can skip over single AngleBetweenSprocketTeeth multiple times
			NumberOfFullOffsets = FMath::FloorToInt(NewPitch / AngleBetweenSprocketTeeth);
			TreadOffsetCount += NumberOfFullOffsets;
			PartsOffsetCount += NumberOfFullOffsets;
			TreadFractionalTravel = NewPitch - NumberOfFullOffsets * AngleBetweenSprocketTeeth;
		}
		else
		{
			if (NewPitch < 0.0f)
			{
				//When new pitch is smaller than zero, we advance integer offset of rotation backward and re-calculate fractional part
				NewPitch *= -1.f;
				NumberOfFullOffsets = FMath::FloorToInt(NewPitch / AngleBetweenSprocketTeeth);
				TreadOffsetCount -= NumberOfFullOffsets + 1;
				PartsOffsetCount -= NumberOfFullOffsets + 1;
				//Our TrackFractionalTravel is always between 0 and 1 representing movement of the single tread along the angular sector occupied by single tread
				TreadFractionalTravel = AngleBetweenSprocketTeeth - (NewPitch - NumberOfFullOffsets * AngleBetweenSprocketTeeth);
			}
			else
			{
				TreadFractionalTravel = NewPitch;
			}
		}

		//Treads loop around the whole length of the track this is where we keep them in proper range		
		if (TreadOffsetCount < 0)
		{
			TreadOffsetCount = TreadsOnSide + TreadOffsetCount;
		}
		else
		{
			if (TreadOffsetCount > (TreadsOnSide - 1))
			{
				TreadOffsetCount = 0;
			}
		}

		//Track parts such as sprocket but not treads, loop around 360 degrees
		if (PartsOffsetCount < 0)
		{
			PartsOffsetCount = FMath::FloorToInt(360.0f / AngleBetweenSprocketTeeth) + PartsOffsetCount;
		}
		else
		{
			if (PartsOffsetCount > (FMath::FloorToInt(360.0f / AngleBetweenSprocketTeeth) - 1))
			{
				PartsOffsetCount = 0;
			}
		}

	}
}

void UMMTTrackAnimationComponent::PlacesInstancesAlongSpline(bool CreateInstances)
{
	//Calculate new position of tread instances
	if (IsValid(TrackSplineComponent))
	{
		float SplineLength = TrackSplineComponent->GetSplineLength();

		//Measure of how much single sprocket tooth have rotated
		float RotationDivision = TreadFractionalTravel / AngleBetweenSprocketTeeth;

		//Distance can change on every frame as spline stretches
		float DistanceBetweenInstances = SplineLength / (float)TreadsOnSide;
		float SecondaryPartOffset = SecondTrackPartOffset;

		//Calculate how much single instance have moved between discrete rotations by sprocket between teeth angle
		float InstancePositionLerp = FMath::Frac(RotationDivision);
		float InstancePositionLocalOffset = FMath::Lerp(0.0f, DistanceBetweenInstances, InstancePositionLerp);

		//How many full discrete rotations by sprocket between teeth angle, first instance have moved
		int FirstInstancePlaceOffset = TreadOffsetCount;

		FVector PositionOfPrevInstance;
		FVector PositionOfCurrentInstance;
		FVector PositionOfCurrentSecondaryInstance;
		for (int32 i = 0; i < TreadsOnSide; i++)
		{
			//Move each instance by amount that first instance have traveled + own offset based on index
			//FMod result to prevent distance along spline getting larger than length of the spline as GetLocationAlongSpline won't return correct result
			float DistanceAlongSpline = FMath::Fmod((float)(i + FirstInstancePlaceOffset) * DistanceBetweenInstances + InstancePositionLocalOffset, SplineLength);

			FTransform TreadAllignedTransform;
			FTransform SecondaryTreadAllignedTransform;


			if (!bTwoPartsTrack)
			{
				//If this is a first instance, we need to align it with position of last instance as track loops
				if (i == 0)
				{
					float LastInstanceDistance = FMath::Fmod((float)((TreadsOnSide - 1) + FirstInstancePlaceOffset) * DistanceBetweenInstances + InstancePositionLocalOffset, SplineLength);
					PositionOfPrevInstance = TrackSplineComponent->GetLocationAtDistanceAlongSpline(LastInstanceDistance, ESplineCoordinateSpace::Local);
				}

				TreadAllignedTransform = GetAllignedTransformAlongSplineUsingPosition(DistanceAlongSpline, PositionOfPrevInstance, PositionOfCurrentInstance);

				if (IsValid(TreadsInstancedMeshComponent))
				{
					if(CreateInstances)
					{
						TreadsInstancedMeshComponent->AddInstance(TreadAllignedTransform);
						if(bAnimationLagCompensation)
						{TreadAllignedTransformPreviousUpdate.Add(TreadAllignedTransform);}
					}
					else
					{
						if (TreadsInstancedMeshComponent->GetInstanceCount() > 0) //was TreadsInstancedMeshComponent->GetNumRenderInstances() before 4.18
						{
							TreadsInstancedMeshComponent->UpdateInstanceTransform(i, bAnimationLagCompensation ? TreadAllignedTransformPreviousUpdate [i] : TreadAllignedTransform,
								false, (i + 1 == TreadsOnSide) ? true : false, false);
						}
					}
				}

				PositionOfPrevInstance = PositionOfCurrentInstance;

				if (bDebugMode)
				{
					FTransform ReferenceFrameTransform = UMMTBPFunctionLibrary::MMTGetTransformComponent(TreadsInstancedMeshComponent, NAME_None);
					DrawDebugString(GetWorld(), ReferenceFrameTransform.TransformPosition(TreadAllignedTransform.GetLocation()), FString::FromInt(i), nullptr, FColor::White, 0.0f, false);
				}
			}
			else
			{
				float SecondaryDistanceAlongSpline = FMath::Fmod((float)(i + FirstInstancePlaceOffset) * DistanceBetweenInstances + InstancePositionLocalOffset - SecondaryPartOffset, SplineLength);
				if (SecondaryDistanceAlongSpline < 0.0)
				{
					SecondaryDistanceAlongSpline = SplineLength + SecondaryDistanceAlongSpline;
				}

				//If this is a first instance, we need to align it with position of last instance as track loops
				if (i == 0)
				{
					float LastInstanceDistance = FMath::Fmod((float)((TreadsOnSide - 1) + FirstInstancePlaceOffset) * DistanceBetweenInstances + InstancePositionLocalOffset, SplineLength);
					PositionOfPrevInstance = TrackSplineComponent->GetLocationAtDistanceAlongSpline(LastInstanceDistance, ESplineCoordinateSpace::Local);
				}
				//First find transform and position of secondary instance as it's connected to previous primary instance
				SecondaryTreadAllignedTransform = GetAllignedTransformAlongSplineUsingPosition(SecondaryDistanceAlongSpline, PositionOfPrevInstance, PositionOfCurrentSecondaryInstance);
				//Primary instance is rotated in direction of secondary instance
				TreadAllignedTransform = GetAllignedTransformAlongSplineUsingPosition(DistanceAlongSpline, PositionOfCurrentSecondaryInstance, PositionOfCurrentInstance);

				if (IsValid(TreadsInstancedMeshComponent) & IsValid(SecondTreadsInstancedMeshComponent))
				{
					if (CreateInstances)
					{
						SecondTreadsInstancedMeshComponent->AddInstance(SecondaryTreadAllignedTransform);
						TreadsInstancedMeshComponent->AddInstance(TreadAllignedTransform);
						
						if (bAnimationLagCompensation)
						{
							SecondaryTreadAllignedTransformPreviousUpdate.Add(SecondaryTreadAllignedTransform);
							TreadAllignedTransformPreviousUpdate.Add(TreadAllignedTransform);
						}
					}
					else
					{
						if(SecondTreadsInstancedMeshComponent->GetInstanceCount() > 0) //was TreadsInstancedMeshComponent->GetNumRenderInstances() before 4.18
						{SecondTreadsInstancedMeshComponent->UpdateInstanceTransform(i, bAnimationLagCompensation ? SecondaryTreadAllignedTransformPreviousUpdate[i] : SecondaryTreadAllignedTransform,
							false, (i + 1 == TreadsOnSide) ? true : false, false);}

						if(TreadsInstancedMeshComponent->GetInstanceCount() > 0) //was TreadsInstancedMeshComponent->GetNumRenderInstances() before 4.18
						{TreadsInstancedMeshComponent->UpdateInstanceTransform(i, bAnimationLagCompensation ? TreadAllignedTransformPreviousUpdate[i] : TreadAllignedTransform,
							false, (i + 1 == TreadsOnSide) ? true : false, false);}
					}
				}

				PositionOfPrevInstance = PositionOfCurrentInstance;

				if (bDebugMode)
				{
					FTransform ReferenceFrameTransform = UMMTBPFunctionLibrary::MMTGetTransformComponent(TreadsInstancedMeshComponent, NAME_None);
					DrawDebugString(GetWorld(), ReferenceFrameTransform.TransformPosition(TreadAllignedTransform.GetLocation()), FString::FromInt(i), nullptr, FColor::White, 0.0f, false);
					DrawDebugString(GetWorld(), ReferenceFrameTransform.TransformPosition(SecondaryTreadAllignedTransform.GetLocation()), FString::FromInt(i), nullptr, FColor::Cyan, 0.0f, false);
				}

			}

			//Store calculated transforms to retrieve them later if user needs them
			if (CreateInstances)
			{
				TreadsTransforms.Add(TreadAllignedTransform);

				if (bTwoPartsTrack)
				{
					SecondaryTreadsTransforms.Add(SecondaryTreadAllignedTransform);
				}
			}
			else
			{
					
				if (bAnimationLagCompensation)
				{
					if (TreadsTransforms.IsValidIndex(i) & TreadAllignedTransformPreviousUpdate.IsValidIndex(i))
					{
						TreadsTransforms[i] = TreadAllignedTransformPreviousUpdate[i];
						
						if (bTwoPartsTrack & SecondaryTreadsTransforms.IsValidIndex(i) & SecondaryTreadAllignedTransformPreviousUpdate.IsValidIndex(i))
						{
							SecondaryTreadsTransforms[i] = SecondaryTreadAllignedTransformPreviousUpdate[i];
						}
					}
					
					if(TreadAllignedTransformPreviousUpdate.IsValidIndex(i))
					{
						TreadAllignedTransformPreviousUpdate[i] = TreadAllignedTransform;
					}
					
					if (bTwoPartsTrack & SecondaryTreadAllignedTransformPreviousUpdate.IsValidIndex(i))
					{
						SecondaryTreadAllignedTransformPreviousUpdate[i] = SecondaryTreadAllignedTransform;
					}

				}
				else 
				{
					if (TreadsTransforms.IsValidIndex(i))
					{
						TreadsTransforms[i] = TreadAllignedTransform;
					}

					if(bTwoPartsTrack & SecondaryTreadsTransforms.IsValidIndex(i))
					{ 
						SecondaryTreadsTransforms[i] = SecondaryTreadAllignedTransform;
					}
				}
			}
		}

	}
};

TArray<FTransform> UMMTTrackAnimationComponent::GetTreadsTransformArray()
{
	return TreadsTransforms;
}

TArray<FTransform> UMMTTrackAnimationComponent::GetSecondaryTreadsTransformArray()
{
	return SecondaryTreadsTransforms;
}


float UMMTTrackAnimationComponent::GetTrackAngularPosition()
{
	return TreadFractionalTravel;
}