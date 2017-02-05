//Copyright(c) 2017 Viktor Kuropiatnyk "BoredEngineer"

#pragma once

UENUM(BlueprintType)
enum class ESuspensionSimMode : uint8
{
	// RayCheck - is simplest and fastest but works correctly only on relatively smooth terrain. 
	RayCheck 	UMETA(DisplayName = "RayCheck"),
	// Shape Sweep - works with any type of terrain and obstacles but more expensive than RayCheck. 
	ShapeSweep	UMETA(DisplayName = "Shape Sweep"),
	// Tread Simulation - road - wheels will collide with dynamic track surface, provides the most accurate simulation compared to other modes(requires track simulation component to work)
	TreadSim	UMETA(DisplayName = "Tread Simulation")
};