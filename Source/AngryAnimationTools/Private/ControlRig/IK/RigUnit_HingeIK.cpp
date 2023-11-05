

#include "ControlRig/IK/RigUnit_HingeIK.h"
#include "ControlRig/RigUnit_BendTowards.h"

#include "ControlRig.h"
#include "Units/RigUnitContext.h"

bool FRigUnit_HingeIK::ComputeTriangle(float a, float b, float s, float& h, float& x)
{
	if (a + b < s)
	{
		x = s * (a / (a + b));
		h = 0.0f;
		return false;
	}

	const float aa = a * a;
	const float bb = b * b;
	const float ss = s * s;
	const float hh = (2.0f * (aa * bb + aa * ss + bb * ss) - (aa * aa + bb * bb + ss * ss)) / (4.0f * ss);

	if (hh < 0.0f)
	{
		x = s * (a / (a + b));
		h = 0.0f;
		return false;
	}

	x = FMath::Sqrt(aa - hh);
	h = FMath::Sqrt(hh);
	return true;
}

FRigUnit_HingeIK_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT();
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;
	FRigVMDrawInterface* DrawInterface = ExecuteContext.GetDrawInterface();
	if (!Hierarchy)
	{
		return;
	}

	if (Chain.Num() < 3)
	{
		UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Chain has to have length at least 3."));
	}
	else
	{
		// Objective properties
		const FTransform EndEETarget = GET_IK_OBJECTIVE_TRANSFORM();

		// Compute initial leg properties
		const FTransform InitialUpper = Hierarchy->GetInitialGlobalTransform(Chain[0]);
		const FTransform InitialLower = Hierarchy->GetInitialGlobalTransform(Chain[1]);
		const FTransform InitialEE = Hierarchy->GetInitialGlobalTransform(Chain[2]);

		FVector2D InitialLengths;
		InitialLengths.X = (InitialUpper.GetLocation() - InitialLower.GetLocation()).Size() * Customisation.X;
		InitialLengths.Y = (InitialLower.GetLocation() - InitialEE.GetLocation()).Size() * Customisation.Y;

		// Rescale to fit length
		float ChainMaxLength = InitialLengths.X + InitialLengths.Y;
		if (!FMath::IsNearlyZero(ChainMaxLength))
		{
			const float TargetRatio = TargetLength;

			ChainMaxLength *= TargetRatio;
			InitialLengths *= TargetRatio;
		}

		// Compute objective deltas
		FTransform Upper = Hierarchy->GetGlobalTransform(Chain[0]);
		FTransform Lower = Hierarchy->GetGlobalTransform(Chain[1]);
		FTransform EE = Hierarchy->GetGlobalTransform(Chain[2]);
		const FVector ObjectiveDelta = Objective.GetLocation() - Upper.GetLocation();
		const float ObjectiveNorm = ObjectiveDelta.Size();
		const FVector ObjectiveNormal = ObjectiveDelta / ObjectiveNorm;

		const FVector HingeDirection = FVector::VectorPlaneProject(Direction, ObjectiveNormal).GetSafeNormal();

		// Compute pivot location
		float Kath = 0.0f;
		float Height = 0.0f;
		ComputeTriangle(InitialLengths.X, InitialLengths.Y, ObjectiveNorm, Height, Kath);

		const FVector HingeLocation = Upper.GetLocation() + ObjectiveNormal * Kath + HingeDirection * Height;

		if (DebugSettings.bEnabled)
		{
			const FVector Location = Upper.GetLocation() + ObjectiveNormal * Kath;
			DrawInterface->DrawLine(FTransform::Identity, Location, HingeLocation, FLinearColor::Yellow, DebugSettings.Scale);
			DrawInterface->DrawLine(FTransform::Identity, Objective.GetLocation(), Upper.GetLocation(), FLinearColor::Yellow, DebugSettings.Scale * 0.5f);
		}

		// Transform to propagate along the hinge
		FRigUnit_BendTowards::BendTowards(Chain[0], Chain[1], HingeLocation, Hierarchy, ScaleType, PropagateToChildren == EPropagation::All);
		FRigUnit_BendTowards::BendTowards(Chain[1], Chain[2], Objective.GetLocation(), Hierarchy, ScaleType, PropagateToChildren == EPropagation::All);

		// Set foot transform
		EE.SetRotation(EndEETarget.GetRotation());
		EE.SetScale3D(InitialEE.GetScale3D());
		EE.SetLocation(Objective.GetLocation());
		Hierarchy->SetGlobalTransform(Chain[2], EE, false, PropagateToChildren != EPropagation::Off);
	}
}