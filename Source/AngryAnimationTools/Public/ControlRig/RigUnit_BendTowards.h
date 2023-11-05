

#pragma once

#include "Units/RigUnit.h"
#include "ControlRig/Utility.h"

#include "RigUnit_BendTowards.generated.h"

/**
 * 
 */
USTRUCT(meta = (DisplayName = "Bend Towards", Category = "Utility", Keywords = "Angry,Utility", PrototypeName = "BendTowards", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_BendTowards : public FRigUnitMutable
{
	GENERATED_BODY()

		FRigUnit_BendTowards() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:
	static FTransform BendTowards(const FRigElementKey& Key, const FRigElementKey& NextKey, const FVector& Target, URigHierarchy* Hierarchy, EBendScaleType type, bool bPropagateToChildren, float Intensity = 1.0f);

	/**
	* Starting key
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey Key;

	/**
	* Next key in line that we want to move towards target
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey NextKey;

	/**
	* Blending ratio
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		float Alpha = 1.0f;

	/**
	* How to scale the bones
	 */
	UPROPERTY(meta = (Input))
		EBendScaleType ScaleType = EBendScaleType::Default;

	/**
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FVector TargetLocation = FVector::ZeroVector;

	/**
	 */
	UPROPERTY(meta = (Input, Constant))
		bool PropagateToChildren = true;
};

