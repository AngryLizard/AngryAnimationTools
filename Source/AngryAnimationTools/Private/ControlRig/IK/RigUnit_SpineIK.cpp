

#include "ControlRig/IK/RigUnit_SpineIK.h"
#include "ControlRig/RigUnit_Constraints.h"
#include "ControlRig/IK/RigUnit_ConeFABRIK.h"

#include "ControlRig.h"
#include "Units/RigUnitContext.h"

void WeightedMean(const FRigUnitContext& Context, const FDebugSettings& DebugSettings, TArray<FTransform>& Transforms, const TArray<FTransform>& A, const TArray<FTransform>& B, float Bias)
{
	// Both ends always match
	Transforms[0] = A[0];
	Transforms.Last() = B.Last();

	// Compute weighted average
	const int32 Num = Transforms.Num();
	for (int32 Index = Num - 1; Index > 0; Index--)
	{
		const float Ratio = ((float)Index) / (Num - 1);
		const float Weight = FMath::Lerp(FMath::Square(Ratio), 1.0f - FMath::Square(1.0f - Ratio), Bias);
		Transforms[Index].Blend(A[Index], B[Index], Weight);

		if (DebugSettings.bEnabled)
		{
			Context.DrawInterface->DrawPoint(FTransform::Identity, Transforms[Index].GetLocation(), DebugSettings.Scale * 5.0f, FLinearColor::Yellow);
		}
	}
}

void Straighten(TArray<FTransform>& Transforms, const TArray<FTransform>& Rest)
{
	// Rotate bones to align with given rest pose
	const int32 Num = Transforms.Num();
	for (int32 Index = 1; Index < Num - 1; Index++)
	{
		const FVector Target = Transforms[Index].InverseTransformPosition(Transforms[Index + 1].GetLocation());
		const FQuat Rotation = FQuat::FindBetweenVectors(Rest[Index + 1].GetLocation(), Target);
		Transforms[Index].SetRotation(Transforms[Index].GetRotation() * Rotation);
	}
}

void InitialiseBendTransforms(
	const FRigUnitContext& Context,
	const FDebugSettings& DebugSettings,
	const FRigElementKeyCollection& Chain,
	const FTransform& StartEE, const FVector& StartOffset, float StartRadius,
	const FTransform& EndEETarget, const FVector& EndOffset, float EndRadius,
	TArray<FTransform>& Rest,
	TArray<FTransform>& StartChain,
	TArray<FTransform>& EndChain,
	TArray<FTransform>& Transforms)
{
	const int32 ChainNum = Chain.Num();

	// Populate transform lists
	Rest.Reserve(ChainNum);
	StartChain.Reserve(ChainNum);
	EndChain.Reserve(ChainNum);
	Transforms.Reserve(ChainNum);
	for (int32 Index = 0; Index < ChainNum; Index++)
	{
		Rest.Emplace(Context.Hierarchy->GetInitialLocalTransform(Chain[Index]));
		Transforms.Emplace(Context.Hierarchy->GetGlobalTransform(Chain[Index]));
		StartChain.Emplace(StartEE);
		EndChain.Emplace(EndEETarget);
	}

	const FQuat StartOffsetRotation = FQuat(StartOffset.GetSafeNormal(), FMath::DegreesToRadians(StartOffset.Size()));
	const FQuat StartLimited = FRigUnit_LimitRotation::LimitRotation(StartOffsetRotation * StartEE.GetRotation().Inverse() * Transforms.Last().GetRotation(), FMath::DegreesToRadians(StartRadius), true);
	StartChain[0].SetRotation(StartEE.GetRotation() * StartOffsetRotation.Inverse() * StartLimited);

	const FQuat EndOffsetRotation = FQuat(EndOffset.GetSafeNormal(), FMath::DegreesToRadians(EndOffset.Size()));
	const FQuat EndLimited = FRigUnit_LimitRotation::LimitRotation(EndOffsetRotation * EndEETarget.GetRotation().Inverse() * Transforms.Last().GetRotation(), FMath::DegreesToRadians(EndRadius), true);
	EndChain.Last().SetRotation(EndEETarget.GetRotation() * EndOffsetRotation.Inverse() * EndLimited);

	// Compute separate global transforms assuming rest pose for both ends
	for (int32 Index = 1; Index < ChainNum; Index++)
	{
		StartChain[Index] = Rest[Index] * StartChain[Index - 1];
		EndChain[ChainNum - Index - 1] = Rest[ChainNum - Index].Inverse() * EndChain[ChainNum - Index];

		if (DebugSettings.bEnabled)
		{
			Context.DrawInterface->DrawPoint(FTransform::Identity, StartChain[Index].GetLocation(), DebugSettings.Scale * 5.0f, FLinearColor::Red);
			Context.DrawInterface->DrawPoint(FTransform::Identity, EndChain[ChainNum - Index - 1].GetLocation(), DebugSettings.Scale * 5.0f, FLinearColor::Blue);
		}
	}
}

