

#include "ControlRig/RigUnit_Analysis.h"
#include "Math/Matrix3x3.h"

#include "ControlRig.h"
#include "Units/RigUnitContext.h"

FRigUnit_MeanDirection_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT();
	const URigHierarchy* Hierarchy = Context.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		ReferenceCache.Reset();
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		if (!Hierarchy)
		{
			return;
		}

		Output = FVector::ForwardVector;

		if (!ReferenceCache.UpdateCache(ReferenceKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Mean '%s' is not valid."), *ReferenceKey.ToString());
		}
		else
		{
			const FVector Mean = Hierarchy->GetGlobalTransform(ReferenceCache).GetLocation();

			const int32 Num = Chain.Num();
			if (Num > 1)
			{
				const FVector First = Hierarchy->GetGlobalTransform(Chain.First()).GetLocation();
				const FVector Last = Hierarchy->GetGlobalTransform(Chain.Last()).GetLocation();
				const float InvSquared = 1.0f / (Last - First).SizeSquared();

				float MeanWeight = 0.0f;
				FVector MeanSum = FVector::ZeroVector;
				for (int32 Index = 1; Index < Num; Index++)
				{
					const FVector Location = Hierarchy->GetGlobalTransform(Chain[Index]).GetLocation();
					const float Weight = (Location - Mean).SizeSquared() * InvSquared;
					MeanSum += (Location - First) * Weight;
					MeanWeight += Weight;
				}

				Output = (MeanSum / MeanWeight).GetSafeNormal();
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FRigUnit_PowerDirection_Execute()
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

		Output = FVector::ForwardVector;

		TArray<FVector> Samples;
		const int32 Num = Chain.Num();
		Samples.Reserve(Num);
		if (Num >= 2)
		{
			FVector Mean = FVector::ZeroVector;
			for (int32 Index = 0; Index < Num; Index++)
			{
				const FVector Sample = Hierarchy->GetGlobalTransform(Chain[Index]).GetLocation();
				Samples.Emplace(Sample);
				Mean += Sample;
			}
			Mean /= Num;

			for (FVector& Sample : Samples)
			{
				Sample -= Mean;
			}

			Output = (Samples[1] - Samples[0]).GetSafeNormal();
			Output = FMatrix3x3(Samples).PowerMethod(Output, Iterations);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float FRigUnit_ChainAnalysis::ComputeInitialChainLength(const FRigElementKeyCollection& Chain, const URigHierarchy* Hierarchy)
{
	float Length = 0.0f;

	const int32 Num = Chain.Num();
	if (Num > 2)
	{
		FVector Current = Hierarchy->GetInitialGlobalTransform(Chain[0]).GetLocation();
		for (int32 Index = 1; Index < Num; Index++)
		{
			const FVector Next = Hierarchy->GetInitialGlobalTransform(Chain[Index]).GetLocation();
			Length += (Current - Next).Size();
			Current = Next;
		}
	}
	return Length;
}

void FRigUnit_ChainAnalysis::Analysis(const FRigElementKeyCollection& Chain, const URigHierarchy* Hierarchy, float Multiplier, float& MaxLength, float& CurrentLength, float& InitialLength)
{
	MaxLength = FRigUnit_ChainAnalysis::ComputeInitialChainLength(Chain, Hierarchy) * Multiplier;

	const FVector First = Hierarchy->GetGlobalTransform(Chain.First()).GetLocation();
	const FVector Last = Hierarchy->GetGlobalTransform(Chain.Last()).GetLocation();
	CurrentLength = (Last - First).Size();

	const FVector InitialFirst = Hierarchy->GetInitialGlobalTransform(Chain.First()).GetLocation();
	const FVector InitialLast = Hierarchy->GetInitialGlobalTransform(Chain.Last()).GetLocation();
	InitialLength = (InitialLast - InitialFirst).Size();
}

FRigUnit_ChainAnalysis_Execute()
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

		if (Chain.Num() < 2)
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Chain has to have length at least 2."));
		}
		else
		{
			Analysis(Chain, Hierarchy, LengthMultiplier, MaxLength, CurrentLength, InitialLength);

			if (FMath::IsNearlyZero(MaxLength))
			{
				UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Max chain length is null."));
			}
			else
			{
				LengthRatio = CurrentLength / MaxLength;
			}
		}
	}
}