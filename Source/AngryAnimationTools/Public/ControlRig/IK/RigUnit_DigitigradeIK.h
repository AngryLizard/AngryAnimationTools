// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Units/RigUnit.h"
#include "ControlRig/Utility.h"

#include "RigUnit_DigitigradeIK.generated.h"

/**
 *
 */
USTRUCT(meta = (DisplayName = "Digitigrade IK", Category = "IK", Keywords = "Angry,IK", PrototypeName = "DigitigradeIK", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_DigitigradeIK : public FRigUnitMutable
{
	GENERATED_BODY()

		FRigUnit_DigitigradeIK() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

public:

	/**
	 * The chain to adapt (Has to be continuous chain)
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection Chain;

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
		EBendScaleType ScaleType = EBendScaleType::Default;

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
