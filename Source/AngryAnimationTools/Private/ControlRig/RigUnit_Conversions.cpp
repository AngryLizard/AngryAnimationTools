

#include "ControlRig/RigUnit_Conversions.h"

#include "ControlRig.h"
#include "Units/RigUnitContext.h"

FRigUnit_SetTransformWithOffset_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT();
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (!Hierarchy)
	{
		return;
	}

	if (!Cache.UpdateCache(Key, Hierarchy))
	{
		UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Key '%s' is not valid."), *Key.ToString());
	}
	else
	{
		const FTransform Output = FTransform(OffsetRotation, OffsetTranslation) * Transform;
		Hierarchy->SetGlobalTransform(Cache, Output, bPropagateToChildren);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FRigUnit_CloneTransforms_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT();
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (!Hierarchy)
	{
		return;
	}

	const int32 Num = Items.Num();
	for (int32 Index = 0; Index < Num; Index++)
	{
		const FTransform Transform = Hierarchy->GetGlobalTransform(Items[Index]);
		const FRigElementKey NewKey = FRigElementKey(Items[Index].Name, TargetType);
		if (Hierarchy->GetIndex(NewKey) != INDEX_NONE)
		{
			Hierarchy->SetGlobalTransform(NewKey, Transform, false, false);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FRigUnit_Rebase_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT();
	
	Output = (Transform * FromSpace.Inverse());
	Output.SetLocation(Output.GetLocation() * TranslationScale);
	Output *= ToSpace;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FRigUnit_AffineRebase_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT();

	Output.SetLocation((Transform.GetLocation() - FromSpace.GetLocation()) * TranslationScale + ToSpace.GetLocation());
	Output.SetRotation((Transform.GetRotation() * FromSpace.GetRotation().Inverse()) * ToSpace.GetRotation());
	Output.NormalizeRotation();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FQuat FRigUnit_AxisAlignRotation::ComputeHeadingRotation(const FVector& SourceForward, const FVector& TargetForward, const FVector& SourceUp, const FVector& TargetUp)
{
	FQuat Rotation = FQuat::FindBetweenNormals(SourceForward, TargetForward);
	const FVector UpProject = FVector::VectorPlaneProject(TargetUp, TargetForward).GetSafeNormal();

	// Flip if on the other hemisphere for more accurate results
	FVector UpTarget = Rotation * SourceUp;
	if ((UpTarget | UpProject) < 0.0f)
	{
		// This corresponds to a 180° rotation around forward axis
		Rotation = FQuat(TargetForward.X, TargetForward.Y, TargetForward.Z, 0.0f) * Rotation;
		Rotation = FQuat::FindBetweenNormals(-UpTarget, UpProject) * Rotation;
	}
	else
	{
		Rotation = FQuat::FindBetweenNormals(UpTarget, UpProject) * Rotation;
	}

	return Rotation.GetNormalized();
}

FRigUnit_AxisAlignRotation_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT();

	if (SourceForward.IsNearlyZero())
	{
		UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Source forward is null."));
	}
	else if (TargetForward.IsNearlyZero())
	{
		UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Target forward is null."));
	}
	else if (SourceUp.IsNearlyZero())
	{
		UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Source up is null."));
	}
	else if (TargetUp.IsNearlyZero())
	{
		UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Target up is null."));
	}
	else
	{
		Output = ComputeHeadingRotation(SourceForward, TargetForward.GetSafeNormal(), SourceUp, TargetUp.GetSafeNormal());
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FRigUnit_RotationBetween_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT();
	const URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (!Hierarchy)
	{
		return;
	}

	Output = FQuat::FindBetweenVectors(Source, Target);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FRigUnit_ProjectOntoPlane_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT();
	const URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (!Hierarchy)
	{
		return;
	}

	const FVector Delta = Point - Reference;
	double Project = Delta | Direction;

	const FVector Projected = Direction * Project;

	const FVector Warp = Reference + Delta - Projected;
	const FVector Intersection = FVector::PointPlaneProject(Warp, FPlane(Location, Normal));

	Height = Project;
	Projection = Intersection + Projected;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FRigUnit_WarpAlongDirection_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT();
	const URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (!Hierarchy)
	{
		return;
	}

	const FVector Delta = Point - Reference;
	const FVector Projected = Delta.ProjectOnToNormal(Direction);
	const FVector Offset = Delta - Projected;

	Warped = Reference + Offset + Projected * Scale;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FRigUnit_ScaleToValue_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT();
	const URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (!Hierarchy)
	{
		return;
	}

	if (!Cache.UpdateCache(Key, Hierarchy))
	{
		UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Cone '%s' is not valid."), *Key.ToString());
	}
	else
	{
		const FTransform Transform = Hierarchy->GetGlobalTransform(Cache);
		if (Axis == EAxis::None)
		{
			Output = Transform.GetScale3D().Size() / SQRT_THREE * 100.0f;
		}
		else
		{
			Output = Transform.GetScale3D().GetComponentForAxis(Axis) * 100.0f;
		}
	}
}