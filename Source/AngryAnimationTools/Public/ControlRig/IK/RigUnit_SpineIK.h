// The Gateway of Realities: Planes of Existence.

#pragma once

#include "RigUnit_IK.h"
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
struct ANGRYANIMATIONTOOLS_API FRigUnit_SpineIK : public FRigUnit_IK
{
	GENERATED_BODY()

		FRigUnit_SpineIK() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

public:

	/*
	 * Constraint properties of the beginning of the chain
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FSpineEffectorSettings AnchorSettings;

	/*
	 * Constraint properties of the end of the chain
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FSpineEffectorSettings ObjectiveSettings;

	/**
	 * Bending iterations
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		int32 Iterations = 10;

};