void ChainForwardSolve(const FRigUnitContext& Context, const FDebugSettings& DebugSettings, const TArray<FTransform>& Rest, const TArray<FTransform>& Transforms, TArray<FTransform>& StartChain, float MaxAnchorRadians)
{
	const int32 ChainNum = Rest.Num();
	for (int32 Index = 1; Index < ChainNum; Index++)
	{
		FTransform Regular = Rest[Index];
		const float RegularDistance = (Transforms[Index].GetLocation() - Transforms[Index - 1].GetLocation()).Size();
		Regular.SetLocation(Regular.GetLocation().GetClampedToMaxSize(RegularDistance));

		const FQuat Rotation = FRigUnit_ConeFABRIK::SoftRotate(Regular, StartChain[Index - 1], Transforms[Index], MaxAnchorRadians);
		StartChain[Index] = Regular * Rotation * StartChain[Index - 1];

		if (DebugSettings.bEnabled)
		{
			Context.DrawInterface->DrawPoint(FTransform::Identity, StartChain[Index].GetLocation(), DebugSettings.Scale * 7.5f, FLinearColor::White);
		}
	}
}

void ChainBackwardSolve(const FRigUnitContext& Context, const FDebugSettings& DebugSettings, const TArray<FTransform>& Rest, const TArray<FTransform>& Transforms, TArray<FTransform>& EndChain, float MaxObjectiveRadians)
{
	const int32 ChainNum = Rest.Num();
	for (int32 Index = ChainNum - 1; Index >= 1; Index--)
	{
		FTransform Regular = Rest[Index];
		const float RegularDistance = (Transforms[Index].GetLocation() - Transforms[Index - 1].GetLocation()).Size();
		Regular.SetLocation(Regular.GetLocation().GetClampedToMaxSize(RegularDistance));

		const FTransform& RegularInv = Regular.Inverse();
		const FQuat Rotation = FRigUnit_ConeFABRIK::SoftRotate(RegularInv, EndChain[Index], Transforms[Index - 1], MaxObjectiveRadians);
		EndChain[Index - 1] = RegularInv * Rotation * EndChain[Index];

		if (DebugSettings.bEnabled)
		{
			Context.DrawInterface->DrawPoint(FTransform::Identity, EndChain[Index - 1].GetLocation(), DebugSettings.Scale * 7.5f, FLinearColor::Black);
		}
	}
}

FRigUnit_SpineIK_Execute()
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
			const FTransform StartEE = Context.Hierarchy->GetGlobalTransform(Chain.First());
			const FTransform EndEETarget = GET_IK_OBJECTIVE_TRANSFORM();

			TArray<FTransform> Rest, StartChain, EndChain, Transforms;
			InitialiseBendTransforms(Context, DebugSettings, Chain, 
				StartEE, ObjectiveSettings.LimitBias, ObjectiveSettings.LimitRadius, 
				EndEETarget, AnchorSettings.LimitBias, AnchorSettings.LimitRadius, 
				Rest, StartChain, EndChain, Transforms);

			// Collapse start and end chain into one
			WeightedMean(Context, DebugSettings, Transforms, StartChain, EndChain, 0.0f);

			const float MaxAnchorRadians = FMath::DegreesToRadians(AnchorSettings.AngleLimit);
			const float MaxObjectiveRadians = FMath::DegreesToRadians(ObjectiveSettings.AngleLimit);
			for (int32 Iteration = 0; Iteration < Iterations; Iteration++)
			{
				// Apply one FABRIK iteration to both directions
				ChainForwardSolve(Context, DebugSettings, Rest, Transforms, StartChain, MaxAnchorRadians);
				ChainBackwardSolve(Context, DebugSettings, Rest, Transforms, EndChain, MaxObjectiveRadians);

				// Collapse both FABRIK iterations into one
				WeightedMean(Context, DebugSettings, Transforms, StartChain, EndChain, 1.0f);
			}

			// Make sure all bones are properly rotated
			Straighten(Transforms, Rest);

			Transforms[0] = StartEE;
			Transforms.Last() = EndEETarget;

			// Set bones to transforms
			Hierarchy->SetGlobalTransform(Chain[0], Transforms[0], false, PropagateToChildren != EPropagation::Off);
			for (int32 Index = 1; Index < ChainNum - 1; Index++)
			{
				Hierarchy->SetGlobalTransform(Chain[Index], Transforms[Index], false, PropagateToChildren == EPropagation::All);
			}
			Hierarchy->SetGlobalTransform(Chain.Last(), Transforms.Last(), false, PropagateToChildren != EPropagation::Off);
		}
	}
}