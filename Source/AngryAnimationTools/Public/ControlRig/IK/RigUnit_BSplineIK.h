

#pragma once

#include "RigUnit_IK.h"
#include "ControlRig/Utility.h"

#include "RigUnit_BSplineIK.generated.h"

/**
 *
 */
USTRUCT(meta = (DisplayName = "B-Spline IK", Category = "IK", Keywords = "Angry,IK", PrototypeName = "BSplineIK", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_BSplineIK : public FRigUnit_IK
{
	GENERATED_BODY()

		FRigUnit_BSplineIK() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

public:

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
};
