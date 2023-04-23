

#pragma once

#include "RigUnit_IK.h"
#include "ControlRig/Utility.h"

#include "RigUnit_ArmIK.generated.h"

/**
 * Arm IK solver without clavikle. Properly rotates the ellbow according to hand rotation.
 */
USTRUCT(meta = (DisplayName = "Arm IK", Category = "IK", Keywords = "Angry,IK", PrototypeName = "ArmIK", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_ArmIK : public FRigUnit_IK
{
	GENERATED_BODY()

		FRigUnit_ArmIK() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

public:

	/**
	 * Minimum ellbow angle in degrees
	 */
	UPROPERTY(meta = (Input))
		float MinEllbowAngle = 25.0f;

	/**
	 * Objective axis to use for ellbow alignment
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector EllbowAlignmentAxis = FVector::ForwardVector;

	/**
	 * Offset rotation for ellbow direction
	 */
	UPROPERTY(meta = (Input))
		FRotator EllbowOffset = FRotator::ZeroRotator;

	/**
	 * Length customisation
	 */
	UPROPERTY(meta = (Input))
		FVector2D Customisation = FVector2D(1.0f, 1.0f);

	/**
	 * How to scale the bones
	 */
	UPROPERTY(meta = (Input))
		EBendScaleType ScaleType = EBendScaleType::Default;

	/**
	 * Stretch ratio, 1.0 for initial, 0.0 for all the way compressed
	 */
	UPROPERTY(meta = (Output))
		float Stretch = 1.0f;
};
