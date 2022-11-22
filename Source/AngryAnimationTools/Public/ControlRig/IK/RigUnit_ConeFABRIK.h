// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Units/RigUnit.h"
#include "ControlRig/Utility.h"

#include "RigUnit_ConeFABRIK.generated.h"

/**
 * FABRIK with angle constraint
 */
USTRUCT(meta = (DisplayName = "Cone FABRIK", Category = "IK", Keywords = "Angry,IK", PrototypeName = "ConeFABRIK", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_ConeFABRIK : public FRigUnitMutable
{
	GENERATED_BODY()

		FRigUnit_ConeFABRIK() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

public:
	static FQuat SoftRotate(const FTransform& Local, const FTransform& Transform, const FTransform& Anchor, float MaxAngle);

	/**
	 * The chain to adapt (Has to be continuous chain)
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection Chain;

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
