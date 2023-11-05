

#pragma once

#include "Units/RigUnit.h"
#include "ControlRig/Utility.h"

#include "RigUnit_PreviewAnimation.generated.h"

USTRUCT(BlueprintType)
struct FRigUnit_PreviewSettings
{
	GENERATED_BODY()

	FRigUnit_PreviewSettings()
	{
		PreviewAnimation = nullptr;
	}

	UPROPERTY(EditAnywhere, Category = "PreviewSettings")
		UAnimSequence* PreviewAnimation = nullptr;

	UPROPERTY(EditAnywhere, Category = "PreviewSettings")
		TSubclassOf<UControlRig> ConversionRigClass;
};

USTRUCT(BlueprintType)
struct FRigUnit_PreviewAnimation_WorkData
{
	GENERATED_BODY()

	FRigUnit_PreviewAnimation_WorkData()
	{
	}

	UPROPERTY()
		double Time = 0.0f;

	UPROPERTY()
		bool bInitialized = false;

	UPROPERTY(transient)
		TWeakObjectPtr<UControlRig> ConversionRig;

	struct FBoneContainer BoneContainer;
	struct FCompactPose Pose;
	struct FBlendedCurve Curve;
	struct UE::Anim::FStackAttributeContainer Attributes;
};

/**
 * Maps an animation asset to available controls
 * NOTE: This node is super slow, only use for testing retargeting rigs
 */
USTRUCT(meta = (DisplayName = "Preview animation", Category = "Utility", Keywords = "Angry,Preview", PrototypeName = "PreviewAnimation", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_PreviewAnimation : public FRigUnitMutable
{
	GENERATED_BODY()

		FRigUnit_PreviewAnimation() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:

	/**
	 * Animation to preview
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FRigUnit_PreviewSettings PreviewSettings;

	// Cache
	UPROPERTY(transient)
		FRigUnit_PreviewAnimation_WorkData WorkData;
};

