

#pragma once

#include "RigUnit_IK.h"
#include "ControlRig/Utility.h"

#include "RigUnit_DigitigradeIK.generated.h"

/**
 * Digitigrade leg IK solver. Properly rotates ankle and knee bone.
 */
USTRUCT(meta = (DisplayName = "Digitigrade IK", Category = "IK", Keywords = "Angry,IK", PrototypeName = "DigitigradeIK", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_DigitigradeIK : public FRigUnit_IK
{
	GENERATED_BODY()

		FRigUnit_DigitigradeIK() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

public:

	/**
	 * Minimum knee angle in degrees
	 */
	UPROPERTY(meta = (Input))
		float MinKneeAngle = 25.0f;

	/**
	 * Minimum ankle angle in degrees
	 */
	UPROPERTY(meta = (Input))
		float MinAnkleAngle = 25.0f;

	/**
	 * Ankle bend when stretched
	 */
	UPROPERTY(meta = (Input))
		float StandingBend = 3.0f;

	/**
	 * Percentage of how much the ankle direction is determined by the hip direction
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		float AnkleKneeDirectionWeight = 1.0f;

	/**
	 * Percentage of how much the knee direction is determined by the hip direction
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		float KneeHipDirectionWeight = 1.0f;

	/**
	 * Length customisation
	 */
	UPROPERTY(meta = (Input))
		FVector Customisation = FVector(1.0f, 1.0f, 1.0f);

	/**
	* How to scale the bones
	 */
	UPROPERTY(meta = (Input))
		EBendScaleType ScaleType = EBendScaleType::None;

	/**
	 * Stretch ratio, 1.0 for initial, 0.0 for all the way compressed
	 */
	UPROPERTY(meta = (Output))
		float Stretch = 1.0f;
};
