

#include "ControlRig/IK/RigUnit_ArmIK.h"
#include "ControlRig/IK/RigUnit_HingeIK.h"
#include "ControlRig/RigUnit_BendTowards.h"

#include "ControlRig.h"
#include "Units/RigUnitContext.h"

FRigUnit_ArmIK_Execute()
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
		const FVector EEForwardTarget = GET_IK_OBJECTIVE_FORWARD();
		const FVector EELocation = EndEETarget.GetLocation();

		// Compute initial leg properties
		const FTransform InitialUpperArm = Hierarchy->GetGlobalTransform(Chain[0]);
		const FTransform InitialLowerArm = Hierarchy->GetGlobalTransform(Chain[1]);
		const FTransform InitialHand = Hierarchy->GetGlobalTransform(Chain[2]);

		FVector2D Lengths;
		Lengths.X = (InitialUpperArm.GetLocation() - InitialLowerArm.GetLocation()).Size() * Customisation.X;
		Lengths.Y = (InitialLowerArm.GetLocation() - InitialHand.GetLocation()).Size() * Customisation.Y;

		const FVector InitialEllbowDelta = InitialLowerArm.GetLocation() - InitialUpperArm.GetLocation();
		const FVector InitialHandDelta = InitialHand.GetLocation() - InitialUpperArm.GetLocation();

		// Compute shoulder direction
		const FTransform Shoulder = Hierarchy->GetGlobalTransform(Chain[0]);
		const FVector ShoulderObjectiveDelta = EELocation - Shoulder.GetLocation();
		const float ShoulderObjectiveDistance = ShoulderObjectiveDelta.Size();
		const FVector ShoulderObjectiveNormal = ShoulderObjectiveDelta / ShoulderObjectiveDistance;
		const FVector ShoulderLocation = Shoulder.GetLocation();

		// Compute minimal distance between pelvis and ankle
		const float CosMinEllbowAngle = FMath::Cos(FMath::DegreesToRadians(MinEllbowAngle));
		const float MinArmDistance = FMath::Sqrt(Lengths.X * Lengths.X + Lengths.Y * Lengths.Y - 2.0f * Lengths.X * Lengths.Y * CosMinEllbowAngle);

		// Compute objective deltas
		const FVector ObjectiveDelta = EELocation - ShoulderLocation;
		const float ObjectiveNorm = ObjectiveDelta.Size();
		const FVector ObjectiveNormal = ObjectiveDelta / ObjectiveNorm;
		const float ObjectiveDistance = FMath::Max(ObjectiveNorm, MinArmDistance);
		const FVector ObjectiveLocation = ShoulderLocation + ObjectiveNormal * ObjectiveDistance;

		// Compute proper rotation invariant direction according to initial pose
		const FVector InitialHandNormal = InitialHandDelta.GetSafeNormal();
		const FQuat ArmRotation = FQuat::FindBetweenNormals(InitialHandNormal, ObjectiveNormal) * EllbowOffset.Quaternion();
		const FVector InitialArmDelta = FVector::VectorPlaneProject(InitialEllbowDelta, InitialHandNormal);
		const FVector ArmDirection = (ArmRotation * InitialArmDelta).GetSafeNormal();

		if (DebugSettings.bEnabled)
		{
			DrawInterface->DrawLine(FTransform::Identity, ShoulderLocation, ShoulderLocation + ArmDirection * 50.0f, FLinearColor::White, DebugSettings.Scale * 0.5f);
		}

		// Compute directions
		const FVector EEAlignment = Objective.TransformVectorNoScale(EllbowAlignmentAxis);
		const FVector HandAlignment = EEForwardTarget * (EEAlignment | ObjectiveNormal) - EEAlignment * (EEForwardTarget | ObjectiveNormal);
		const FVector LowerDirection = FVector::VectorPlaneProject(HandAlignment * Lengths.Y + ArmDirection * Lengths.X, ObjectiveNormal).GetSafeNormal();

		if (DebugSettings.bEnabled)
		{
			DrawInterface->DrawLine(FTransform::Identity, ObjectiveLocation, ObjectiveLocation + HandAlignment * 30.0f, FLinearColor::Black, DebugSettings.Scale * 0.5f);
		}

		// Compute ellbow location
		float EllbowKath = 0.0f;
		float EllbowHeight = 0.0f;
		FRigUnit_HingeIK::ComputeTriangle(Lengths.X, Lengths.Y, ObjectiveDistance, EllbowHeight, EllbowKath);

		const FVector EllbowDelta = ObjectiveNormal * EllbowKath + LowerDirection * EllbowHeight;
		const FVector EllbowLocation = ShoulderLocation + EllbowDelta;

		if (DebugSettings.bEnabled)
		{
			DrawInterface->DrawLine(FTransform::Identity, EllbowLocation, EllbowLocation + LowerDirection * 20.0f, FLinearColor::Green, DebugSettings.Scale * 0.5f);

			DrawInterface->DrawLine(FTransform::Identity, ShoulderLocation, EllbowLocation, FLinearColor::Red, DebugSettings.Scale);
			DrawInterface->DrawLine(FTransform::Identity, EllbowLocation, ObjectiveLocation, FLinearColor::Yellow, DebugSettings.Scale);
			DrawInterface->DrawPoint(FTransform::Identity, EllbowLocation, DebugSettings.Scale * 3.0f, FLinearColor::White);
		}

		// Transform to propagate along the arm
		FRigUnit_BendTowards::BendTowards(Chain[0], Chain[1], EllbowLocation, Hierarchy, ScaleType, PropagateToChildren == EPropagation::All);
		FRigUnit_BendTowards::BendTowards(Chain[1], Chain[2], ObjectiveLocation, Hierarchy, ScaleType, PropagateToChildren == EPropagation::All);

		// Set foot transform
		FTransform EE;
		EE.SetScale3D(Hierarchy->GetInitialGlobalTransform(Chain[2]).GetScale3D());
		EE.SetRotation(EndEETarget.GetRotation());
		EE.SetLocation(ObjectiveLocation);
		Hierarchy->SetGlobalTransform(Chain[2], EE, false, PropagateToChildren != EPropagation::Off);

		Stretch = ObjectiveDistance / InitialHandDelta.Size();
	}
}