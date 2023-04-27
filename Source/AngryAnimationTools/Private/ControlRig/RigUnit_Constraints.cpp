

#include "ControlRig/RigUnit_Constraints.h"

#include "ControlRig.h"
#include "Units/RigUnitContext.h"

float FRigUnit_SoftLimitValue::SoftLimit(float Value, float Limit)
{
	return Limit * 2.0f * (1.0f / (1.0f + FMath::Exp(-Value * 2.0f / Limit)) - 0.5f);
}

FRigUnit_SoftLimitValue_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT();
	const URigHierarchy* Hierarchy = Context.Hierarchy;

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

		Output = SoftLimit(Value, Limit);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float ComputeWLimit(float W, float Limit, bool Soft)
{
	const float Ls = FMath::Cos(Limit * 0.5f);
	if (Soft)
	{
		const float WSign = FMath::Sign(W);
		return 1.0f - FRigUnit_SoftLimitValue::SoftLimit(1.0f - WSign * W, 1.0f - Ls);
	}
	return FMath::Max(Ls, W);
}

FQuat AssembleQuat(const FVector& A, float W, float Sq)
{
	const float V = 1.0f - W * W;
	if (!FMath::IsNearlyZero(V))
	{
		const float InvS = FMath::Sqrt(V / Sq);
		return FQuat(A.X * InvS, A.Y * InvS, A.Z * InvS, W);
	}
	return FQuat::Identity;
}

FQuat FRigUnit_LimitRotation::LimitRotation(const FQuat& Quat, float Limit, bool Soft)
{
	const FVector A = FVector(Quat.X, Quat.Y, Quat.Z);
	const float Sq = A.SizeSquared();

	const float W = ComputeWLimit(Quat.W, Limit, Soft);
	return AssembleQuat(A, W, Sq);
}

FRigUnit_LimitRotation_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT();
	const URigHierarchy* Hierarchy = Context.Hierarchy;

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

		Output = LimitRotation(Quat, Limit, Soft);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FQuat FRigUnit_LimitRotationAroundAxis::AxisLimitRotation(const FQuat& Quat, const FVector& Axis, float Min, float Max, bool Soft)
{
	// Compute acceptable W
	const FVector A = FVector(Quat.X, Quat.Y, Quat.Z);
	const float Sq = A.SizeSquared();

	const float Lr = (Axis | (A / FMath::Sqrt(Sq)));
	const float L = FMath::Lerp(Min, Max, (1.0f + Lr) * 0.5f);

	const float W = ComputeWLimit(Quat.W, L, Soft);
	return AssembleQuat(A, W, Sq);
}

FRigUnit_LimitRotationAroundAxis_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT();
	const URigHierarchy* Hierarchy = Context.Hierarchy;

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

		Output = AxisLimitRotation(Quat, Axis, MinLimit, MaxLimit, Soft);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FRigUnit_BellCurve_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
		const URigHierarchy* Hierarchy = Context.Hierarchy;

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

		Output = FMath::Exp(Value * Value / -Variance);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FRigUnit_DistanceBellCurve_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
		const URigHierarchy* Hierarchy = Context.Hierarchy;

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

		const FVector Delta = Reference - Location;
		Output = FMath::Exp(Delta.SizeSquared() / -Variance);
	}
}