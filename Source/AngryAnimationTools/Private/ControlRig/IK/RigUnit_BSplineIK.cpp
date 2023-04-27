

#include "ControlRig/IK/RigUnit_BSplineIK.h"
#include "ControlRig/RigUnit_BendTowards.h"
#include "ControlRig/RigUnit_Conversions.h"

#include "ControlRig.h"
#include "Units/RigUnitContext.h"

FRigUnit_BSplineIK_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT();
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		if (!Hierarchy)
		{
			return;
		}

		const int32 ChainCount = Chain.Num();
		if (ChainCount < 2)
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Chain has to have length at least 2."));
		}
		else
		{
			// Extract length information from initial chain
			TArray<float> Lengths;
			Lengths.SetNumZeroed(ChainCount - 1);
			float TotalDistance = 0.0f;

			FVector Current = Hierarchy->GetInitialGlobalTransform(Chain[0]).GetLocation();
			for (int32 Index = 1; Index < ChainCount; Index++)
			{
				const FVector Next = Hierarchy->GetInitialGlobalTransform(Chain[Index]).GetLocation();
				const float Length = (Current - Next).Size();
				Lengths[Index - 1] = Length;
				TotalDistance += Length;
				Current = Next;
			}

			if (FMath::IsNearlyZero(TotalDistance))
			{
				UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Total chain length is null."));
			}
			else
			{
				const FTransform EndEETarget = GET_IK_OBJECTIVE_TRANSFORM();
				const float InvTotalDistance = PositionAlongSpline / TotalDistance;

				// Define B-Spline
				const FTransform Origin = Hierarchy->GetGlobalTransform(Chain.First());
				const float TargetDistance = (Origin.GetLocation() - EndEETarget.GetLocation()).Size();

				const FVector StartAnchor = Origin.GetLocation() + TangentStart * (TargetDistance * Bend);
				const FVector EndAnchor = EndEETarget.GetLocation() + TangentEnd * (TargetDistance * Bend);
				auto LerpSpline = [&](float Alpha) -> FVector {
					return FMath::Lerp(FMath::Lerp(Origin.GetLocation(), StartAnchor, Alpha), FMath::Lerp(EndAnchor, EndEETarget.GetLocation(), Alpha), FMath::SmoothStep(0.f, 1.f, Alpha));
				};

				// Build spline
				float Distance = 0.0f;
				for (int32 Index = 1; Index < ChainCount; Index++)
				{
					// Compute distance and next target along spline
					const float NextDistance = Distance + Lengths[Index - 1] * InvTotalDistance;
					const FVector NextLocation = LerpSpline(NextDistance);
					Distance = NextDistance;

					if (DebugSettings.bEnabled)
					{
						Context.DrawInterface->DrawPoint(FTransform::Identity, NextLocation, DebugSettings.Scale * 2.0f, FLinearColor::Red);
					}

					// Compute transform and prepare next iteration
					FRigUnit_BendTowards::BendTowards(Chain[Index - 1], Chain[Index], NextLocation, Hierarchy, ScaleType, PropagateToChildren == EPropagation::All);
				}

				// Set last member of chain
				FTransform EE = Hierarchy->GetGlobalTransform(Chain.Last());
				EE.SetRotation(FQuat::Slerp(EndEETarget.GetRotation(), EE.GetRotation(), RotateWithTangent));
				EE.SetScale3D(Objective.GetScale3D());
				EE.SetLocation(EE.GetLocation());
				Hierarchy->SetGlobalTransform(Chain.Last(), EE, false, PropagateToChildren != EPropagation::Off);
			}
		}
	}
}
