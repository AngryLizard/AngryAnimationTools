

#include "ControlRig/IK/RigUnit_NeckIK.h"
#include "ControlRig/RigUnit_Constraints.h"

#include "ControlRig.h"
#include "Units/RigUnitContext.h"

FRigUnit_NeckIK_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT();
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		const int32 ChainNum = Chain.Num();
		if (ChainNum < 2)
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Chain has to have length at least 2."));
		}
		else
		{
			// Objective properties
			const FTransform EndEETarget = GET_IK_OBJECTIVE_TRANSFORM();
			const FVector EEUpTarget = GET_IK_OBJECTIVE_UP();

			const FTransform Last = Hierarchy->GetGlobalTransform(Chain.Last());
			const FVector TargetDirection = (EndEETarget.GetLocation() - Last.GetLocation()).GetSafeNormal();
			const FVector CurrentForward = Last.TransformVectorNoScale(EndeffectorForward);
			const FVector CurrentUp = Last.TransformVectorNoScale(EndeffectorUp);

			// Compute bending applied to each segment
			const FVector BendAxis = FVector::CrossProduct(CurrentForward, TargetDirection).GetSafeNormal();
			const float BendAngle = FRigUnit_SoftLimitValue::SoftLimit(FMath::Acos(CurrentForward | TargetDirection), FMath::DegreesToRadians(BendAngleLimit));
			const FQuat BendSegment = FQuat(BendAxis, BendAngle * Intensity / (ChainNum - 1));
			const FQuat FullBend = FQuat(BendAxis, BendAngle * Intensity);

			// Compute twist applied to each segment
			const FVector TwistUpAxis = FullBend * CurrentUp;
			const FVector TwistAxis = FVector::CrossProduct(TwistUpAxis, EEUpTarget).GetSafeNormal().ProjectOnToNormal(TargetDirection);
			const float TwistAngle = FRigUnit_SoftLimitValue::SoftLimit(FMath::Acos(TwistUpAxis | EEUpTarget), FMath::DegreesToRadians(TwistAngleLimit));
			const FQuat TwistSegment = FQuat(TwistAxis, TwistAngle * Intensity / (ChainNum - 1));

			if (DebugSettings.bEnabled)
			{
				const FTransform First = Hierarchy->GetGlobalTransform(Chain.First());
				Context.DrawInterface->DrawLine(FTransform::Identity, First.GetLocation(), First.GetLocation() + TwistUpAxis * 20.0f, FLinearColor::Red, DebugSettings.Scale * 0.2f);
				Context.DrawInterface->DrawLine(FTransform::Identity, First.GetLocation(), First.GetLocation() + EEUpTarget * 20.0f, FLinearColor::Green, DebugSettings.Scale * 0.2f);
				Context.DrawInterface->DrawLine(FTransform::Identity, First.GetLocation(), First.GetLocation() + TwistAxis * 20.0f, FLinearColor::Blue, DebugSettings.Scale * 0.5f);
			}

			// Apply rotation along the whole chain
			FTransform Transform = Hierarchy->GetGlobalTransform(Chain[0]);
			for (int32 Index = 1; Index < ChainNum; Index++)
			{
				const FTransform Local = Hierarchy->GetLocalTransform(Chain[Index]);

				// Rotate segment along forward axis according to how much they are aligned to roll axis
				Transform.SetRotation(TwistSegment * BendSegment * Transform.GetRotation());

				// Update chain element (no update needed since we propagate everything later)
				Hierarchy->SetGlobalTransform(Chain[Index - 1], Transform, false, false);

				// Propagate to next in line and store delta for fabrik
				Transform = Local * Transform;
			}

			// Set EE transform
			Hierarchy->SetGlobalTransform(Chain.Last(), Transform, false, PropagateToChildren != EPropagation::Off);

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, Transform.GetLocation(), Transform.GetLocation() + EEUpTarget * 20.0f, FLinearColor::Blue, DebugSettings.Scale * 0.5f);
			}
		}
	}
}
