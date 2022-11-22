// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Units/RigUnit.h"
#include "ControlRig/Utility.h"

#include "RigUnit_SplineIK.generated.h"

/**
 *
 */
USTRUCT(meta = (DisplayName = "Spline IK", Category = "IK", Keywords = "Angry,IK", PrototypeName = "SplineIK", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_SplineIK : public FRigUnitMutable
{
	GENERATED_BODY()

		FRigUnit_SplineIK() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

public:

	/**
	 * The chain to adapt (Has to be continuous chain)
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection Chain;

	/**
	 * Starting direction of the chain
	 */
	UPROPERTY(meta = (Input))
		FVector TangentStart = FVector::ForwardVector;

	/**
	 * Ending direction of the chain
	 */
	UPROPERTY(meta = (Input))
		FVector TangentEnd = FVector::BackwardVector;

	/**
	 * How much objective should rotate with last tangent instead of objective
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		float RotateWithTangent = 1.0f;

	/**
	 * Position along spline between (0 for start and 1 for end)
	 */
	UPROPERTY(meta = (Input))
		float PositionAlongSpline = 1.0f;

	/**
	 * Bendiness of the chain
	 */
	UPROPERTY(meta = (Input))
		float Bend = 1.0f;

	/**
	* How to scale the bones
	 */
	UPROPERTY(meta = (Input))
		EBendScaleType ScaleType = EBendScaleType::Default;

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
