// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "EditorStyleSet.h"

class FModifierCommands : public TCommands<FModifierCommands>
{

public:
	FModifierCommands() : TCommands<FModifierCommands>
		(
			"Modifiers",
			NSLOCTEXT("Contexts", "Modifiers", "Modifiers"),
			NAME_None,
			FAppStyle::GetAppStyleSetName()
		)
	{}

	/** Mirror selected track */
	TSharedPtr<FUICommandInfo> MirrorX;
	TSharedPtr<FUICommandInfo> Loop;
	TSharedPtr<FUICommandInfo> TimeOffsetHalf;

	TSharedPtr<FUICommandInfo> MirrorLeftToRight;
	TSharedPtr<FUICommandInfo> FlipLeftToRight;

	TSharedPtr<FUICommandInfo> MirrorRightToLeft;
	TSharedPtr<FUICommandInfo> FlipRightToLeft;

	/**
	 * Initialize commands
	 */
	virtual void RegisterCommands() override;
};
