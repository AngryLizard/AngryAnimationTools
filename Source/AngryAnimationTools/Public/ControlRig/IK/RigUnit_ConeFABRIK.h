// The Gateway of Realities: Planes of Existence.

#pragma once

#include "RigUnit_IK.h"
#include "ControlRig/Utility.h"

#include "RigUnit_ConeFABRIK.generated.h"

/**
 * FABRIK with angle constraint
 */
USTRUCT(meta = (DisplayName = "Cone FABRIK", Category = "IK", Keywords = "Angry,IK", PrototypeName = "ConeFABRIK", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_ConeFABRIK : public FRigUnit_IK
{
	GENERATED_BODY()

		FRigUnit_ConeFABRIK() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

public:
	static FQuat SoftRotate(const FTransform& Local, const FTransform& Transform, const FTransform& Anchor, float MaxAngle);

	/*
	 * Max angle change per segment
	 */
	UPROPERTY(meta = (Input))
		float MaxAngle = 20.0f;

	/**
	 * Fabrik iterations
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		int32 Iterations = 10;
};
