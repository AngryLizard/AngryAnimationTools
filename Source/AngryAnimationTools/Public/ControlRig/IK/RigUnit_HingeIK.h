

#pragma once

#include "RigUnit_IK.h"
#include "ControlRig/Utility.h"

#include "RigUnit_HingeIK.generated.h"

/**
 * Simple analytic IK solver for a hinge joint.
 */
USTRUCT(meta = (DisplayName = "Hinge IK", Category = "IK", Keywords = "Angry,IK", PrototypeName = "HingeIK", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_HingeIK : public FRigUnit_IK
{
	GENERATED_BODY()

		FRigUnit_HingeIK() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:
	// Given two triangle sides a,b,s, return length and distance along s of the triangle altitude/apex
	static bool ComputeTriangle(float a, float b, float s, float& h, float& x);

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
		EBendScaleType ScaleType = EBendScaleType::Default;
};
