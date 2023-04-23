// The Gateway of Realities: Planes of Existence.

#include "ControlRig/RigUnit_Ellipsoid.h"
#include "ControlRig/RigUnit_Analysis.h"
#include "ControlRig/RigUnit_Constraints.h"
#include "ControlRig/RigUnit_BendTowards.h"
#include "Units/RigUnitContext.h"

void FRigUnit_EllipsoidProjection::ComputeEllispoidProjection(const FTransform& Transform, float Radius, const FVector& Point, FVector& Closest, FVector& Normal)
{
	const FVector Scale = Transform.GetScale3D();
	if (FMath::IsNearlyZero(Scale.X * Scale.Y * Scale.Z))
	{
		Closest = Transform.GetLocation();
		Normal = (Point - Closest).GetSafeNormal();
		return;
	}

	// Warp the input along scale to get an appropriate ray direction that will alyways intersect the ellipsoid again
	const FVector ScaledNormal = (Transform.InverseTransformPosition(Point) * Scale).GetSafeNormal();
	const FVector ProjectedNormal = Transform.TransformVectorNoScale((ScaledNormal / Scale)).GetSafeNormal();
	const float RayDistance = Scale.GetAbsMax() * Radius;

	// Reproject for ellipsoid intersection
	const FVector RayStart = Transform.InverseTransformPosition(Point + ProjectedNormal * RayDistance);
	const FVector RayEnd = Transform.InverseTransformPosition(Point - ProjectedNormal * RayDistance);
	const FVector RayDelta = RayEnd - RayStart;
	const float RayLength = RayDelta.Size();
	const FVector RayDir = RayDelta / RayLength;

	// Compute ellipsoid intersection
	float Value = -(RayDir | RayStart);

	const float Rd = RayStart.SizeSquared() - FMath::Square(Radius);
	const float Discr = FMath::Square(Value) - Rd;
	if (Discr >= 0.0f)
	{
		Value -= FMath::Sqrt(Discr);
	}

	// Cast location on the sphere, restrict by raylength
	const FVector IntersectionNormal = (RayStart + RayDir * FMath::Clamp(Value, 0.0f, RayLength)).GetSafeNormal();

	// Transform back to world
	Normal = Transform.TransformVectorNoScale((IntersectionNormal / Scale)).GetSafeNormal();
	Closest = Transform.TransformPosition(IntersectionNormal * Radius);
}

FRigUnit_EllipsoidProjection_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	const URigHierarchy* Hierarchy = Context.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		EllipsoidCache.Reset();
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		if (!EllipsoidCache.UpdateCache(Ellipsoid.Key, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("key '%s' is not valid."), *Ellipsoid.Key.ToString());
		}
		else
		{
			const FTransform Transform = Hierarchy->GetGlobalTransform(EllipsoidCache);
			ComputeEllispoidProjection(Transform, Ellipsoid.Radius, Point, Closest, Normal);

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, Closest, Closest + Normal * 25.0f, FLinearColor::Red, DebugSettings.Scale * 0.1f);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float FRigUnit_EllipsoidPointPlaneProject::ComputeEllispoidPointPlaneProject(const FTransform& Transform, float Radius, const FVector& Point, const FVector& Normal, FVector& Projected)
{
	const FVector Scale = Transform.GetScale3D();
	if (FMath::IsNearlyZero(Scale.X * Scale.Y * Scale.Z))
	{
		Projected = Transform.GetLocation();
		return 1.0f;
	}

	const FVector ScaledNormal = Transform.InverseTransformVector(Normal).GetSafeNormal();
	const FVector ScaledPoint = Transform.InverseTransformPosition(Point);

	const float dot = ScaledNormal | ScaledPoint;
	Projected = Transform.TransformPosition(ScaledPoint - dot * ScaledNormal);
	return dot;
}

