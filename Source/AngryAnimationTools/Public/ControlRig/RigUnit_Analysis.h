

#pragma once

#include "Units/RigUnit.h"
#include "ControlRig/Utility.h"

#include "RigUnit_Analysis.generated.h"

/**
 * 
 */
USTRUCT(meta = (DisplayName = "Mean Direction", Category = "Analysis", Keywords = "Angry,Utility", PrototypeName = "MeanDirection", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_MeanDirection : public FRigUnit
{
	GENERATED_BODY()

		FRigUnit_MeanDirection() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

public:

	/**
	 * The chain to compute the direction for
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection Chain;

	/**
	 * Direction is computed relative to this key
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey ReferenceKey;

	/**
	 * Computed direction
	 */
	UPROPERTY(meta = (Output))
		FVector Output = FVector::ForwardVector;

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement ReferenceCache;
};

/**
 * Computes the main direction of a chain of bones using the power method (first row of SVD)
 */
USTRUCT(meta = (DisplayName = "Power Direction", Category = "Analysis", Keywords = "Angry,Utility", PrototypeName = "PowerDirection", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_PowerDirection : public FRigUnit
{
	GENERATED_BODY()

		FRigUnit_PowerDirection() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

public:

	/**
	 * The chain to compute the direction for
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection Chain;

	/**
	 * Number of power iterations
	 */
	UPROPERTY(meta = (Input))
		int32 Iterations = 8;

	/**
	 * Computed direction
	 */
	UPROPERTY(meta = (Output))
		FVector Output = FVector::ForwardVector;
};


/**
 * Analyse properties of a chain
 */
USTRUCT(meta = (DisplayName = "Chain Analysis", Category = "Analysis", Keywords = "TGOR,Utility", PrototypeName = "ChainAnalysis", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_ChainAnalysis : public FRigUnit
{
	GENERATED_BODY()

		FRigUnit_ChainAnalysis() {}

	RIGVM_METHOD()
		virtual void Execute(const FRigUnitContext& Context) override;

public:
	static float ComputeInitialChainLength(const FRigElementKeyCollection& Chain, const URigHierarchy* Hierarchy);
	static void Analysis(const FRigElementKeyCollection& Chain, const URigHierarchy* Hierarchy, float Multiplier, float& ChainMaxLength, float& CurrentLength, float& InitialLength);


	/**
	 * The chain to apply the power method to
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection Chain;

	/**
	 * Chain Length Multiplier (Multiplier of 1.0 assumes that maximum length of the Source Chain is when it's perfectly straight, but most organic things can not straighten all their bones like that, thus it might be needed to have this value lower than 1.0)
	 */
	UPROPERTY(meta = (Input))
		float LengthMultiplier = 1.0f;

	/**
	 * Chain's Max Length in cm, determined from measuring length of each bone in the chain and summing it up, then multiplying it by the length multiplier.
	 */
	UPROPERTY(meta = (Output))
		float MaxLength = 0.0f;

	/**
	 * What is the current length of a straight line from the start of the chain to the end, in cm.
	 */
	UPROPERTY(meta = (Output))
		float CurrentLength = 0.0f;

	/**
	 * What is the initial length of a straight line from the start of the chain to the end, in cm.
	 */
	UPROPERTY(meta = (Output))
		float InitialLength = 0.0f;

	/**
	 * The ratio between max and current chain length.
	 */
	UPROPERTY(meta = (Output))
		float LengthRatio = 0.0f;
};