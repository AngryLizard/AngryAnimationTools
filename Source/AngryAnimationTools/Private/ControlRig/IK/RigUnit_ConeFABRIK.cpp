

#include "ControlRig/IK/RigUnit_ConeFABRIK.h"
#include "ControlRig/RigUnit_Constraints.h"

#include "ControlRig.h"
#include "Units/RigUnitContext.h"

FQuat FRigUnit_ConeFABRIK::SoftRotate(const FTransform& Local, const FTransform& Transform, const FTransform& Anchor, float MaxAngle)
{
	const FQuat Rotation = FQuat::FindBetweenVectors(Local.GetLocation(), Transform.InverseTransformPosition(Anchor.GetLocation()));
	const float Angle = FRigUnit_SoftLimitValue::SoftLimit(Rotation.GetAngle(), MaxAngle);
	return (FQuat(Rotation.GetRotationAxis(), Angle));
}

FTransform Fabrik(TArray<FTransform>& Transforms, const TArray<FTransform>& Rest, int32 Index, const FTransform& Forward, const FTransform& Backward, float MaxAngle)
{
	if (Index == Rest.Num() - 1)
	{
		return Backward;
	}

	const FTransform Regular = Rest[Index + 1];

	Transforms[Index] = Forward;
	const FQuat ForwardRotation = FRigUnit_ConeFABRIK::SoftRotate(Regular, Forward, Transforms[Index + 1], MaxAngle);

	const FTransform Transform = Regular * ForwardRotation * Forward;
	const FTransform Objective = Fabrik(Transforms, Rest, Index + 1, Transform, Backward, MaxAngle);

	Transforms[Index + 1] = Objective;
	const FQuat BackwardRotation = FRigUnit_ConeFABRIK::SoftRotate(Regular.Inverse(), Objective, Transforms[Index], MaxAngle);

	return Regular.Inverse() * BackwardRotation * Objective;
}

FRigUnit_ConeFABRIK_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT();
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
	}

	if (Context.State == EControlRigState::Update)
	{
		if (!Hierarchy)
		{
			return;
		}

		const int32 ChainNum = Chain.Num();
		if (ChainNum < 2)
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Chain has to have length at least 2."));
		}
		else
		{
			const FTransform StartEE = Context.Hierarchy->GetGlobalTransform(Chain.First());
			const FTransform EndEETarget = GET_IK_OBJECTIVE_TRANSFORM();

			// Populate transform lists
			TArray<FTransform> Transforms, Rest;
			Transforms.Reserve(ChainNum);
			Rest.Reserve(ChainNum);
			for (int32 Index = 0; Index < ChainNum; Index++)
			{
				Transforms.Emplace(Hierarchy->GetGlobalTransform(Chain[Index]));
				Rest.Emplace(Hierarchy->GetInitialGlobalTransform(Chain[Index]));
			}

			// Objective properties
			const float MaxRadians = FMath::DegreesToRadians(MaxAngle);
			for (int32 Iteration = 0; Iteration < Iterations; Iteration++)
			{
				Transforms[0] = Fabrik(Transforms, Rest, 0, StartEE, EndEETarget, MaxRadians);
			}

			// Set bones to transforms
			Hierarchy->SetGlobalTransform(Chain.First(), Transforms[0], false, PropagateToChildren != EPropagation::Off);
			for (int32 Index = 1; Index < ChainNum - 1; Index++)
			{
				Hierarchy->SetGlobalTransform(Chain[Index], Transforms[Index], false, PropagateToChildren == EPropagation::All);
			}
			Hierarchy->SetGlobalTransform(Chain.Last(), Transforms.Last(), false, PropagateToChildren != EPropagation::Off);
		}
	}
}
