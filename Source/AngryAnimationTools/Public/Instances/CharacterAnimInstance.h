// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

class UCharacterMovementComponent;

UCLASS(Transient, Blueprintable)
class ANGRYANIMATIONTOOLS_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_UCLASS_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	/**
	* We measure angular velocity using a spring proxy.
	* Stiffness changes how snappy said velocity follows the target.
	* The spring is always critically damped.
	*/
	UPROPERTY(EditDefaultsOnly)
		float AngularStiffness = 125.f;

	/**
	* We measure acceleration using a spring proxy.
	* Stiffness changes how snappy said accleration follows the target.
	* The spring is always critically damped.
	*/
	UPROPERTY(EditDefaultsOnly)
		float LinearAccelerationStiffness = 125.f;

	/**
	* We measure acceleration using a spring proxy.
	* Stiffness changes how snappy said accleration follows the target.
	* The spring is always critically damped.
	*/
	UPROPERTY(EditDefaultsOnly)
		float AngularAccelerationStiffness = 125.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		bool bIsCrouching = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		bool bIsFalling = false;

	/**
	* Linear velocity relative to movement base and owning skeletal mesh
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FVector LinearVelocity = FVector::ZeroVector;

	/**
	* Angular velocity relative to movement base and owning skeletal mesh
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FVector AngularVelocity = FVector::ZeroVector;

	/**
	* Linear acceleration relative to movement base and owning skeletal mesh
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FVector LinearAcceleration = FVector::ZeroVector;

	/**
	* Angular acceleration relative to movement base and owning skeletal mesh
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FVector AngularAcceleration = FVector::ZeroVector;

	/**
	* Currently attached character movement component
	*/
	UPROPERTY(BlueprintReadOnly)
		UCharacterMovementComponent* CharacterMovement = nullptr;

private:

	FQuat CurrentWorldQuat = FQuat::Identity;
	FQuaternionSpringState WorldQuatSpringState;

	FVector CurrentWorldLinearVelocity = FVector::ZeroVector;
	FVectorSpringState WorldLinearSpringState;

	FVector CurrentWorldAngularVelocity = FVector::ZeroVector;
	FVectorSpringState WorldAngularSpringState;
};
