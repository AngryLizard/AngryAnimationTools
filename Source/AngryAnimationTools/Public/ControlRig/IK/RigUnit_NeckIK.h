// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Units/RigUnit.h"
#include "ControlRig/Utility.h"

#include "RigUnit_NeckIK.generated.h"

/**
 *
 */
USTRUCT(meta = (DisplayName = "Neck IK", Category = "IK", Keywords = "Angry,IK", PrototypeName = "NeckIK", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_NeckIK : public FRigUnitMutable
{
	GENERATED_BODY()

		FRigUnit_NeckIK() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

public:

	/**
	 * The chain to adapt (Has to be continuous chain)
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection Chain;

	/**
	 * Bend intensity applied to the chain
	 */
	UPROPERTY(meta = (Input))
		float Intensity = 0.75f;

	/**
	 * Max bending angle per segment
	 */
	UPROPERTY(meta = (Input))
		float BendAngleLimit = 90.0f;

	/**
	 * Max twist angle per segment
	 */
	UPROPERTY(meta = (Input))
		float TwistAngleLimit = 75.0f;

	/**
	 * Desired distance between EE and target
	 */
	UPROPERTY(meta = (Input))
		float Distance = 50.0f;

	/**
	 * local forward direction of endeffector
	 */
	UPROPERTY(meta = (Input))
		FVector EndeffectorForward = FVector::ForwardVector;

	/**
	 * local forward direction of endeffector
	 */
	UPROPERTY(meta = (Input))
		FVector EndeffectorUp = FVector::UpVector;

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
