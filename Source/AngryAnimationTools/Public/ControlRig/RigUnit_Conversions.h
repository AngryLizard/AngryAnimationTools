

#pragma once

#include "Units/RigUnit.h"
#include "ControlRig/Utility.h"

#include "RigUnit_Conversions.generated.h"

#define SQRT_THREE 1.73205f

/**
 *
 */
USTRUCT(meta = (DisplayName = "Set Transform With Offset", Category = "Conversions", Keywords = "TGOR,Utility", PrototypeName = "SetTransformWithOffset", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_SetTransformWithOffset : public FRigUnitMutable
{
	GENERATED_BODY()

		FRigUnit_SetTransformWithOffset() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:

	/**
	 * Key
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey Key = FRigElementKey(FName(), ERigElementType::Control);

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
	 * Output transform
	 */
	UPROPERTY(meta = (Input))
		FTransform Transform = FTransform::Identity;

	/**
	 * Whether to propagate applied transform
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		bool bPropagateToChildren = true;

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement Cache;
};

/**
 * 
 */
USTRUCT(meta = (DisplayName = "Clone Transforms", Category = "Conversions", Keywords = "Angry,Utility", PrototypeName = "CloneTransforms", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_CloneTransforms : public FRigUnitMutable
{
	GENERATED_BODY()

		FRigUnit_CloneTransforms() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:
	UPROPERTY(meta = (Input))
		FRigElementKeyCollection Items;

	UPROPERTY(meta = (Input))
		ERigElementType TargetType = ERigElementType::Bone;
};

/**
 * 
 */
USTRUCT(meta = (DisplayName = "Rebase", Category = "Conversions", Keywords = "TGOR,Utility", PrototypeName = "Rebase", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_Rebase : public FRigUnit
{
	GENERATED_BODY()

		FRigUnit_Rebase() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:

	/**
	 * Transform to convert
	 */
	UPROPERTY(meta = (Input))
		FTransform Transform = FTransform::Identity;

	/**
	 * Space transform comes from
	 */
	UPROPERTY(meta = (Input))
		FTransform FromSpace = FTransform::Identity;

	/**
	 * Space we transform to
	 */
	UPROPERTY(meta = (Input))
		FTransform ToSpace = FTransform::Identity;

	/**
	 * Conversion factor applied to translation
	 */
	UPROPERTY(meta = (Input))
		float TranslationScale = 0.0f;

	/**
	 * Output transform
	 */
	UPROPERTY(meta = (Output))
		FTransform Output = FTransform::Identity;
};

/**
 * 
 */
USTRUCT(meta = (DisplayName = "Affine Rebase", Category = "Conversions", Keywords = "TGOR,Utility", PrototypeName = "AffineRebase", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_AffineRebase : public FRigUnit
{
	GENERATED_BODY()

		FRigUnit_AffineRebase() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:
	
	/**
	 * Transform to convert
	 */
	UPROPERTY(meta = (Input))
		FTransform Transform = FTransform::Identity;

	/**
	 * Space transform comes from
	 */
	UPROPERTY(meta = (Input))
		FTransform FromSpace = FTransform::Identity;

	/**
	 * Space we transform to
	 */
	UPROPERTY(meta = (Input))
		FTransform ToSpace = FTransform::Identity;

	/**
	 * Conversion factor applied to translation
	 */
	UPROPERTY(meta = (Input))
		float TranslationScale = 0.0f;

	/**
	 * Output transform
	 */
	UPROPERTY(meta = (Output))
		FTransform Output = FTransform::Identity;
};

/**
 * 
 */
USTRUCT(meta = (DisplayName = "Axis Align Rotation", Category = "Conversions", Keywords = "TGOR,Utility", PrototypeName = "AxisAlignRotation", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_AxisAlignRotation : public FRigUnit
{
	GENERATED_BODY()

		FRigUnit_AxisAlignRotation() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:
	static FQuat ComputeHeadingRotation(const FVector& SourceForward, const FVector& TargetForward, const FVector& SourceUp, const FVector& TargetUp);

	/**
	 * Rotation forward axis to align with target forward
	 */
	UPROPERTY(meta = (Input))
		FVector SourceForward = FVector::ForwardVector;

	/**
	 * Direction to align rotation forward axis with
	 */
	UPROPERTY(meta = (Input))
		FVector TargetForward = FVector::ForwardVector;

	/**
	 * Rotation up axis to align with target up (projected around TargetForward)
	 */
	UPROPERTY(meta = (Input))
		FVector SourceUp = FVector::UpVector;

	/**
	 * Direction to align rotation up axis with
	 */
	UPROPERTY(meta = (Input))
		FVector TargetUp = FVector::UpVector;

	/**
	 * Aligned rotation
	 */
	UPROPERTY(meta = (Output))
		FQuat Output = FQuat::Identity;
};

/**
 * Rotation to align two vectors
 */
USTRUCT(meta = (DisplayName = "Rotation between", Category = "TGOR Utility", Keywords = "TGOR,Utility", PrototypeName = "RotationBetween", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_RotationBetween : public FRigUnit
{
	GENERATED_BODY()

		FRigUnit_RotationBetween() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:
	/**
	 * Source direction
	 */
	UPROPERTY(meta = (Input))
		FVector Source = FVector::ForwardVector;

	/**
	 * Target direction
	 */
	UPROPERTY(meta = (Input))
		FVector Target = FVector::ForwardVector;

	/**
	 * Rotation between
	 */
	UPROPERTY(meta = (Output))
		FQuat Output = FQuat::Identity;
};

/**
 * Reproject a trajectory onto a plane
 */
USTRUCT(meta = (DisplayName = "Project Onto Plane", Category = "TGOR Utility", Keywords = "TGOR,Utility", PrototypeName = "ProjectOntoPlane", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_ProjectOntoPlane : public FRigUnit
{
	GENERATED_BODY()

		FRigUnit_ProjectOntoPlane() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:

	/**
	 * Point to reproject
	 */
	UPROPERTY(meta = (Input))
		FVector Point = FVector::ZeroVector;

	/**
	 * Trajectory reference point, maintains distance along projection direction
	 */
	UPROPERTY(meta = (Input))
		FVector Reference = FVector::ZeroVector;

	/**
	 * Projection direction
	 */
	UPROPERTY(meta = (Input))
		FVector Direction = FVector::UpVector;

	/**
	 * Plane location
	 */
	UPROPERTY(meta = (Input))
		FVector Location = FVector::ZeroVector;

	/**
	 * Plane normal
	 */
	UPROPERTY(meta = (Input))
		FVector Normal = FVector::UpVector;

	/**
	 * Height relative to input plate
	 */
	UPROPERTY(meta = (Output))
		float Height = 0.f;

	/**
	 * Reprojected location
	 */
	UPROPERTY(meta = (Output))
		FVector Projection = FVector::ZeroVector;
};

/**
 * Warp point along direction
 */
USTRUCT(meta = (DisplayName = "Warp Along direction", Category = "TGOR Utility", Keywords = "TGOR,Utility", PrototypeName = "WarpAlongDirection", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_WarpAlongDirection : public FRigUnit
{
	GENERATED_BODY()

		FRigUnit_WarpAlongDirection() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:

	/**
	 * Point to reproject
	 */
	UPROPERTY(meta = (Input))
		FVector Point = FVector::ZeroVector;

	/**
	 * Trajectory reference point, scales along this point
	 */
	UPROPERTY(meta = (Input))
		FVector Reference = FVector::ZeroVector;

	/**
	 * Projection direction
	 */
	UPROPERTY(meta = (Input))
		FVector Direction = FVector::UpVector;

	/**
	 * Warp scale
	 */
	UPROPERTY(meta = (Input))
		float Scale = 1.0f;

	/**
	 * Reprojected location
	 */
	UPROPERTY(meta = (Output))
		FVector Warped = FVector::ZeroVector;
};

/**
 * Scale value from scale vector
 */
USTRUCT(meta = (DisplayName = "Scale To Value", Category = "TGOR Utility", Keywords = "TGOR,Utility", PrototypeName = "ScaleToValue", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_ScaleToValue : public FRigUnit
{
	GENERATED_BODY()

		FRigUnit_ScaleToValue() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:

	/**
	 * Key
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey Key = FRigElementKey(FName(), ERigElementType::Control);

	/**
	 * Scale axis
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		TEnumAsByte<EAxis::Type> Axis = EAxis::None;

	/**
	 * Output length
	 */
	UPROPERTY(meta = (Output))
		float Output = 0.0f;

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement Cache;
};

