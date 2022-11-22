// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Units/RigUnit.h"
#include "ControlRig/Utility.h"

#include "RigUnit_SpineIK.generated.h"

USTRUCT()
struct ANGRYANIMATIONTOOLS_API FSpineEffectorSettings
{
	GENERATED_BODY()

		FSpineEffectorSettings()
	{
	}
	/*
	 * Max angle change per segment
	 */
	UPROPERTY(meta = (Input))
		float AngleLimit = 30.0f;

	/**
	 * Limit bias in degrees
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector LimitBias = FVector::ZeroVector;

	/**
	 * Limit radius in degrees
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		float LimitRadius = 0.0f;
};



/**
 *
 */
USTRUCT(meta = (DisplayName = "Spine IK", Category = "IK", Keywords = "Angry,IK", PrototypeName = "SpineIK", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_SpineIK : public FRigUnitMutable
{
	GENERATED_BODY()

		FRigUnit_SpineIK() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

public:

	/**
	 * The chain to adapt (Has to be continuous chain)
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection Chain;

	/*
	 * Constraint properties of the beginning of the chain
	 */
	UPROPERTY(meta = (Input))
		FSpineEffectorSettings AnchorSettings;

	/*
	 * Constraint properties of the end of the chain
	 */
	UPROPERTY(meta = (Input))
		FSpineEffectorSettings ObjectiveSettings;

	/**
	 * Bending iterations
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
