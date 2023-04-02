

#include "ControlRig/IK/RigUnit_DigitigradeIK.h"
#include "ControlRig/IK/RigUnit_HingeIK.h"
#include "ControlRig/RigUnit_BendTowards.h"

#include "ControlRig.h"
#include "Units/RigUnitContext.h"

FRigUnit_DigitigradeIK_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT();
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		if (Chain.Num() < 4)
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Chain has to have length at least 4."));
		}
		else
		{
			// Objective properties
			const FTransform EndEETarget = GET_IK_OBJECTIVE_TRANSFORM();
			const FVector EEForwardTarget = GET_IK_OBJECTIVE_FORWARD();
			const FVector EEUpTarget = GET_IK_OBJECTIVE_UP();

			// Compute initial leg properties
			const FTransform InitialUpperLeg = Hierarchy->GetGlobalTransform(Chain[0]);
			const FTransform InitialLowerLeg = Hierarchy->GetGlobalTransform(Chain[1]);
			const FTransform InitialAnkle = Hierarchy->GetGlobalTransform(Chain[2]);
			const FTransform InitialFoot = Hierarchy->GetGlobalTransform(Chain[3]);

			// Compute objective deltas
			const FTransform UpperLeg = Hierarchy->GetGlobalTransform(Chain[0]);
			const FVector PelvisLocation = UpperLeg.GetLocation();
			const FVector ObjectiveDelta = EndEETarget.GetLocation() - PelvisLocation;
			const float ObjectiveNorm = ObjectiveDelta.Size();
			const FVector ObjectiveNormal = ObjectiveDelta / ObjectiveNorm;

			FVector Lengths;
			Lengths.X = (InitialUpperLeg.GetLocation() - InitialLowerLeg.GetLocation()).Size() * Customisation.X;
			Lengths.Y = (InitialLowerLeg.GetLocation() - InitialAnkle.GetLocation()).Size() * Customisation.Y;
			Lengths.Z = (InitialAnkle.GetLocation() - InitialFoot.GetLocation()).Size() * Customisation.Z;

			const FVector InitialAnkleDelta = InitialAnkle.GetLocation() - InitialUpperLeg.GetLocation();
			const FVector InitialKneeDelta = InitialLowerLeg.GetLocation() - InitialUpperLeg.GetLocation();
			const FVector InitialFootDelta = InitialFoot.GetLocation() - InitialUpperLeg.GetLocation();

			// Compute minimal distance between pelvis and ankle
			const float CosMinKneeAngle = FMath::Cos(FMath::DegreesToRadians(MinKneeAngle));
			const float HypothSqr = Lengths.X * Lengths.X + Lengths.Y * Lengths.Y - 2.0f * Lengths.X * Lengths.Y * CosMinKneeAngle;
			const float Hypoth = FMath::Sqrt(HypothSqr);

			// Compute minimal objective distance to not break angle constraints
			const float MinHeelRadians = FMath::Acos((HypothSqr + Lengths.Y * Lengths.Y - Lengths.X * Lengths.X) / (2.0f * Hypoth * Lengths.Y));
			const float CosMinHeelAngle = FMath::Cos(MinHeelRadians + FMath::DegreesToRadians(MinAnkleAngle));
			const float MinLegDistance = FMath::Sqrt(HypothSqr + Lengths.Z * Lengths.Z - 2.0f * Hypoth * Lengths.Z * CosMinHeelAngle);

			// Compute projected objective
			const float ObjectiveDistance = FMath::Max(ObjectiveNorm, MinLegDistance);
			const FVector ObjectiveLocation = PelvisLocation + ObjectiveNormal * ObjectiveDistance;

			// Compute proper rotation invariant direction according to initial pose
			const FVector InitialFootNormal = InitialFootDelta.GetSafeNormal();
			const FQuat LegRotation = FQuat::FindBetweenNormals(InitialFootNormal, ObjectiveNormal);
			const FVector InitialKneeDirection = FVector::VectorPlaneProject(InitialKneeDelta - InitialAnkleDelta, InitialFootNormal);
			const FVector LegDirection = (LegRotation * InitialKneeDirection).GetSafeNormal();

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, PelvisLocation, PelvisLocation + LegDirection * 50.0f, FLinearColor::White, DebugSettings.Scale * 0.5f);
			}

			// Compute directions
			const FVector FootDirection = -EEForwardTarget * (EEUpTarget | ObjectiveNormal) + EEUpTarget * (EEForwardTarget | ObjectiveNormal);
			const FVector LowerDirection = FVector::VectorPlaneProject(FootDirection * (Lengths.X + Lengths.Y) + LegDirection * Lengths.Z * AnkleKneeDirectionWeight, ObjectiveNormal).GetSafeNormal();

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, ObjectiveLocation, ObjectiveLocation + FootDirection * 50.0f, FLinearColor::Black, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, ObjectiveLocation, ObjectiveLocation + EEForwardTarget * 50.0f, FLinearColor::Red, DebugSettings.Scale * 2.5f);
			}

			// Interpolate ankle height
			float AnkleKath = 0.0f;
			float AnkleHeight = 0.0f;
			FRigUnit_HingeIK::ComputeTriangle(Lengths.Z, Hypoth, MinLegDistance, AnkleHeight, AnkleKath);
			const float MaxChangLength = Lengths.X + Lengths.Y + Lengths.Z;
			const float DistanceRatio = (ObjectiveDistance - MinLegDistance) / (MaxChangLength - MinLegDistance);
			const float FinalHeight = AnkleHeight * (1.0f - FMath::Pow(FMath::Min(DistanceRatio, 1.0f), StandingBend));

			// Compute ankle location
			const FVector AnkleDelta = LowerDirection * FinalHeight + ObjectiveNormal * AnkleKath;
			const FVector AnkleLocation = ObjectiveLocation - AnkleDelta.GetSafeNormal() * Lengths.Z * FMath::Max(DistanceRatio, 1.0f);
			const FVector UpperDelta = AnkleLocation - PelvisLocation;
			const float UpperDistance = UpperDelta.Size();
			const FVector UpperNormal = UpperDelta / UpperDistance;

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, ObjectiveLocation, ObjectiveLocation + AnkleDelta, FLinearColor::Blue, DebugSettings.Scale * 0.5f);
			}

			// Compute knee location
			float KneeKath = 0.0f;
			float KneeHeight = 0.0f;
			FRigUnit_HingeIK::ComputeTriangle(Lengths.X, Lengths.Y, UpperDistance, KneeHeight, KneeKath);
			const FVector UpperDirection = FVector::VectorPlaneProject(LowerDirection * Lengths.X * KneeHipDirectionWeight + LegDirection * Lengths.Y, UpperNormal).GetSafeNormal();

			const FVector KneeDelta = UpperNormal * KneeKath + UpperDirection * KneeHeight;
			const FVector KneeLocation = UpperLeg.GetLocation() + KneeDelta;

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, KneeLocation, KneeLocation + UpperDirection * 20.0f, FLinearColor::Green, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, AnkleLocation, AnkleLocation - LowerDirection * 20.0f, FLinearColor::Green, DebugSettings.Scale * 0.5f);

				Context.DrawInterface->DrawLine(FTransform::Identity, PelvisLocation, AnkleLocation, FLinearColor::Red, DebugSettings.Scale);
				Context.DrawInterface->DrawLine(FTransform::Identity, PelvisLocation, ObjectiveLocation, FLinearColor::Yellow, DebugSettings.Scale);
				Context.DrawInterface->DrawPoint(FTransform::Identity, KneeLocation, DebugSettings.Scale * 3.0f, FLinearColor::White);
			}

			// Transform to propagate along the leg
			FRigUnit_BendTowards::BendTowards(Chain[0], Chain[1], KneeLocation, Hierarchy, ScaleType, PropagateToChildren == EPropagation::All);
			FRigUnit_BendTowards::BendTowards(Chain[1], Chain[2], AnkleLocation, Hierarchy, ScaleType, PropagateToChildren == EPropagation::All);
			FRigUnit_BendTowards::BendTowards(Chain[2], Chain[3], ObjectiveLocation, Hierarchy, ScaleType, PropagateToChildren == EPropagation::All);

			// Set foot transform
			FTransform Foot;
			Foot.SetScale3D(Hierarchy->GetInitialGlobalTransform(Chain[3]).GetScale3D());
			Foot.SetRotation(EndEETarget.GetRotation());
			Foot.SetLocation(ObjectiveLocation);
			Hierarchy->SetGlobalTransform(Chain[3], Foot, false, PropagateToChildren != EPropagation::Off);

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, ObjectiveLocation, ObjectiveLocation + EEForwardTarget * 20.0f, FLinearColor::Red, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, ObjectiveLocation, ObjectiveLocation + EEUpTarget * 20.0f, FLinearColor::Blue, DebugSettings.Scale * 0.5f);
			}

			Stretch = ObjectiveDistance / InitialFootDelta.Size();
		}
	}
}