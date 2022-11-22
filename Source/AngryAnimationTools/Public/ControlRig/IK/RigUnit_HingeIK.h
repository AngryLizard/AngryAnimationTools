// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Units/RigUnit.h"
#include "ControlRig/Utility.h"

#include "RigUnit_HingeIK.generated.h"

/**
 *
 */
USTRUCT(meta = (DisplayName = "Hinge IK", Category = "IK", Keywords = "Angry,IK", PrototypeName = "HingeIK", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_HingeIK : public FRigUnitMutable
{
	GENERATED_BODY()

		FRigUnit_HingeIK() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

public:
	// Given two triangle sides a,b,s, return length and distance along s of the triangle altitude/apex
	static bool ComputeTriangle(float a, float b, float s, float& h, float& x);

	/**
	 * The chain to adapt (Has to be continuous chain)
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection Chain;

	/**
	 * Desired length of this chain
	 */
	UPROPERTY(meta = (Input))
		float TargetLength = 1.0f;

	/**
	 * Length customisation
	 */
	UPROPERTY(meta = (Input))
		FVector2D Customisation = FVector2D(1.0f, 1.0f);

	/**
	 * Direction to be used for the hinge
	 */
	UPROPERTY(meta = (Input))
		FVector Direction = FVector::ForwardVector;

	/**
	 * How soon hinge starts to stretch
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		float StretchThreshold = 0.01f;

	/**
	* How to scale the bones
	 */
	UPROPERTY(meta = (Input))
		EBendScaleType ScaleType = EBendScaleType::None;

	/**
	 * Objective settings
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FTransform Objective;

	/**
	 * Local objective offset
	 */
	UPROPERTY(meta = (Input))
		FTransform Offset;

	/**
	 * If set to true all of the global transforms of the children
	 * of this bone will be recalculated based on their local transforms.
	 */
	UPROPERTY(meta = (Input, Constant))
		EPropagation PropagateToChildren = EPropagation::All;

	/**
	 * Debug settings
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FDebugSettings DebugSettings;
};
