

#pragma once

#include "Units/RigUnit.h"
#include "ControlRig/Utility.h"

#include "RigUnit_Expressions.generated.h"

/**
 *
 */
USTRUCT(meta = (DisplayName = "Eye Look At", Category = "Expressions", Keywords = "Angry,Expression", PrototypeName = "EyeLookAt", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_EyeLookAt : public FRigUnitMutable
{
	GENERATED_BODY()

		FRigUnit_EyeLookAt() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:

	/**
	 * Eye bone
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey Key = FRigElementKey(FName(), ERigElementType::Bone);

	/**
	 * Look-at intensity
	 */
	UPROPERTY(meta = (Input))
		float Intensity = 1.0f;

	/**
	 * Offset bias in degrees
	 */
	UPROPERTY(meta = (Input))
		FVector2D Bias = FVector2D::ZeroVector;

	/**
	 * Max range in each direction in degrees
	 */
	UPROPERTY(meta = (Input))
		FVector2D Range = FVector2D(40.0f, 30.0f);

	/**
	 * Look at target
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FVector Target;

	/**
	 * Alignment offset rotation
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRotator AxisOffset;

	/**
	 * Whether to propagate applied transform
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		bool bPropagateToChildren = false;

	/**
	 * Local objective offset rotation
	 */
	UPROPERTY(meta = (Input, Constant, DetailsOnly))
		FRotator OffsetRotation;

	/**
	 * Iris direction
	 */
	UPROPERTY(meta = (Output))
		FVector Direction = FVector::ForwardVector;

	/**
	 * Debug settings
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FDebugSettings DebugSettings;

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement Cache;
};

/**
 *
 */
USTRUCT(meta = (DisplayName = "Eyelid Displacement", Category = "Expressions", Keywords = "Angry,Expression", PrototypeName = "EyelidDisplacement", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_EyelidDisplacement : public FRigUnitMutable
{
	GENERATED_BODY()

		FRigUnit_EyelidDisplacement() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:

	/**
	 * Bone for eyelid to move
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey Key = FRigElementKey(FName(), ERigElementType::Bone);

	/**
	 * Amount the eye lid is open (from fully open to iris, used for emoting)
	 */
	UPROPERTY(meta = (Input))
		float Openness = 0.5f;

	/**
	 * Amount the lid is closed (from openess to fully closed, used for blinking)
	 */
	UPROPERTY(meta = (Input))
		float Closeness = 0.0f;

	/**
	 * Distance in degrees which the eyelid should be pushed away from the iris
	 */
	UPROPERTY(meta = (Input))
		float IrisDisplacement = 0.0f;

	/**
	 * Offset iris direction in degrees
	 */
	UPROPERTY(meta = (Input))
		float IrisBias = 0.0f;

	/**
	 * Max range in each direction in degrees
	 */
	UPROPERTY(meta = (Input))
		float Range = 30.0f;

	/**
	 * Iris direction
	 */
	UPROPERTY(meta = (Input))
		FVector IrisDirection;

	/**
	 * Alignment offset rotation
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRotator AxisOffset;

	/**
	 * Whether to propagate applied transform
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		bool bPropagateToChildren = false;

	/**
	 * Local objective offset rotation
	 */
	UPROPERTY(meta = (Input, Constant, DetailsOnly))
		FRotator OffsetRotation;

	/**
	 * Eyelid position
	 */
	UPROPERTY(meta = (Output))
		float Position = 0.0f;

	/**
	 * Debug settings
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FDebugSettings DebugSettings;

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement Cache;
};

