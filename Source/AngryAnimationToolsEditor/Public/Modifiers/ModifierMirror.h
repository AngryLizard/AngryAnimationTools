// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"

class ISequencer;

struct FModifierMirror
{
	TWeakPtr<ISequencer> GetSequencer() const;

	void Mirror();
	void Loop();
	void TimeOffsetHalf();

	void FlipLeftToRight(bool bWithOffet);
	void FlipRightToLeft(bool bWithOffet);
};


