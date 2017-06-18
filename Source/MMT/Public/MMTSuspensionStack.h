//Copyright(c) 2017 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

#include "Components/MeshComponent.h"
#include "Containers/UnrealString.h"
#include "Engine/EngineTypes.h"
#include "Runtime/Engine/Public/CollisionQueryParams.h"
#include "MMTSuspensionStack.generated.h"

UENUM(BlueprintType)
enum class ESuspensionSimMode : uint8
{
	// Ray Check - is simplest and fastest but works correctly only on relatively smooth terrain. 
	RayCheck 	UMETA(DisplayName = "Ray Check"),
	// Sphere Check - wheels are approximated with a sphere. This improves traverse of the obstacles over RayCheck and still cheaper than Shape Sweep, although still not as reliable as Shape Sweep
	SphereCheck	UMETA(DisplayName = "Sphere Check"),
	// Shape Sweep - works with any type of terrain and obstacles but more expensive than RayCheck. 
	ShapeSweep	UMETA(DisplayName = "Shape Sweep")
};


USTRUCT(BlueprintType)
struct FSuspensionStackStruct
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Disables component from doing physics calculations and applying forces, useful for debugging."))
		bool bDisabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Suspension simulation mode in which suspension stack starts by default. Simulation mode can be later changed in real-time (useful for LODs). RayCheck - is simplest and fastest but works correctly only on relatively smooth terrain. Shape Sweep - works with any type of terrain and obstacles but more expensive than RayCheck. Tread Simulation - road-wheels will collide with dynamic track surface, provides the most accurate simulation compared to other modes (requires track simulation component to work)"))
		ESuspensionSimMode SimulationMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "By default, component's position is used as stack position. Set to TRUE when array of stacks is used inside of a single component, otherwise all stacks will be in the same spot"))
		uint32 bUseCustomPosition : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (EditCondition = "bUseCustomPosition", ToolTip = "Local position of the stack in relation to component. Normally a point where suspension would be physically attached to the chassis"))
		FVector StackLocalPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Radius of the road-wheel in cm. Needed for when track is using simulation feature or when IsShapeTrace is off and ray-check is used instead of sweeps"))
		float RoadWheelRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Thickness of the track in cm. Needed for offset calculations to prevent wheels from clipping track"))
		float TrackThickness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Top limit of the spring in relation to suspension stack position. Doesn't have to be aligned with Z axis. In case of torsion bar suspension, spring can be angled to follow closer radial movement of suspension"))
		FVector SpringTopOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Bottom limit of the spring in relation to suspension stack position. Doesn't have to be aligned with Z axis. In case of torsion bar suspension, spring can be angled to follow closer radial movement of suspension"))
		FVector SpringBottomOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Force of the spring. Minimal value should be enough to support portion of the vehicle's mass. Can calculate as SpringStiffnessMinimal = Vehicle_Mass * 980 / total_number_suspesion_stacks_of_all_tracks"))
		float SpringStiffness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "As spring compresses it produces more and more force which can push vehicle more than desired. This parameter defines maximum force that can be produced by the spring as ratio of SpringStiffness. Value of 1.05 means that maximum force will be not more than 5% higher than SpringStiffness."))
		float SpringMaxOutputRatio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Dampening coefficient of the spring. The smaller the value the more spring will oscilate. Too large value can lead to unstable behaviour. Value of about 1/10th to 1/100th of spring stiffness should be enough"))
		float SpringDampening;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Will return velocity of the body that wheel made contact with. Takes performance but allows to drive vehicle on moving platforms correctly."))
		bool bGetContactBodyVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Will output debug information on screen"))
		bool bEnableDebugMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ray Check mode", meta = (ToolTip = "Default channel for suspension tracing in Ray Check mode"))
		TEnumAsByte <ECollisionChannel> RayCheckTraceChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Check mode", meta = (ToolTip = "Can use sphere check to find suspension compression?"))
		bool bCanSphereCheck;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Check mode", meta = (EditCondition = "bCanSphereCheck", ToolTip = "Default channel for suspension tracing in Sphere Check mode"))
		TEnumAsByte <ECollisionChannel> SphereCheckTraceChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape Sweep mode", meta = (ToolTip = "Can use shape sweep to find suspension compression?"))
		bool bCanShapeSweep;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape Sweep mode", meta = (EditCondition = "bCanShapeSweep", ToolTip = "Name of the mesh component that will be used for shape sweep of this suspension stack"))
		FString SweepShapeComponentName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shape Sweep mode", meta = (ToolTip = "Will rotate sweep shape such that its Z vector will be parallel to direction of sweep, otherwise shape is not rotated"))
		bool bRotateAlongTraceVector;


	//Default constructor
	FSuspensionStackStruct()
	{
		bDisabled = false;
		SimulationMode = ESuspensionSimMode::RayCheck;
		bUseCustomPosition = false;
		StackLocalPosition = FVector::ZeroVector;
		RoadWheelRadius = 15.0;
		TrackThickness = 3.0;
		SpringTopOffset = FVector(0, 0, 10);
		SpringBottomOffset = FVector(0, 0, -10);
		SpringStiffness = 1000000.0f;
		SpringMaxOutputRatio = 1.05f;
		SpringDampening = 10000.0f;
		bGetContactBodyVelocity = false;
		bEnableDebugMode = false;
		RayCheckTraceChannel = ECC_Visibility;
		bCanSphereCheck = false;
		SphereCheckTraceChannel = ECC_Visibility;
		bCanShapeSweep = false;
		SweepShapeComponentName = FString("none");
	}

};


