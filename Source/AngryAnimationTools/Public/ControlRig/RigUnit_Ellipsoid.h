// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Units/RigUnit.h"
#include "IK/RigUnit_IK.h"
#include "ControlRig/Utility.h"
#include "Kismet/KismetMathLibrary.h"

#include "Animation/InputScaleBias.h"
#include "RigUnit_Ellipsoid.generated.h"


USTRUCT(BlueprintType)
struct FEllipsoid
{
	GENERATED_BODY()

	void Reset(FCachedRigElement& Cache) const;
	bool Update(const URigHierarchy* Hierarchy, FCachedRigElement& Cache) const;

	/**
	* Radius of unscaled sphere.
	*/
	UPROPERTY(meta = (Input))
		float Radius = 100.0f;

	/**
	 * Ellipsoid control to use for collision. Control should preferably be shaped like a sphere with radius defined above.
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey Key = FRigElementKey(FName(), ERigElementType::Control);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Get closest point on an ellipsoid
 */
USTRUCT(meta = (DisplayName = "Ellipsoid Projection", Category = "Ellipsoid", Keywords = "Ellipsoid", PrototypeName = "EllipsoidProjection", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_EllipsoidProjection : public FRigUnit
{
	GENERATED_BODY()

		FRigUnit_EllipsoidProjection() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:
	static void ComputeEllispoidProjection(const FTransform& Transform, float Radius, const FVector& Point, FVector& Closest, FVector& Normal);

	/**
	* Ellipsoid used for collision
	*/
	UPROPERTY(meta = (Input))
		FEllipsoid Ellipsoid;

	/**
	* Projection point
	*/
	UPROPERTY(meta = (Input))
		FVector Point = FVector::ZeroVector;

	/**
	* Closest point
	*/
	UPROPERTY(meta = (Output))
		FVector Closest = FVector::ZeroVector;

	/**
	* Impact normal
	*/
	UPROPERTY(meta = (Output))
		FVector Normal = FVector::ForwardVector;

	/**
	 * Debug settings
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FDebugSettings DebugSettings;

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement EllipsoidCache;
};


////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Computes the distance between a point and plane local to the ellipsoid.
 * Can be used to determine e.g. the relative distance along the ellipsoid
 */
USTRUCT(meta = (DisplayName = "Ellipsoid Point Plane Project", Category = "Ellipsoid", Keywords = "Ellipsoid", PrototypeName = "EllipsoidProjection", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_EllipsoidPointPlaneProject : public FRigUnit
{
	GENERATED_BODY()

		FRigUnit_EllipsoidPointPlaneProject() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:
	static float ComputeEllispoidPointPlaneProject(const FTransform& Transform, float Radius, const FVector& Point, const FVector& Normal, FVector& Projected);

	/**
	* Ellipsoid used for the dot product
	*/
	UPROPERTY(meta = (Input))
		FEllipsoid Ellipsoid;

	/**
	* Point in global space for plane projectyion
	*/
	UPROPERTY(meta = (Input))
		FVector Point = FVector::ZeroVector;

	/**
	* Normal of the plane, does not need to be normalized
	*/
	UPROPERTY(meta = (Input))
		FVector Normal = FVector::ZeroVector;

	/**
	* Distance to the plane, negative if behind the plane
	*/
	UPROPERTY(meta = (Output))
		float Distance = 0.f;

	/**
	* Projected point
	*/
	UPROPERTY(meta = (Output))
		FVector Projected = FVector::ZeroVector;

	/**
	 * Debug settings
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FDebugSettings DebugSettings;

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement EllipsoidCache;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Intersect a line segment with an ellipsoid
 */
USTRUCT(meta = (DisplayName = "Ellipsoid Raycast", Category = "Ellipsoid", Keywords = "Ellipsoid", PrototypeName = "EllipsoidRaycast", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_EllipsoidRaycast : public FRigUnit
{
	GENERATED_BODY()

		FRigUnit_EllipsoidRaycast() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:
	static float ComputeEllispoidRaycast(const FTransform& Transform, float Radius, const FVector& Start, const FVector& End, FVector& Impact, FVector& Normal, float& Distance);

	/**
	* Ellipsoids used for collision
	*/
	UPROPERTY(meta = (Input))
		FEllipsoid Ellipsoid;

	/**
	* Raycast start point
	*/
	UPROPERTY(meta = (Input))
		FVector Start = FVector::ZeroVector;

	/**
	* Raycast end point
	*/
	UPROPERTY(meta = (Input))
		FVector End = FVector::ZeroVector;

	/**
	* Distance ratio along the ray of the intersection, 0 for start an 1 for end.
	*/
	UPROPERTY(meta = (Output))
		float Time = 0.0f;

	/**
	* Distance of he ray to the sphere if not intersecting.
	* Not to be confused with the intersection distance on the ray use Time for that.
	*/
	UPROPERTY(meta = (Output))
		float Distance = 0.0f;

	/**
	* Impact point
	* Closest point on the ellipsoid if there is no impact
	*/
	UPROPERTY(meta = (Output))
		FVector Impact = FVector::ZeroVector;

	/**
	* Impact normal
	*/
	UPROPERTY(meta = (Output))
		FVector Normal = FVector::ForwardVector;

	/**
	* Raycast location
	* End of the raycast if there is no impact
	*/
	UPROPERTY(meta = (Output))
		FVector Location = FVector::ZeroVector;

	/**
	 * Debug settings
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FDebugSettings DebugSettings;

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement EllipsoidCache;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Intersect a line segment with a collection of ellipsoids
 */
USTRUCT(meta = (DisplayName = "Ellipsoid Raycast Multi", Category = "Ellipsoid", Keywords = "Ellipsoid", PrototypeName = "EllipsoidRaycast", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_EllipsoidRaycastMulti : public FRigUnit
{
	GENERATED_BODY()

		FRigUnit_EllipsoidRaycastMulti() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:

	/**
	* Ellipsoids used for collision
	*/
	UPROPERTY(meta = (Input))
		TArray<FEllipsoid> Ellipsoids;

	/**
	* Raycast start point
	*/
	UPROPERTY(meta = (Input))
		FVector Start = FVector::ZeroVector;

	/**
	* Raycast end point
	*/
	UPROPERTY(meta = (Input))
		FVector End = FVector::ZeroVector;

	/**
	* Distance ratio along the ray of the intersection, 0 for start an 1 for end.
	*/
	UPROPERTY(meta = (Output))
		float Time = 0.0f;

	/**
	* Impact point
	* Closest point on the ellipsoid if there is no impact
	*/
	UPROPERTY(meta = (Output))
		FVector Impact = FVector::ZeroVector;

	/**
	* Impact normal
	*/
	UPROPERTY(meta = (Output))
		FVector Normal = FVector::ForwardVector;

	/**
	 * Debug settings
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FDebugSettings DebugSettings;

	// Cache
	UPROPERTY(Transient)
		TArray<FCachedRigElement> EllipsoidCaches;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

USTRUCT()
struct FRigUnit_EllipsoidRingCastItem_WorkData
{
	GENERATED_BODY()

	UPROPERTY()
		FCachedRigElement Cache;

	UPROPERTY()
		FTransform Transform;

	UPROPERTY()
		FVector Delta;

	UPROPERTY()
		float TargetTime;

	UPROPERTY()
		float CurrentTime;

	UPROPERTY()
		bool bIsInitialised;

	UPROPERTY()
		FFloatSpringState SpringState;
};

USTRUCT()
struct FRigUnit_EllipsoidRingCast_WorkData
{
	GENERATED_BODY()

	UPROPERTY()
		TArray<FCachedRigElement> EllipsoidCaches;

	UPROPERTY()
		TArray<FRigUnit_EllipsoidRingCastItem_WorkData> ItemCaches;
};

/**
 * Moves a ring of bones around an ellipsoid
 */
USTRUCT(meta = (DisplayName = "Ellipsoid Ring Cast", Category = "Ellipsoid", Keywords = "Ellipsoid", PrototypeName = "EllipsoidRaycast", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_EllipsoidRingCast : public FRigUnitMutable
{
	GENERATED_BODY()

		FRigUnit_EllipsoidRingCast() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:

	/**
	* Ellipsoids used for collision
	*/
	UPROPERTY(meta = (Input))
		TArray<FEllipsoid> Ellipsoids;

	/**
	 * Bones of the ring to adapt cast to the ellipsoid
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection Items;

	/**
	* Axis for each item to use for raycasting
	*/
	UPROPERTY(meta = (Input))
		FVector CastAxis = FVector(0, -1, 0);

	/**
	* Distance for each item to use for raycasting
	*/
	UPROPERTY(meta = (Input))
		float CastDistance = 10.f;

	/**
	* Variance for how far items will adapt to their neighbours
	*/
	UPROPERTY(meta = (Input))
		float SpreadVariance = 0.5f;

	/**
	* Used spring strength
	*/
	UPROPERTY(meta = (Input))
		float SpringStrength = 3.0f;

	/**
	* Used spring damping
	*/
	UPROPERTY(meta = (Input))
		float SpringDamping = 2.0f;

	/**
	 * Debug settings
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FDebugSettings DebugSettings;

	// Cache
	UPROPERTY(Transient)
		FRigUnit_EllipsoidRingCast_WorkData WorkData;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Collides a line segment with the ellispoid in a given direction. The sweep is done rotationally around the line start as pivot, imagine a stick on a hinge.
 */
USTRUCT(meta = (DisplayName = "Ellipsoid Line Collision", Category = "Ellipsoid", Keywords = "Ellipsoid", PrototypeName = "EllipsoidLineCollision", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_EllipsoidLineCollide : public FRigUnit
{
	GENERATED_BODY()

		FRigUnit_EllipsoidLineCollide() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:
	static FVector ComputeEllispoidLineCollide(const FTransform& Transform, float Radius, const FVector& Start, const FVector& End, const FVector& Direction, float Adapt);

	/**
	* Ellipsoid used for collision
	*/
	UPROPERTY(meta = (Input))
		FEllipsoid Ellipsoid;

	/**
	* Line start
	*/
	UPROPERTY(meta = (Input))
		FVector Start = FVector::ZeroVector;

	/**
	* Line end
	*/
	UPROPERTY(meta = (Input))
		FVector End = FVector::ZeroVector;

	/**
	* Sweep direction
	*/
	UPROPERTY(meta = (Input))
		FVector Direction = FVector::ForwardVector;

	/**
	* Distance in which we smoothly lerp if outside collision radius instead of snapping
	*/
	UPROPERTY(meta = (Input, DetailsOnly))
		float Adapt = 50.0f;

	/**
	* Deflected point
	*/
	UPROPERTY(meta = (Output))
		FVector Deflect = FVector::ZeroVector;

	/**
	 * Debug settings
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FDebugSettings DebugSettings;

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement EllipsoidCache;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Ellipsoid collision for a rotating chain. This uses projection between points along each chain bone and the ellipsoid.
 */
USTRUCT(meta = (DisplayName = "Ellipsoid Chain Collision", Category = "Ellipsoid", Keywords = "Ellipsoid", PrototypeName = "EllipsoidChainCollision", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_EllipsoidChainCollide : public FRigUnitMutable
{
	GENERATED_BODY()

		FRigUnit_EllipsoidChainCollide() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:

	/**
	* Ellipsoid used for collision
	*/
	UPROPERTY(meta = (Input))
		FEllipsoid Ellipsoid;

	/**
	 * The chain to adapt (Has to be continuous chain)
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKeyCollection Chain;

	/**
	 * Ellipsoid control to use for collision. Needs to be a sphere with radius 100
	 */
	UPROPERTY(meta = (Input, ExpandByDefault))
		FRigElementKey Key = FRigElementKey(FName(), ERigElementType::Control);

	/**
	* Point (ratio) along each segment that should be used to determine collision distance to the ellipsoid.
	*/
	UPROPERTY(meta = (Input, DetailsOnly))
		float CollisionPointRatio = 0.5f;

	/**
	* Distance ratio relative to total chain length at which chain will start to adapt even if there is no collision yet
	*/
	UPROPERTY(meta = (Input, DetailsOnly))
		float DiscoveryRatio = 0.25f;

	/**
	* Max angle against rotation angle
	*/
	UPROPERTY(meta = (Input))
		float MaxAngle = 20.0f;

	/**
	* Rotation axis for each segment
	*/
	UPROPERTY(meta = (Input, DetailsOnly))
		FVector RotationAxis = FVector::ForwardVector;

	/**
	* Rotation angle for each segment
	*/
	UPROPERTY(meta = (Input))
		float RotationAngle = 0.0f;

	/**
	* How to scale the bones
	 */
	UPROPERTY(meta = (Input))
		EBendScaleType ScaleType = EBendScaleType::None;

	/**
	 * If set to true all of the global transforms of the children
	 * of the chain bones will be recalculated based on their local transforms.
	 */
	UPROPERTY(meta = (Input, Constant))
		EPropagation PropagateToChildren = EPropagation::All;

	/**
	 * Debug settings
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FDebugSettings DebugSettings;

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement EllipsoidCache;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Ellipsoid projections for an objective transform
 */
USTRUCT(meta = (DisplayName = "Ellipsoid Transform Project", Category = "Ellipsoid", Keywords = "Ellipsoid", PrototypeName = "EllipsoidTransformProject", NodeColor = "1.0 0.44 0.0"))
struct ANGRYANIMATIONTOOLS_API FRigUnit_EllipsoidTransformProject : public FRigUnit
{
	GENERATED_BODY()

		FRigUnit_EllipsoidTransformProject() {}

	RIGVM_METHOD()
		virtual void Execute() override;

public:

	/**
	* Ellipsoid used for collision
	*/
	UPROPERTY(meta = (Input))
		FEllipsoid Ellipsoid;

	/**
	 * Local objective offset rotation
	 */
	UPROPERTY(meta = (Input, Constant, DetailsOnly))
		FRotator OffsetRotation;

	/**
	 * Local objective offset translation
	 */
	UPROPERTY(meta = (Input, Constant, DetailsOnly))
		FVector OffsetTranslation;

	/**
	* Target location/rotation for the chain end
	*/
	UPROPERTY(meta = (Input))
		FTransform Objective = FTransform::Identity;

	/**
	* How far away in up direction we poll for ellipsoid collision
	*/
	UPROPERTY(meta = (Input))
		float Discovery = 150.0f;

	/**
	* Max distance from ellipsoid where adaptation happens
	*/
	UPROPERTY(meta = (Input))
		float Distance = 50.0f;

	/**
	* Max angle to rotate hand
	*/
	UPROPERTY(meta = (Input))
		float MaxAngle = 30.0f;

	/**
	* Projected transform
	*/
	UPROPERTY(meta = (Output))
		FTransform Projection = FTransform::Identity;

	/**
	 * Debug settings
	 */
	UPROPERTY(meta = (Input, DetailsOnly))
		FDebugSettings DebugSettings;

	// Cache
	UPROPERTY(Transient)
		FCachedRigElement AnchorCache;

	UPROPERTY(Transient)
		FCachedRigElement EllipsoidCache;
};