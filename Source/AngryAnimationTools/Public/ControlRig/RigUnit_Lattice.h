

#pragma once

#include "Units/RigUnit.h"
#include "ControlRig/Utility.h"

#include "RigUnit_Lattice.generated.h"

USTRUCT(BlueprintType)
struct FRigUnit_LatticePoint
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Lattice")
		FRigElementKey Key = FRigElementKey(FName(), ERigElementType::Control);

	UPROPERTY(EditAnywhere, Category = "Lattice")
		FVector Distribution = FVector::OneVector;
};

USTRUCT()
struct FRigUnit_LatticeTransform_WorkData
{
	GENERATED_BODY()

	TArray<TArray<float>> Weights;

	UPROPERTY()
		TArray<FCachedRigElement> CachedParents;
};

/**
 * Transform a chain of bones according to a target. The closer the bones are to the target transform the more it will get transformed by the target.
 */
USTRUCT(meta = (DisplayName = "Lattice Transform", Category = "Utility", Keywords = "Angry,Utility", PrototypeName = "LatticeTransform", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_LatticeTransform : public FRigUnitMutable
{
	GENERATED_BODY()

		FRigUnit_LatticeTransform() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:

	/**
	 * Chain of bones to be lattice transformed
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection Chain;

	/**
	 * Controls making up the lattice
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		TArray<FRigUnit_LatticePoint> Lattice;

	/**
	 */
	UPROPERTY(meta = (Input, Constant))
		bool PropagateToChildren = true;

	/**
	* How to scale the bones
	 */
	UPROPERTY(meta = (Input))
		EBendScaleType ScaleType = EBendScaleType::None;

	/**
	 * Debug settings
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FDebugSettings DebugSettings;

	// Cache
	UPROPERTY(transient)
		FRigUnit_LatticeTransform_WorkData WorkData;
};

