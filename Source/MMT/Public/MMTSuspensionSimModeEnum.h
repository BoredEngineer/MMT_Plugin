//Copyright(c) 2017 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

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