// The Gateway of Realities: Planes of Existence.

#pragma once
#include "Units/RigUnit.h"
#include "Utility.generated.h"

UENUM(BlueprintType)
enum class EPropagation : uint8
{
	/** Don't propagate to children */
	Off,
	/** Only propagate to children of last node (fast) */
	OnlyLast,
	/** Propagate to all children (slowest) */
	All
};


USTRUCT()
struct FDebugSettings
{
	GENERATED_BODY()

	FDebugSettings()
	{
		bEnabled = false;
		Scale = 1.f;
	}

	/**
	 * If enabled debug information will be drawn
	 */
	UPROPERTY(EditAnywhere, meta = (Input), Category = "DebugSettings")
		bool bEnabled;

	/**
	 * The size of the debug drawing information
	 */
	UPROPERTY(EditAnywhere, meta = (Input, EditCondition = "bEnabled"), Category = "DebugSettings")
		float Scale;
};