FRigUnit_EllipsoidPointPlaneProject_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
		const URigHierarchy* Hierarchy = Context.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		EllipsoidCache.Reset();
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		if (!EllipsoidCache.UpdateCache(Ellipsoid.Key, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("key '%s' is not valid."), *Ellipsoid.Key.ToString());
		}
		else
		{
			const FTransform Transform = Hierarchy->GetGlobalTransform(EllipsoidCache);
			Distance = ComputeEllispoidPointPlaneProject(Transform, Ellipsoid.Radius, Point, Normal, Projected);

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, Projected, Point, FLinearColor::Red, DebugSettings.Scale * 0.1f);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float FRigUnit_EllipsoidRaycast::ComputeEllispoidRaycast(const FTransform& Transform, float Radius, const FVector& Start, const FVector& End, FVector& Impact, FVector& Normal, float& Distance)
{
	FTransform NormalizedTransform = Transform;
	FVector Scale = NormalizedTransform.GetScale3D();
	if (FMath::IsNearlyZero(Scale.X)) Scale.X = 0.01f;
	if (FMath::IsNearlyZero(Scale.Y)) Scale.Y = 0.01f;
	if (FMath::IsNearlyZero(Scale.Z)) Scale.Z = 0.01f;
	NormalizedTransform.SetScale3D(Scale);

	const FVector RayStart = NormalizedTransform.InverseTransformPosition(Start);
	const FVector RayEnd = NormalizedTransform.InverseTransformPosition(End);
	const FVector RayDelta = RayEnd - RayStart;
	const float RayLength = RayDelta.Size();

	if (FMath::IsNearlyZero(RayLength))
	{
		Normal = FVector::ZeroVector;
		Impact = End;
		Distance = (NormalizedTransform.GetLocation() - Impact).Size();
		return 1.f;
	}

	const FVector RayDir = RayDelta / RayLength;

	const float Sq = RayStart.SizeSquared();
	const float Rd = Sq - Radius * Radius;

	float Value = -(RayDir | RayStart);

	const float Discr = FMath::Square(Value) - Rd;
	if (Discr >= 0.0f)
	{
		Value -= FMath::Sqrt(Discr);
	}

	// Cast location on the sphere, restrict by raylength
	const float Time = Value / RayLength;
	const FVector RaySpherePoint = RayStart + RayDir * FMath::Clamp(Value, 0.0f, RayLength);
	const float RaySphereDistance = RaySpherePoint.Size();

	Normal = RaySpherePoint / RaySphereDistance;
	Impact = Normal * Radius;

	// Transform back to world
	Normal = NormalizedTransform.TransformVectorNoScale((Normal / NormalizedTransform.GetScale3D())).GetSafeNormal();
	Impact = NormalizedTransform.TransformPosition(Impact);

	Distance = RaySphereDistance - Radius;
	return Time;
}

