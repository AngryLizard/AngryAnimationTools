

#pragma once

#include "ControlRig/RigUnit_BendTowards.h"

FTransform FRigUnit_BendTowards::BendTowards(const FRigElementKey& Current, const FRigElementKey& Next, const FVector& Target, URigHierarchy* Hierarchy, EBendScaleType type, bool bPropagateToChildren, float Intensity)
{
	FTransform Transform = Hierarchy->GetGlobalTransform(Current);
	const FTransform Local = Hierarchy->GetLocalTransform(Next);

	// Rotate to match
	const FVector CurrentLocation = Transform.GetLocation();
	const FVector CurrentDelta = Transform.TransformVectorNoScale(Local.GetLocation());
	const FVector TargetDelta = FMath::Lerp(CurrentDelta, Target - CurrentLocation, Intensity);
	const FQuat Rotation = FQuat::FindBetweenVectors(CurrentDelta, TargetDelta);
	Transform.SetRotation(Rotation * Transform.GetRotation());

	switch (type)
	{
		case EBendScaleType::Default:
		{
			//FTransform NextTransform = Hierarchy->GetGlobalTransform(Next);

			// Rotate current element (We need to propagate here in case other children are attached to current)
			Hierarchy->SetGlobalTransform(Current, Transform, false, bPropagateToChildren);

			// Move next element towards desired target without changing scale 
			// (TODO: if bPropagateToChildren true we do some of these computations double)
			FTransform NextTransform = Local * Transform;
			NextTransform.SetLocation(CurrentLocation + TargetDelta);
			Hierarchy->SetGlobalTransform(Next, NextTransform, false, bPropagateToChildren);
			break;
		}
		case EBendScaleType::None:
		{
			// Rotate current element
			Hierarchy->SetGlobalTransform(Current, Transform, false, bPropagateToChildren);

			// Result is identical if propagate is on
			if (!bPropagateToChildren)
			{
				Hierarchy->SetGlobalTransform(Next, Local * Transform, false, false);
			}
			break;
		}
		case EBendScaleType::Stretch:
		{
			// Scale bone to match
			const float LocationSize = Transform.TransformVector(Local.GetLocation()).Size();
			if (!FMath::IsNearlyZero(LocationSize))
			{
				const float AxisScale = TargetDelta.Size() / LocationSize - 1.0f;
				Transform.SetScale3D(Transform.GetScale3D() * (FVector::OneVector + AxisScale * Local.GetLocation().GetSafeNormal().GetAbs()));
			}

			// Update chain element
			Hierarchy->SetGlobalTransform(Current, Transform, false, bPropagateToChildren);

			// Propagate to only next in line if propagation is turned off
			if (!bPropagateToChildren)
			{
				Hierarchy->SetGlobalTransform(Next, Local * Transform, false, false);
			}
			break;
		}
	}
	return Transform;
}

FRigUnit_BendTowards_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT();
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;
	if (!Hierarchy)
	{
		return;
	}

	if (!Hierarchy)
	{
		return;
	}

	BendTowards(Key, NextKey, TargetLocation, Hierarchy, ScaleType, PropagateToChildren, Alpha);
}