UCLASS(EditInlineNew)
class MMT_API UMMTSuspensionStack : public UObject
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMMTSuspensionStack();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension Stack Settings", meta = (ToolTip = "Name of the mesh component that will receive suspension force, usually root component"))
		FString SprungComponentName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension Stack Settings", meta = (ToolTip = "Suspension stack settings"))
		FSuspensionStackStruct SuspensionSettings;

	/**
	*	Runs calculations of suspension stack, applies spring force to sprung component and calculates new position of the road-wheels
	*	@param DeltaTime				Delta time
	*	@return	WheelLocalPosition		Position of the road-wheel in local coordinate system of the actor, NOT a relative position to component
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Suspension Stack")
		void PhysicsUpdate(const float& DeltaTime);

	/**
	*	Initializes class logic, finds references and pre-calculates parameters. Expected to be called from BeginPlay only
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Suspension Stack")
		void Initialize();

	/**
	*	Toggle debug mode on/off
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Suspension Stack")
		void ToggleDebugMode();

	/**
	*	Get last calculated suspension force
	*	@return	Magnitude	Magnitude of the force as float
	*	@return	WorldSpace	Vector force in world space
	*	@return	LocalSpace	Vector force in local space
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Suspension Stack")
	void GetSuspensionForce(float &Magnitude, FVector& WorldSpace, FVector& LocalSpace);

	/**
	*	Get contact data at point where trace or sweep has succeeded. This information is necessary for friction component
	*	@return	bPointActive			Indicates if contact with surface was made
	*	@return	ForceAtPoint			Force at contact point in world space (suspension force pushing against surface)
	*	@return	PointLocation			Location of contact point in world space
	*	@return SurfaceNormal			Normal of the surface at contact point
	*	@return	SurfacePhysicalMaterial		Physical material of contact surface
	*	@return	SurfaceVelocity			Velocity of the body that contact was made with
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Suspension Stack")
		void GetContactData(bool& bPointActive, FVector& ForceAtPoint, FVector& PointLocation, FVector& SurfaceNormal, class UPhysicalMaterial*& SurfacePhysicalMaterial, FVector& SurfaceVelocity);

	/**
	*	Get last calculated suspension force
	*	@return		Wheel hub position in local space
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Suspension Stack")
		FVector GetWheelHubPosition(bool bInWorldSpace=false);

	/**
	*	Set reference of sprung mesh component manually
	*	@param SprungMeshComponentRef	Pointer to sprung mesh component, usually root physics component
	*	@return		Returns true if reference is valid and was set
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Suspension Stack")
		bool SetSprungComponentReference(UMeshComponent* SprungMeshComponentRef);

	/**
	*	Set reference of mesh component used for sweeps
	*	@param SweepShapeMeshComponentRef	Pointer to mesh component used for sweeps, usually a physical wheel component
	*	@return		Returns true if reference is valid and was set
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Suspension Stack")
		bool SetSweepShapeComponentReference(UMeshComponent* SweepShapeMeshComponentRef);

	/**
	*	Set stiffness of the spring
	*	@param NewSpringStiffness	Stiffness of the spring
	*	@param bUpdateAllParameters	Will refresh all precomputed parameters. If you are changing multiple suspension settings at a time, consider setting this flag to true only on last setter to save some performance
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Suspension Stack")
		void SetSpringStiffness(float NewSpringStiffness=0.0f);

	/**
	*	Set new spring offsets
	*	@param NewSpringStiffness	Stiffness of the spring
	*	@param bUpdateAllParameters	Will refresh all precomputed parameters. If you are changing multiple suspension settings at a time, consider setting this flag to true only on last setter to save some performance
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Suspension Stack")
		void SetSpringOffsets(FVector SpringTopOffset = FVector::ZeroVector, FVector SpringBottomOffset = FVector::ZeroVector, bool bUpdateAllParameters=true);

	/**
	*	Sets scale of the final suspension force (spring + damping), useful for active suspension
	*	@param NewSpringForceScale	New scale value of final suspension force
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Suspension Stack")
		void SetSuspensionForceScale(float NewSuspensionForceScale = 1.0f);

	/**
	*	Get current suspension force scale parameter
	*	@return		Returns current suspension force scale parameter
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Suspension Stack")
		float GetSuspensionForceScale();

	/**
	*	Get current suspension compression ratio 0..1
	*	@return		Returns current suspension compression ratio
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Suspension Stack")
		float GetSuspensionCompressionRatio();

	/**
	*	Applies anti-roll force transfered from another suspension stack on the same axis
	*	@param AntiRollForceMagnitude	Magnitude of anti-roll force, direction is determined by suspension configuration
	*/
	UFUNCTION(BlueprintCallable, Category = "MMT Suspension Stack")
		void ApplyAntiRollForce(float AntiRollForceMagnitude = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "MMT Suspension Stack")
		void Test();