FRigUnit_EllipsoidRaycast_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	const URigHierarchy* Hierarchy = Context.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		EllipsoidCache.Reset();
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		if (!EllipsoidCache.UpdateCache(Ellipsoid.Key, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("key '%s' is not valid."), *Ellipsoid.Key.ToString());
		}
		else
		{
			const FTransform Transform = Hierarchy->GetGlobalTransform(EllipsoidCache);

			Time = ComputeEllispoidRaycast(Transform, Ellipsoid.Radius, Start, End, Impact, Normal, Distance);
			Location = Distance < 0.01f ? Impact : End;
			Time = Distance < 0.01f ? Time : 1.0f;

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, Impact, Impact + Normal * 25.0f, FLinearColor::Red, DebugSettings.Scale * 0.1f);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FRigUnit_EllipsoidRaycastMulti_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
		const URigHierarchy* Hierarchy = Context.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		EllipsoidCaches.Reset(Ellipsoids.Num());
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		const int32 EllipsoidNum = Ellipsoids.Num();
		if (EllipsoidNum != EllipsoidCaches.Num())
		{
			EllipsoidCaches.SetNumZeroed(EllipsoidNum);
		}

		Time = 1.0f;
		Impact = End;
		Normal = FVector::UpVector;

		for (int32 Index = 0; Index < EllipsoidNum; Index++)
		{
			FCachedRigElement& EllipsoidCache = EllipsoidCaches[Index];
			const FEllipsoid& Ellipsoid = Ellipsoids[Index];

			if (!EllipsoidCache.UpdateCache(Ellipsoid.Key, Hierarchy))
			{
				UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("key '%s' is not valid."), *Ellipsoid.Key.ToString());
			}
			else
			{
				const FTransform Transform = Hierarchy->GetGlobalTransform(EllipsoidCache);

				float CurrDistance;
				FVector CurrImpact, CurrNormal;
				const float CurrTime = FRigUnit_EllipsoidRaycast::ComputeEllispoidRaycast(Transform, Ellipsoid.Radius, Start, End, CurrImpact, CurrNormal, CurrDistance);
				if (CurrDistance < 0.01f && CurrTime < Time)
				{
					Time = CurrTime;
					Impact = CurrImpact;
					Normal = CurrNormal;
				}

				if (DebugSettings.bEnabled)
				{
					Context.DrawInterface->DrawLine(FTransform::Identity, CurrImpact, CurrImpact + CurrNormal * 25.0f, FLinearColor::Red, DebugSettings.Scale * 0.1f);
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FRigUnit_EllipsoidRingCast_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		WorkData.EllipsoidCaches.Reset(Ellipsoids.Num());
		WorkData.ItemCaches.Reset(Items.Num());
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		const int32 EllipsoidNum = Ellipsoids.Num();
		if (EllipsoidNum != WorkData.EllipsoidCaches.Num())
		{
			WorkData.EllipsoidCaches.SetNumZeroed(EllipsoidNum);
		}

		const int32 ItemNum = Items.Num();
		if (ItemNum != WorkData.ItemCaches.Num())
		{
			WorkData.ItemCaches.SetNumZeroed(ItemNum);
			for (int32 ItemIndex = 0; ItemIndex < ItemNum; ItemIndex++)
			{
				FRigUnit_EllipsoidRingCastItem_WorkData& ItemCache = WorkData.ItemCaches[ItemIndex];
				UKismetMathLibrary::ResetFloatSpringState(ItemCache.SpringState);
				ItemCache.bIsInitialised = false;
				ItemCache.CurrentTime = 1.f;
			}
		}

		for (int32 ItemIndex = 0; ItemIndex < ItemNum; ItemIndex++)
		{
			const FRigElementKey& Item = Items[ItemIndex];
			FRigUnit_EllipsoidRingCastItem_WorkData& ItemCache = WorkData.ItemCaches[ItemIndex];
			if (!ItemCache.Cache.UpdateCache(Item, Hierarchy))
			{
				UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("key '%s' is not valid."), *Item.ToString());
			}
			else
			{
				ItemCache.Transform = Hierarchy->GetGlobalTransform(ItemCache.Cache);
				const FVector End = ItemCache.Transform.GetLocation();
				ItemCache.Delta = ItemCache.Transform.TransformVectorNoScale(CastAxis) * CastDistance;

				ItemCache.TargetTime = 1.0f;
				for (int32 EllipsoidIndex = 0; EllipsoidIndex < EllipsoidNum; EllipsoidIndex++)
				{
					FCachedRigElement& EllipsoidCache = WorkData.EllipsoidCaches[EllipsoidIndex];
					const FEllipsoid& Ellipsoid = Ellipsoids[EllipsoidIndex];

					if (!EllipsoidCache.UpdateCache(Ellipsoid.Key, Hierarchy))
					{
						UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("key '%s' is not valid."), *Ellipsoid.Key.ToString());
					}
					else
					{
						const FTransform Transform = Hierarchy->GetGlobalTransform(EllipsoidCache);

						float CurrDistance;
						FVector CurrImpact, CurrNormal;
						const float CurrTime = FRigUnit_EllipsoidRaycast::ComputeEllispoidRaycast(Transform, Ellipsoid.Radius, End + ItemCache.Delta, End, CurrImpact, CurrNormal, CurrDistance);
						if (CurrDistance < 0.01f && CurrTime < ItemCache.TargetTime)
						{
							ItemCache.TargetTime = CurrTime;
						}
					}
				}

				if (DebugSettings.bEnabled)
				{
					Context.DrawInterface->DrawLine(FTransform::Identity, End + ItemCache.Delta, End, FLinearColor::Red, DebugSettings.Scale * 0.1f);
					Context.DrawInterface->DrawPoint(FTransform::Identity, FMath::Lerp(End + ItemCache.Delta, End, ItemCache.TargetTime), DebugSettings.Scale * 5.0f, FLinearColor::Green);
				}
			}
		}

		for (int32 ItemIndex = 0; ItemIndex < ItemNum; ItemIndex++)
		{
			FRigUnit_EllipsoidRingCastItem_WorkData& ItemCache = WorkData.ItemCaches[ItemIndex];
			if (ItemCache.Cache.IsValid())
			{
				float ConstrainedTime = 1.f;

				const float ItemRatio = ((float)ItemIndex) / ItemNum;
				for (int32 OtherIndex = 0; OtherIndex < ItemNum; OtherIndex++)
				{
					const FRigUnit_EllipsoidRingCastItem_WorkData& OtherCache = WorkData.ItemCaches[OtherIndex];
					if (OtherCache.Cache.IsValid())
					{
						const float OtherRatio = ((float)OtherIndex) / ItemNum;

						const float Value =
							FMath::Exp(FMath::Square(OtherRatio - (ItemRatio - 1.0f)) / -SpreadVariance) +
							FMath::Exp(FMath::Square(OtherRatio - (ItemRatio + 0.0f)) / -SpreadVariance) +
							FMath::Exp(FMath::Square(OtherRatio - (ItemRatio + 1.0f)) / -SpreadVariance);

						ConstrainedTime = FMath::Min(ConstrainedTime, FMath::Lerp(1.0f, OtherCache.TargetTime, FMath::Clamp(Value, 0.f, 1.f)));
					}
				}

				/*if (ConstrainedTime < ItemCache.CurrentTime)
				{
					ItemCache.CurrentTime = ConstrainedTime;
					ItemCache.SpringState.Reset();
				}*/

				ItemCache.CurrentTime = UKismetMathLibrary::FloatSpringInterp(
					ItemCache.CurrentTime, ConstrainedTime, ItemCache.SpringState,
					SpringStrength, SpringDamping, Context.DeltaTime, 1.f, 1.f,
					false, -1, 1, ItemCache.bIsInitialised);
				ItemCache.bIsInitialised = true;

				ItemCache.Transform.AddToTranslation((1.0f - ItemCache.CurrentTime) * ItemCache.Delta);
				Hierarchy->SetGlobalTransform(ItemCache.Cache.GetKey(), ItemCache.Transform, false, true);
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////

FVector FRigUnit_EllipsoidLineCollide::ComputeEllispoidLineCollide(const FTransform& Transform, float Radius, const FVector& Start, const FVector& End, const FVector& Direction, float Adapt)
{
	const FVector RayStart = Transform.InverseTransformPosition(Start);
	const FVector RayEnd = Transform.InverseTransformPosition(End);
	const FVector RayDelta = RayEnd - RayStart;
	const float RayLength = RayDelta.Size();
	const FVector RayDir = RayDelta / RayLength;

	const FVector RayMove = Transform.InverseTransformVector(Direction);
	const FVector RayAxis = (RayMove ^ RayDir).GetSafeNormal();

	const float Ad = RayAxis | RayStart; // Distance between circle center to sphere
	const float Rq = FMath::Square(Radius) - FMath::Square(Ad); // Pythagoras to compute square circle radius

	// Check if we're in reach
	if (!FMath::IsNearlyZero(Adapt) || Rq > 0.0f)
	{
		const FVector PC = RayAxis * Ad; // Circle center

		// Check if where moving through the sphere at all
		const FVector CenterDelta = RayStart - RayDir * (RayDir | RayStart) - PC;
		if (CenterDelta.SizeSquared() < Rq || (CenterDelta | RayMove) > 0.0f)
		{
			const FVector SD = PC - RayStart;
			const float Sd = SD.Size(); // Distance between line start and circle center

			const float Pq = FMath::Max(Rq, 0.0f);
			const float Dq = FMath::Square(Sd) - Pq; // Square distance from line start to tangent point
			if (Dq > 0.0f)
			{
				const FVector SN = SD / Sd;
				const FVector HN = FVector::VectorPlaneProject(-RayMove, SN).GetSafeNormal();

				const float Height = FMath::Sqrt(Dq * Pq) / Sd; // Triangle height (area / hypoth)
				const float Kath = FMath::Sqrt(Dq - FMath::Square(Height));
				const FVector Tangent = HN * Height + SN * Kath; // Tangent

				const FVector TangentPoint = Transform.TransformPosition(RayStart + Tangent);
				const FVector WorldDelta = End - Start;
				const float WorldLength = WorldDelta.Size();
				const FVector TangentDelta = TangentPoint - Start;

				const float Intensity = FMath::Clamp(-Rq / (Adapt * Adapt), 0.0f, 1.0f);
				const FVector Delta = FMath::Lerp(TangentDelta, WorldDelta, Intensity);
				return Start + Delta.GetSafeNormal() * WorldLength;
			}
		}
	}

	// Don't move by default
	return End;
}

FRigUnit_EllipsoidLineCollide_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	const URigHierarchy* Hierarchy = Context.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		EllipsoidCache.Reset();
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		if (!EllipsoidCache.UpdateCache(Ellipsoid.Key, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("key '%s' is not valid."), *Ellipsoid.Key.ToString());
		}
		else
		{
			const FTransform Transform = Hierarchy->GetGlobalTransform(EllipsoidCache);
			Deflect = ComputeEllispoidLineCollide(Transform, Ellipsoid.Radius, Start, End, Direction, Adapt);

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawPoint(FTransform::Identity, Deflect, DebugSettings.Scale * 5.0f, FLinearColor::White);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FRigUnit_EllipsoidChainCollide_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		EllipsoidCache.Reset();
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		const int32 ChainNum = Chain.Num();
		if (ChainNum < 2)
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Chain has to have length at least 2."));
		}
		else if (!EllipsoidCache.UpdateCache(Ellipsoid.Key, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("key '%s' is not valid."), *Ellipsoid.Key.ToString());
		}
		else
		{
			// Apply rotation along the whole chain
			FTransform Transform = Hierarchy->GetGlobalTransform(Chain[0]);
			const FTransform EllipsoidTransform = Hierarchy->GetGlobalTransform(EllipsoidCache);

			const float MaxChainLength = FRigUnit_ChainAnalysis::ComputeInitialChainLength(Chain, Hierarchy);

			// Intensity according to relative distance
			FVector Anchor, AnchorNormal;
			FRigUnit_EllipsoidProjection::ComputeEllispoidProjection(EllipsoidTransform, Ellipsoid.Radius, Transform.GetLocation(), Anchor, AnchorNormal);

			// Diminish intensity with distance 
			const float Distance = (Transform.GetLocation() - Anchor).Size();// | AnchorNormal;
			const float Intensity = FMath::Clamp(1.0f + DiscoveryRatio - Distance / MaxChainLength, 0.0f, 1.0f);

			// Rotate each segment
			const float MaxRadians = FMath::DegreesToRadians(MaxAngle);
			const float RotationRadians = FMath::DegreesToRadians(RotationAngle);
			const FVector WorldAxis = Transform.TransformVectorNoScale(RotationAxis);
			const FQuat Rotation = FQuat(WorldAxis, RotationRadians * (1.0f - Intensity));

			for (int32 Index = 1; Index < ChainNum; Index++)
			{
				const FTransform Local = Hierarchy->GetLocalTransform(Chain[Index]);

				// Collide future bone location with ellipsoid
				const FVector Start = Transform.GetLocation();
				const FVector Delta = Rotation * (Transform.TransformVector(Local.GetLocation()));
				const float DeltaSize = Delta.Size();
				const FVector DeltaNormal = Delta / DeltaSize;

				if (!FMath::IsNearlyZero(Intensity))
				{
					FVector Closest, Normal;
					const FVector Point = Start + Delta * CollisionPointRatio;
					FRigUnit_EllipsoidProjection::ComputeEllispoidProjection(EllipsoidTransform, Ellipsoid.Radius, Point, Closest, Normal);

					const FVector FinalPlane = FVector::VectorPlaneProject(FVector::VectorPlaneProject(DeltaNormal, Normal), WorldAxis);
					const FVector FinalDelta = FMath::Lerp(DeltaNormal, FinalPlane, Intensity).GetSafeNormal() * DeltaSize;

					// Rotate to match
					const FVector CurrentDelta = Transform.TransformVectorNoScale(Local.GetLocation());
					const FQuat AlignRotation = FQuat::FindBetweenVectors(CurrentDelta, FinalDelta);
					
					const FQuat FinalRotation = FRigUnit_LimitRotationAroundAxis::AxisLimitRotation(AlignRotation, WorldAxis, MaxRadians, RotationRadians);
					Transform.SetRotation(FinalRotation * Transform.GetRotation());

					// Update chain element
					Hierarchy->SetGlobalTransform(Chain[Index - 1], Transform, false, PropagateToChildren == EPropagation::All);

					// Propagate
					Transform = Local * Transform;

					if (DebugSettings.bEnabled)
					{
						Context.DrawInterface->DrawPoint(FTransform::Identity, Point, DebugSettings.Scale * 2.0f, FLinearColor::White);
						Context.DrawInterface->DrawPoint(FTransform::Identity, Closest, DebugSettings.Scale * 2.0f, FLinearColor::Black);
					}
				}
				else
				{
					FRigUnit_BendTowards::BendTowards(Chain[Index - 1], Chain[Index], Start + Delta, Hierarchy, ScaleType, PropagateToChildren == EPropagation::All);
					Transform = Hierarchy->GetGlobalTransform(Chain[Index]);
				}


				if (DebugSettings.bEnabled)
				{
					Context.DrawInterface->DrawLine(FTransform::Identity, Start - WorldAxis * 5.0f, Start + WorldAxis * 5.0f, FLinearColor::Red, DebugSettings.Scale * 0.1f);
					Context.DrawInterface->DrawPoint(FTransform::Identity, Start, DebugSettings.Scale * 5.0f, FLinearColor::Blue);
				}
			}


			Hierarchy->SetGlobalTransform(Chain.Last(), Transform, false, PropagateToChildren != EPropagation::Off);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FRigUnit_EllipsoidTransformProject_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	const URigHierarchy* Hierarchy = Context.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		EllipsoidCache.Reset();
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		if (!EllipsoidCache.UpdateCache(Ellipsoid.Key, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("key '%s' is not valid."), *Ellipsoid.Key.ToString());
		}
		else
		{
			// Objective properties
			const FTransform EndEETarget = GET_IK_OBJECTIVE_TRANSFORM();
			const FVector EEForwardTarget = GET_IK_OBJECTIVE_FORWARD();
			const FVector EEUpTarget = GET_IK_OBJECTIVE_UP();

			const FVector EELocation = EndEETarget.GetLocation();

			const FTransform EllipsoidTransform = Hierarchy->GetGlobalTransform(EllipsoidCache);
			const FVector RayStart = EELocation + EEUpTarget * Discovery;

			// Get closest point on the ellipsoid to the ray
			float HitDistance;
			FVector Closest, Normal;
			FRigUnit_EllipsoidRaycast::ComputeEllispoidRaycast(EllipsoidTransform, Ellipsoid.Radius, RayStart, EELocation, Closest, Normal, HitDistance);
			if (!FMath::IsNearlyZero(HitDistance))
			{
				const FVector RayDelta = EELocation - RayStart;
				const float RayLength = RayDelta.Size();
				const FVector RayNormal = RayDelta / RayLength;
				const FVector Point = RayStart + RayNormal * FMath::Clamp((Closest - RayStart) | RayNormal, 0.0f, RayLength);
				FRigUnit_EllipsoidProjection::ComputeEllispoidProjection(EllipsoidTransform, Ellipsoid.Radius, Point, Closest, Normal);

				HitDistance = (Point - Closest).Size();

				if (DebugSettings.bEnabled)
				{
					Context.DrawInterface->DrawPoint(FTransform::Identity, Point, DebugSettings.Scale * 5.0f, FLinearColor::Blue);
				}
			}

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawLine(FTransform::Identity, RayStart, EELocation, FLinearColor::White, DebugSettings.Scale * 0.1f);
				Context.DrawInterface->DrawLine(FTransform::Identity, Closest, Closest + Normal * 10.0f, FLinearColor::Green, DebugSettings.Scale * 0.1f);
				Context.DrawInterface->DrawPoint(FTransform::Identity, Closest, DebugSettings.Scale * 5.0f, FLinearColor::Blue);
			}

			const float Intensity = FMath::Square(FMath::Max(1.0f - HitDistance / Distance, 0.0f));
			const FVector UpTarget = FMath::Lerp(EEUpTarget, Normal, Intensity).GetSafeNormal();
			const FVector LocationTarget = FMath::Lerp(EELocation, Closest, Intensity);
			
			const float MaxRadians = FMath::DegreesToRadians(MaxAngle);
			const FQuat AlignRotation = FQuat::FindBetweenNormals(EEUpTarget, UpTarget);
			const FQuat LimitRotation = FRigUnit_LimitRotation::LimitRotation(AlignRotation, MaxRadians, true);


			Projection.SetRotation(LimitRotation * EndEETarget.GetRotation());
			Projection.SetLocation(LocationTarget);
			Projection.SetScale3D(Objective.GetScale3D());

			if (DebugSettings.bEnabled)
			{
				Context.DrawInterface->DrawPoint(FTransform::Identity, LocationTarget, DebugSettings.Scale * 5.0f, FLinearColor::Red);
			}
		}
	}
}
