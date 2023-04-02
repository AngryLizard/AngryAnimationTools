

#include "ControlRig/IK/RigUnit_ClavicleIK.h"
#include "ControlRig/IK/RigUnit_HingeIK.h"
#include "ControlRig/RigUnit_BendTowards.h"

#include "ControlRig.h"
#include "Units/RigUnitContext.h"

FRigUnit_ClaviceIK_Execute()
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
			const FVector EELocation = EndEETarget.GetLocation();

			// Compute initial leg properties
			const FTransform InitialClavicle = Hierarchy->GetGlobalTransform(Chain[0]);
			const FTransform InitialUpperArm = Hierarchy->GetGlobalTransform(Chain[1]);
			const FTransform InitialLowerArm = Hierarchy->GetGlobalTransform(Chain[2]);
			const FTransform InitialHand = Hierarchy->GetGlobalTransform(Chain[3]);

			FVector Lengths;
			Lengths.X = (InitialClavicle.GetLocation() - InitialUpperArm.GetLocation()).Size() * Customisation.X;
			Lengths.Y = (InitialUpperArm.GetLocation() - InitialLowerArm.GetLocation()).Size() * Customisation.Y;
			Lengths.Z = (InitialLowerArm.GetLocation() - InitialHand.GetLocation()).Size() * Customisation.Z;

			const FVector InitialEllbowDelta = InitialLowerArm.GetLocation() - InitialUpperArm.GetLocation();
			const FVector InitialHandDelta = InitialHand.GetLocation() - InitialUpperArm.GetLocation();

			// Compute clavicle offset
			const FTransform Clavicle = Hierarchy->GetGlobalTransform(Chain[0]);
			const FTransform Shoulder = Hierarchy->GetGlobalTransform(Chain[1]);
			const FVector ClavicleShoulderDelta = Shoulder.GetLocation() - Clavicle.GetLocation();
			const FVector ClavicleShoulderNormal = ClavicleShoulderDelta.GetSafeNormal();
			const FVector ClavicleObjectiveDelta = EELocation - Clavicle.GetLocation();
			const float ClavicleObjectiveDistance = ClavicleObjectiveDelta.Size();
			const FVector ClavicleObjectiveNormal = ClavicleObjectiveDelta / ClavicleObjectiveDistance;

			// Compute clavicle location
			const float MaxClavicleRadians = FMath::DegreesToRadians(MaxClavicleAngle);
			const FVector ClavicleOrthogonal = FVector::VectorPlaneProject(ClavicleObjectiveNormal, ClavicleShoulderNormal).GetSafeNormal();

			const float MaxArmLength = Lengths.Y + Lengths.Z;
			const float ClavicleIntensity = FMath::Min(1.0f - (ClavicleShoulderDelta.GetSafeNormal() | ClavicleObjectiveNormal), 1.0f);
			const float ClavicleDistancity = FMath::Min((Shoulder.GetLocation() - EELocation).Size() / MaxArmLength, 1.0f);
			const FVector ClavicleBiasVector = Clavicle.TransformVectorNoScale(ClavicleBias);
			const float BiasIntensity = FMath::Exp(ClavicleBiasVector | ClavicleObjectiveNormal);
			const float ClavicleSin = FMath::Sin(MaxClavicleRadians * BiasIntensity) * ClavicleIntensity * ClavicleDistancity;
			const FVector FinalClavicleNormal = ClavicleShoulderNormal * FMath::Sqrt(1.0f - ClavicleSin * ClavicleSin) + ClavicleOrthogonal * ClavicleSin;
			const FVector ClavicleLocation = Clavicle.GetLocation() + FinalClavicleNormal * Lengths.X;

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, Clavicle.GetLocation(), Clavicle.GetLocation() + ClavicleBiasVector * 30.0f, FLinearColor::White, DebugSettings.Scale * 0.5f);
				Context.DrawInterface->DrawLine(FTransform::Identity, Clavicle.GetLocation(), Clavicle.GetLocation() + ClavicleOrthogonal * 30.0f, FLinearColor::Blue, DebugSettings.Scale * 0.3f);
				Context.DrawInterface->DrawLine(FTransform::Identity, Clavicle.GetLocation(), Clavicle.GetLocation() + ClavicleShoulderNormal * 30.0f, FLinearColor::Green, DebugSettings.Scale * 0.3f);
			}

			// Compute minimal distance between pelvis and ankle
			const float CosMinEllbowAngle = FMath::Cos(FMath::DegreesToRadians(MinEllbowAngle));
			const float MinArmDistance = FMath::Sqrt(Lengths.X * Lengths.X + Lengths.Y * Lengths.Y - 2.0f * Lengths.X * Lengths.Y * CosMinEllbowAngle);

			// Compute objective deltas
			const FVector ObjectiveDelta = EELocation - ClavicleLocation;
			const float ObjectiveNorm = ObjectiveDelta.Size();
			const FVector ObjectiveNormal = ObjectiveDelta / ObjectiveNorm;
			const float ObjectiveDistance = FMath::Max(ObjectiveNorm, MinArmDistance);
			const FVector ObjectiveLocation = ClavicleLocation + ObjectiveNormal * ObjectiveDistance;

			// Compute proper rotation invariant direction according to initial pose
			const FVector InitialHandNormal = InitialHandDelta.GetSafeNormal();
			const FQuat ArmRotation = FQuat::FindBetweenNormals(InitialHandNormal, ObjectiveNormal);
			const FVector InitialArmDelta = FVector::VectorPlaneProject(InitialEllbowDelta, InitialHandNormal);
			const FVector ArmDirection = (ArmRotation * InitialArmDelta).GetSafeNormal();

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, ClavicleLocation, ClavicleLocation + ArmDirection * 50.0f, FLinearColor::White, DebugSettings.Scale * 0.5f);
			}

			// Compute directions
			const FVector EEAlignment = Objective.TransformVectorNoScale(EllbowAlignmentAxis);
			const FVector HandAlignment = EEForwardTarget * (EEAlignment | ObjectiveNormal) - EEAlignment * (EEForwardTarget | ObjectiveNormal);
			const FVector LowerDirection = FVector::VectorPlaneProject(HandAlignment * Lengths.Z + ArmDirection * Lengths.Y, ObjectiveNormal).GetSafeNormal();

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, ObjectiveLocation, ObjectiveLocation + HandAlignment * 30.0f, FLinearColor::Black, DebugSettings.Scale * 0.5f);
			}

			// Compute ellbow location
			float EllbowKath = 0.0f;
			float EllbowHeight = 0.0f;
			FRigUnit_HingeIK::ComputeTriangle(Lengths.Y, Lengths.Z, ObjectiveDistance, EllbowHeight, EllbowKath);

			const FVector EllbowDelta = ObjectiveNormal * EllbowKath + LowerDirection * EllbowHeight;
			const FVector EllbowLocation = ClavicleLocation + EllbowDelta;

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, EllbowLocation, EllbowLocation + LowerDirection * 20.0f, FLinearColor::Green, DebugSettings.Scale * 0.5f);

				Context.DrawInterface->DrawLine(FTransform::Identity, ClavicleLocation, EllbowLocation, FLinearColor::Red, DebugSettings.Scale);
				Context.DrawInterface->DrawLine(FTransform::Identity, EllbowLocation, ObjectiveLocation, FLinearColor::Yellow, DebugSettings.Scale);
				Context.DrawInterface->DrawPoint(FTransform::Identity, EllbowLocation, DebugSettings.Scale * 3.0f, FLinearColor::White);
			}

			// Transform to propagate along the arm
			FRigUnit_BendTowards::BendTowards(Chain[0], Chain[1], ClavicleLocation, Hierarchy, ScaleType, PropagateToChildren == EPropagation::All);
			FRigUnit_BendTowards::BendTowards(Chain[1], Chain[2], EllbowLocation, Hierarchy, ScaleType, PropagateToChildren == EPropagation::All);
			FRigUnit_BendTowards::BendTowards(Chain[2], Chain[3], ObjectiveLocation, Hierarchy, ScaleType, PropagateToChildren == EPropagation::All);

			// Set foot transform
			FTransform EE;
			EE.SetScale3D(Hierarchy->GetInitialGlobalTransform(Chain[3]).GetScale3D());
			EE.SetRotation(EndEETarget.GetRotation());
			EE.SetLocation(ObjectiveLocation);
			Hierarchy->SetGlobalTransform(Chain[3], EE, false, PropagateToChildren != EPropagation::Off);

			Stretch = ObjectiveDistance / InitialHandDelta.Size();
		}
	}
}
