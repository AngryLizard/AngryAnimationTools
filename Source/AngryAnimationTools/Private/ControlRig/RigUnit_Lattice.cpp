

#pragma once

#include "ControlRig/RigUnit_Lattice.h"

FRigUnit_LatticeTransform_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT();
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		WorkData.Weights.Reset();
		WorkData.CachedParents.Reset();
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		if (Hierarchy)
		{
			const int32 ChainNum = Chain.Num();
			const int32 LatticeNum = Lattice.Num();
			if (ChainNum == 0 || LatticeNum == 0)
			{
				UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Chain has to have length at least 1."));
				return;
			}

			if (WorkData.Weights.Num() != ChainNum)
			{
				WorkData.Weights.SetNumZeroed(ChainNum);
				for (int32 ChainIndex = 0; ChainIndex < ChainNum; ChainIndex++)
				{
					const FTransform ChainTransform = Hierarchy->GetGlobalTransform(Chain[ChainIndex]);

					TArray<float>& Weights = WorkData.Weights[ChainIndex];
					Weights.SetNumZeroed(LatticeNum);
					for (int32 LatticeIndex = 0; LatticeIndex < LatticeNum; LatticeIndex++)
					{
						const FTransform LatticeTransform = Hierarchy->GetGlobalTransform(Lattice[LatticeIndex].Key);

						const FVector X = LatticeTransform.InverseTransformPosition(ChainTransform.GetLocation());
						const FVector D = X / Lattice[LatticeIndex].Distribution.ComponentMax(FVector(1.));

						const float Weight = static_cast<float>(FMath::Exp(D.SizeSquared() * -1.));
						Weights[LatticeIndex] = Weight;
					}
				}
			}

			if (WorkData.CachedParents.Num() != LatticeNum)
			{
				WorkData.CachedParents.SetNumZeroed(LatticeNum);
				for (int32 LatticeIndex = 0; LatticeIndex < LatticeNum; LatticeIndex++)
				{
					FCachedRigElement& CachedParent = WorkData.CachedParents[LatticeIndex];
					CachedParent.Reset();

					FRigElementKey Parent = Context.Hierarchy->GetFirstParent(Lattice[LatticeIndex].Key);
					if (Parent.IsValid())
					{
						CachedParent.UpdateCache(Parent, Context.Hierarchy);
					}
				}
			}

			TArray<FTransform> LatticeDeltaTransforms;
			LatticeDeltaTransforms.SetNumUninitialized(LatticeNum);
			for (int32 LatticeIndex = 0; LatticeIndex < LatticeNum; LatticeIndex++)
			{
				const FTransform CurrentLatticeTransform = Hierarchy->GetLocalTransform(Lattice[LatticeIndex].Key, false);
				const FTransform InitialLatticeTransform = Hierarchy->GetLocalTransform(Lattice[LatticeIndex].Key, true);
				const FTransform ParentLatticeTransform = Hierarchy->GetGlobalTransform(WorkData.CachedParents[LatticeIndex].GetKey(), false);

				const FTransform CurrentTransform = CurrentLatticeTransform * ParentLatticeTransform;
				const FTransform InitialTransform = InitialLatticeTransform * ParentLatticeTransform;

				LatticeDeltaTransforms[LatticeIndex] = FTransform(
					CurrentTransform.GetRotation() * InitialTransform.GetRotation().Inverse(),
					CurrentTransform.GetLocation() - InitialTransform.GetLocation(),
					CurrentTransform.GetScale3D() / InitialTransform.GetScale3D());

				if (DebugSettings.bEnabled)
				{
					const FVector Lm = Lattice[LatticeIndex].Distribution.ComponentMax(FVector(1.));
					Context.DrawInterface->DrawLine(CurrentTransform, FVector(-Lm.X, 0., 0.), FVector(Lm.X, 0., 0.), FLinearColor::Red, DebugSettings.Scale * 0.5f);
					Context.DrawInterface->DrawLine(CurrentTransform, FVector(0., -Lm.Y, 0.), FVector(0., Lm.Y, 0.), FLinearColor::Green, DebugSettings.Scale * 0.5f);
					Context.DrawInterface->DrawLine(CurrentTransform, FVector(0., 0., -Lm.Z), FVector(0., 0., Lm.Z), FLinearColor::Blue, DebugSettings.Scale * 0.5f);

					Context.DrawInterface->DrawPoint(InitialTransform, FVector(0., 0., 0.), DebugSettings.Scale * 1.f, FLinearColor::Yellow);
				}
			}

			TArray<FTransform> Transforms;
			Transforms.SetNumUninitialized(ChainNum);
			for (int32 ChainIndex = 0; ChainIndex < ChainNum; ChainIndex++)
			{
				FTransform FieldTransform = Hierarchy->GetGlobalTransform(Chain[ChainIndex]);
				const TArray<float>& Weights = WorkData.Weights[ChainIndex];

				for (int32 LatticeIndex = 0; LatticeIndex < LatticeNum; LatticeIndex++)
				{
					const float Weight = FMath::Min(Weights[LatticeIndex], 1.f);
					const FTransform CurrentLatticeTransform = Hierarchy->GetGlobalTransform(Lattice[LatticeIndex].Key, false);

					const FTransform LatticeDeltaTransform = LatticeDeltaTransforms[LatticeIndex];
					const FVector Delta = FieldTransform.GetLocation() - CurrentLatticeTransform.GetLocation();

					FieldTransform.AddToTranslation((LatticeDeltaTransform.TransformPosition(Delta) - Delta) * Weight);
					//FTransform::BlendFromIdentityAndAccumulate(FieldTransform, LatticeTransforms[LatticeIndex], ScalarRegister(Weight));
				}

				//FieldTransform.SetScale3D(FVector::OneVector);
				Transforms[ChainIndex] = FieldTransform;
			}


			Hierarchy->SetGlobalTransform(Chain[0], Transforms[0], false, PropagateToChildren);
			for (int32 ChainIndex = 1; ChainIndex < ChainNum; ChainIndex++)
			{
				FRigUnit_BendTowards::BendTowards(Chain[ChainIndex-1], Chain[ChainIndex], Transforms[ChainIndex].GetLocation(), Hierarchy, ScaleType, PropagateToChildren);
			}
		}
	}
}