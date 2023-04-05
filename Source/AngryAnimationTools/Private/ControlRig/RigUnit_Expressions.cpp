

#include "ControlRig/RigUnit_Expressions.h"
#include "ControlRig/RigUnit_Constraints.h"

#include "ControlRig.h"
#include "Units/RigUnitContext.h"


FRigUnit_EyeLookAt_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT();
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		Cache.Reset();
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		if (!Cache.UpdateCache(Key, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Key '%s' is not valid."), *Key.ToString());
		}
		else
		{
			// Objective properties

			// Get current transform basis
			FTransform Transform = FTransform(AxisOffset) * Hierarchy->GetGlobalTransform(Cache);
			const FVector CurrentForward = Transform.GetUnitAxis(EAxis::Y);
			const FVector CurrentRight = -Transform.GetUnitAxis(EAxis::X);
			const FVector CurrentUp = Transform.GetUnitAxis(EAxis::Z);

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, Transform.GetLocation(), Transform.GetLocation() + CurrentForward * 10.0f, FLinearColor::Red, DebugSettings.Scale * 0.1f);
				Context.DrawInterface->DrawLine(FTransform::Identity, Transform.GetLocation(), Transform.GetLocation() + CurrentRight * 10.0f, FLinearColor::Green, DebugSettings.Scale * 0.1f);
				Context.DrawInterface->DrawLine(FTransform::Identity, Transform.GetLocation(), Transform.GetLocation() + CurrentUp * 10.0f, FLinearColor::Blue, DebugSettings.Scale * 0.1f);
			}

			// Compute center with input offsets
			const FQuat CenterRotation = FQuat(CurrentUp, FMath::DegreesToRadians(Bias.X)) * FQuat(CurrentRight, FMath::DegreesToRadians(Bias.Y)) * Transform.GetRotation();

			// Transform look-at to localspace so we can limit the angles efficiently
			const FVector TargetDirection = (Target - Transform.GetLocation()).GetSafeNormal();
			const FVector LocalDirection = CenterRotation.Inverse() * TargetDirection;

			const FQuat LocalOffset = AxisOffset.Quaternion();
			const FVector LocalForward = LocalOffset.GetAxisY(); 
			const FVector LocalRight = -LocalOffset.GetAxisX();
			const FVector LocalUp = LocalOffset.GetAxisZ();

			FVector Intensities;
			Intensities.Y = FRigUnit_SoftLimitValue::SoftLimit((LocalDirection | LocalRight) * Intensity, FMath::DegreesToRadians(Range.X));
			Intensities.Z = FRigUnit_SoftLimitValue::SoftLimit((LocalDirection | LocalUp) * Intensity, FMath::DegreesToRadians(Range.Y));
			Intensities.X = FMath::Sqrt(1.0f - (FMath::Square(Intensities.Y) + FMath::Square(Intensities.Z)));

			const FVector Restricted = LocalForward * Intensities.X + LocalRight * Intensities.Y + LocalUp * Intensities.Z;

			// Transform back to global space and set bone transform
			const FVector FinalDirection = CenterRotation * Restricted;
			const FQuat Between = FQuat::FindBetween(CurrentForward, FinalDirection);

			Transform.SetRotation(Between * Transform.GetRotation() * OffsetRotation.Quaternion());

			Hierarchy->SetGlobalTransform(Cache, Transform, bPropagateToChildren);

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, Transform.GetLocation(), Transform.GetLocation() + FinalDirection * 15.0f, FLinearColor::White, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, Transform.GetLocation(), Transform.GetLocation() + TargetDirection * 20.0f, FLinearColor::Black, DebugSettings.Scale * 0.25f);
			}

			Direction = FinalDirection.GetSafeNormal();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FRigUnit_EyelidDisplacement_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT();
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		Cache.Reset();
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		if (!Cache.UpdateCache(Key, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("key '%s' is not valid."), *Key.ToString());
		}
		else
		{
			// Get current transform basis
			FTransform Transform = FTransform(AxisOffset) * Hierarchy->GetGlobalTransform(Cache);
			const FVector CurrentForward = Transform.GetUnitAxis(EAxis::Y);
			const FVector CurrentRight = -Transform.GetUnitAxis(EAxis::X);
			const FVector CurrentUp = Transform.GetUnitAxis(EAxis::Z);

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, Transform.GetLocation(), Transform.GetLocation() + CurrentForward * 10.0f, FLinearColor::Red, DebugSettings.Scale * 0.1f);
				Context.DrawInterface->DrawLine(FTransform::Identity, Transform.GetLocation(), Transform.GetLocation() + CurrentRight * 10.0f, FLinearColor::Green, DebugSettings.Scale * 0.1f);
				Context.DrawInterface->DrawLine(FTransform::Identity, Transform.GetLocation(), Transform.GetLocation() + CurrentUp * 10.0f, FLinearColor::Blue, DebugSettings.Scale * 0.1f);
			}

			// Range from the bottom (starting at 0) all the way to the top

			// Compute bottom with input offsets
			const float CenterRadians = FMath::DegreesToRadians(IrisBias);
			const FQuat CenterRotation = FQuat(CurrentRight, CenterRadians);
			const FVector CenterDirection = CenterRotation * CurrentUp;

			// Compute eyelid angle
			const float IrisRadians = FMath::DegreesToRadians(IrisDisplacement);
			const float RangeRadians = FMath::DegreesToRadians(Range);
			const float EyeRadians = FRigUnit_SoftLimitValue::SoftLimit(IrisRadians - FMath::Asin(CenterDirection | IrisDirection), RangeRadians);
			const float LidRadians = FMath::Lerp(FMath::Lerp(EyeRadians, RangeRadians, Openness), -RangeRadians, Closeness);
			const FQuat LidRotation = FQuat(CurrentRight, CenterRadians + LidRadians);

			Transform.SetRotation(LidRotation * Transform.GetRotation() * OffsetRotation.Quaternion());

			Hierarchy->SetGlobalTransform(Cache, Transform, bPropagateToChildren);

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, Transform.GetLocation(), Transform.GetLocation() + CenterRotation * CurrentUp * 15.0f, FLinearColor::White, DebugSettings.Scale * 0.5f);
			}

			Position = LidRadians / RangeRadians;
		}
	}
}
