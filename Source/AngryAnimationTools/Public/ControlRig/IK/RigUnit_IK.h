// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Units/RigUnit.h"
#include "ControlRig/Utility.h"

#include "RigUnit_IK.generated.h"

#define GET_IK_OBJECTIVE_TRANSFORM() FTransform(OffsetRotation, OffsetTranslation) * Objective
#define GET_IK_OBJECTIVE_FORWARD() Objective.GetUnitAxis(EAxis::Y)
#define GET_IK_OBJECTIVE_RIGHT() -Objective.GetUnitAxis(EAxis::X)
#define GET_IK_OBJECTIVE_UP() Objective.GetUnitAxis(EAxis::Z)

/** Base class for all IK nodes */
USTRUCT(BlueprintType, meta = (Abstract))
struct ANGRYANIMATIONTOOLS_API FRigUnit_IK : public FRigUnitMutable
{
	GENERATED_BODY()

		FRigUnit_IK() {}

public:

	/**
	 * The chain to adapt (Has to be continuous chain)
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection Chain;

	/**
	 * Objective settings
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FTransform Objective;

	/**
	 * Local objective offset rotation
	 */
	UPROPERTY(meta = (Input, Constant, DetailsOnly))
		FRotator OffsetRotation;

	/**
	 * Local objective offset translation
	 */
	UPROPERTY(meta = (Input, Constant, DetailsOnly))
		FVector OffsetTranslation;

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