private:
	UPROPERTY()
		USceneComponent* ParentComponentRef;
	UPROPERTY()
		UMeshComponent* SprungMeshComponent;
	UPROPERTY()
		bool	bSprungMeshComponentSetManually = false;
	UPROPERTY()
		UMeshComponent* SweepShapeMeshComponent;
	UPROPERTY()
		bool	bSweepShapeMeshComponentSetManually = false;
	UPROPERTY()
		FString ComponentName = FString("ComponentRefereneFailed");
	UPROPERTY()
		FString ComponentsParentName = FString("ParentRefereneFailed");
	UPROPERTY()
		float SpringMaxLenght;
	UPROPERTY()
		FVector SpringOffsetTopAdjusted;
	UPROPERTY()
		FVector SpringOffsetBottomAdjusted;
	UPROPERTY()
		FTransform ReferenceFrameTransform;
	UPROPERTY()
		FVector SpringDirectionLocal;
	UPROPERTY()
		bool bWarningMessageDisplayed = false;
	UPROPERTY()
		FVector WheelHubPositionLS = FVector::ZeroVector;
	UPROPERTY()
		float PreviousSpringLenght;
	UPROPERTY()
		float SuspensionForceMagnitude = 0.0f;
	UPROPERTY()
		FVector SuspensionForceLS = FVector::ZeroVector;
	UPROPERTY()
		FVector SuspensionForceWS = FVector::ZeroVector;
	UPROPERTY()
		float SuspensionForceScale = 1.0f;
	UPROPERTY()
		float CompressionRatio;

	//Ray check mode specific
	FCollisionQueryParams LineTraceQueryParameters;
	FCollisionResponseParams LineTraceResponseParameters;
	UPROPERTY()
		FVector LineTraceOffsetTopLS;
	UPROPERTY()
		FVector LineTraceOffsetBottomLS;

	//Variable to store location of the trace hit
	UPROPERTY()
		FVector TracedHubLocation;

	//Sphere check mode specific
	FCollisionQueryParams SphereTraceQueryParameters;
	FCollisionResponseParams SphereTraceResponseParameters;
	FCollisionShape SphereCheckShape;

	//Shape sweep mode specific
	FComponentQueryParams ShapeSweepQueryParameters;
	UPROPERTY()
		FVector ShapeSweepLocation;

	//Variables for recording collision of the wheel and passing data further to friction processing
	UPROPERTY()
		bool bContactPointActive = false;
	UPROPERTY()
		FVector ContactInducedVelocity;
	UPROPERTY()
		FVector ContactForceAtPoint;
	UPROPERTY()
		FVector ContactPointLocation;
	UPROPERTY()
		FVector ContactPointNormal;
	UPROPERTY()
		UPhysicalMaterial* ContactPhysicalMaterial;
	

	//Find reference to named components
	void GetNamesForComponentAndParent();

	void GetNamedComponentsReference();

	//Recalculate parameters to save performance
	void PreCalculateParameters();

	//Find new position of road-wheel according to current simulation mode
	void UpdateWheelHubPosition();

	//Find wheel hub position and contact information for RayCheck suspension mode
	void LineTraceForContact();
	void AsyncLineTraceForContact();

	//Find wheel hub position and contact information for Sphere Check suspension mode
	void SphereTraceForContact();

	//Find wheel hub position and contact information for Shape Sweep suspension mode
	void ShapeSweepForContact();

	//Calculate suspension force and apply to effected body
	void CalculateAndApplySuspensionForce(const float& DeltaTime);

	void GetDefaultWheelPosition();

	//Draw simple debug lines for suspension traces
	static void DrawDebugLineTrace(bool bBlockingHit, FVector Start, FVector End, FVector HitPoint, UWorld *WorldRef);
	
	//Draw simple debug spheres for suspension traces
	static void DrawDebugSphereTrace(bool bBlockingHit, FVector SphereStart, FVector SphereEnd, FVector SphereCenter, float SphereRadius, FVector HitPoint, UWorld *WorldRef);

	//Draw simple debug lines for shape sweep of suspension traces
	static void DrawDebugShapeSweep(bool bBlockingHit, FVector Start, FVector End, FVector Location, FVector HitPoint, UWorld *WorldRef);
	
	//DoOnce for debug messages
	template<typename CallbackType> void DoOnce(const CallbackType& Callback) { static bool bDone = false; if (!bDone) { Callback(); bDone = true; } };

	//Experimental delegate for async trace
	FTraceDelegate TraceDelegate;
	void AsyncTraceDone(const FTraceHandle& TraceHandle, FTraceDatum & TraceData);
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension Stack Settings", meta = (ToolTip = "Suspension stack settings"))
	bool TryAsyncTraceMode = false;
};
