
#include "Instances/CharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

UCharacterAnimInstance::UCharacterAnimInstance(class FObjectInitializer const& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	AActor* Owner = GetOwningActor();
	if (ensure(IsValid(Owner)))
	{
		CharacterMovement = Owner->FindComponentByClass<UCharacterMovementComponent>();
	}

	USkeletalMeshComponent* OwningComponent = GetOwningComponent();
	if (ensure(IsValid(OwningComponent)))
	{
		CurrentWorldQuat = OwningComponent->GetComponentQuat();
	}
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	USkeletalMeshComponent* OwningComponent = GetOwningComponent();
	if (!IsValid(CharacterMovement) || !IsValid(OwningComponent))
	{
		Super::NativeUpdateAnimation(DeltaSeconds);
		return;
	}

	bIsCrouching = CharacterMovement->IsCrouching();
	bIsFalling = CharacterMovement->IsFalling();

	// Measure angular velocity using a spring proxy
	const FQuat TargetWorldQuat = OwningComponent->GetComponentQuat();
	CurrentWorldQuat = UKismetMathLibrary::QuaternionSpringInterp(CurrentWorldQuat, TargetWorldQuat, WorldQuatSpringState, AngularStiffness, 1.0f, DeltaSeconds, 1.f, 1.f, true);


	// Convert relative to movement base and owning skeletal mesh
	const FTransform OwningTransform = OwningComponent->GetComponentTransform();
	UPrimitiveComponent* MovementBase = CharacterMovement->GetMovementBase();
	if(false)//if (IsValid(MovementBase))
	{
		const FVector RelativeLinearVelocity = MovementBase->ComponentVelocity - CharacterMovement->Velocity;
		LinearVelocity = OwningTransform.InverseTransformVector(RelativeLinearVelocity);

		const FVector RelativeAngularVelocity = MovementBase->GetPhysicsAngularVelocityInRadians() - WorldQuatSpringState.AngularVelocity;
		AngularVelocity = OwningTransform.InverseTransformVector(RelativeAngularVelocity);
	}
	else
	{
		LinearVelocity = OwningTransform.InverseTransformVector(CharacterMovement->Velocity);
		AngularVelocity = OwningTransform.InverseTransformVector(WorldQuatSpringState.AngularVelocity);
	}

	// Measure acceleration using a spring proxy
	CurrentWorldLinearVelocity = UKismetMathLibrary::VectorSpringInterp(CurrentWorldLinearVelocity, CharacterMovement->Velocity, WorldLinearSpringState, AngularStiffness, 1.0f, DeltaSeconds, 1.f, 1.f, true);
	CurrentWorldAngularVelocity = UKismetMathLibrary::VectorSpringInterp(CurrentWorldAngularVelocity, WorldQuatSpringState.AngularVelocity, WorldAngularSpringState, AngularStiffness, 1.0f, DeltaSeconds, 1.f, 1.f, true);

	LinearAcceleration = OwningTransform.InverseTransformVector(WorldLinearSpringState.Velocity);
	AngularAcceleration = OwningTransform.InverseTransformVector(WorldAngularSpringState.Velocity);

	Super::NativeUpdateAnimation(DeltaSeconds);
}
