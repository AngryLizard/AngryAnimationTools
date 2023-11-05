

#pragma once

#include "Units/RigUnit.h"
#include "ControlRig/Utility.h"

#include "RigUnit_Constraints.generated.h"

/**
 * Limits a value with exponential decay
 */
USTRUCT(meta = (DisplayName = "Soft Limit Value", Category = "Constraints", Keywords = "Angry,Utility", PrototypeName = "SoftLimitValue", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_SoftLimitValue : public FRigUnit
{
	GENERATED_BODY()

		FRigUnit_SoftLimitValue() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:
	static float SoftLimit(float Value, float Limit);

	/**
	 * Value to limit
	 */
	UPROPERTY(meta = (Input))
		float Value = 0.0f;

	/**
	 * Limit in both negative and positive direction
	 */
	UPROPERTY(meta = (Input))
		float Limit = 1.0f;

	/**
	 * Output value
	 */
	UPROPERTY(meta = (Output))
		float Output = 0.0f;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * 
 */
USTRUCT(meta = (DisplayName = "Limit Rotation", Category = "Constraints", Keywords = "Angry,Utility", PrototypeName = "LimitRotation", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_LimitRotation : public FRigUnit
{
	GENERATED_BODY()

		FRigUnit_LimitRotation() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:
	static FQuat LimitRotation(const FQuat& Quat, float Limit, bool Soft = false);

	/**
	 */
	UPROPERTY(meta = (Input))
		FQuat Quat = FQuat::Identity;

	/**
	 */
	UPROPERTY(meta = (Input))
		float Limit = 0.0f;

	/**
	 */
	UPROPERTY(meta = (Input))
		bool Soft = false;

	/**
	 * Aligned rotation
	 */
	UPROPERTY(meta = (Output))
		FQuat Output = FQuat::Identity;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 *
 */
USTRUCT(meta = (DisplayName = "Limit Rotation Around Axis", Category = "Constraints", Keywords = "Angry,Utility", PrototypeName = "LimitRotationAroundAxis", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_LimitRotationAroundAxis : public FRigUnit
{
	GENERATED_BODY()

		FRigUnit_LimitRotationAroundAxis() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:
	static FQuat AxisLimitRotation(const FQuat& Quat, const FVector& Axis, float MinLimit, float MaxLimit, bool Soft = false);

	/**
	 */
	UPROPERTY(meta = (Input))
		FQuat Quat = FQuat::Identity;

	/**
	 */
	UPROPERTY(meta = (Input))
		FVector Axis = FVector::ZeroVector;

	/**
	* Limit
	 */
	UPROPERTY(meta = (Input))
		float MinLimit = 0.0f;

	/**
	 */
	UPROPERTY(meta = (Input))
		float MaxLimit = 0.0f;

	/**
	 */
	UPROPERTY(meta = (Input))
		bool Soft = false;

	/**
	 * Aligned rotation
	 */
	UPROPERTY(meta = (Output))
		FQuat Output = FQuat::Identity;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 *
 */
USTRUCT(meta = (DisplayName = "Bell Curve", Category = "Constraints", Keywords = "Angry,Utility", PrototypeName = "BellCurve", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_BellCurve : public FRigUnit
{
	GENERATED_BODY()

		FRigUnit_BellCurve() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:

	/**
	 * Input value
	 */
	UPROPERTY(meta = (Input))
		float Value = 0.0f;

	/**
	 * Bellcurve variance
	 */
	UPROPERTY(meta = (Input))
		float Variance = 1.0f;

	/**
	 * Output value
	 */
	UPROPERTY(meta = (Output))
		float Output = 0.0f;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Equivalent to using the value Bell Curve function on the distance between two points, but better performance
 */
USTRUCT(meta = (DisplayName = "Distance Bell Curve", Category = "Constraints", Keywords = "Angry,Utility", PrototypeName = "DistanceBellCurve", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_DistanceBellCurve : public FRigUnit
{
	GENERATED_BODY()

		FRigUnit_DistanceBellCurve() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:

	/**
	 * Input Location
	 */
	UPROPERTY(meta = (Input))
		FVector Location = FVector::ZeroVector;

	/**
	 * Reference Location
	 */
	UPROPERTY(meta = (Input))
		FVector Reference = FVector::ZeroVector;

	/**
	 * Bellcurve variance
	 */
	UPROPERTY(meta = (Input))
		float Variance = 1.0f;

	/**
	 * Output value
	 */
	UPROPERTY(meta = (Output))
		float Output = 0.0f;
};

