// The Gateway of Realities: Planes of Existence.

#pragma once

#include "RigUnit_IK.h"
#include "ControlRig/Utility.h"

#include "RigUnit_ClavicleIK.generated.h"

/**
 *
 */
USTRUCT(meta = (DisplayName = "Clavice IK", Category = "IK", Keywords = "Angry,IK", PrototypeName = "ClaviceIK", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_ClaviceIK : public FRigUnit_IK
{
	GENERATED_BODY()

		FRigUnit_ClaviceIK() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

public:

	/**
	 * Minimum ellbow angle in degrees
	 */
	UPROPERTY(meta = (Input))
		float MinEllbowAngle = 25.0f;

	/**
	 * Maximum clavicle angle in degrees (can't be more than 90?)
	 */
	UPROPERTY(meta = (Input))
		float MaxClavicleAngle = 10.0f;

	/**
	 * Exponential bias for MaxClavicleAngle in vector direction (clavicle bone space)
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector ClavicleBias = FVector::ZeroVector;

	/**
	 * Objective axis to use for ellbow alignment
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector EllbowAlignmentAxis = FVector::ForwardVector;

	/**
	 * Length customisation
	 */
	UPROPERTY(meta = (Input))
		FVector Customisation = FVector(1.0f, 1.0f, 1.0f);

	/**
	* How to scale the bones
	 */
	UPROPERTY(meta = (Input))
		EBendScaleType ScaleType = EBendScaleType::Default;
};
