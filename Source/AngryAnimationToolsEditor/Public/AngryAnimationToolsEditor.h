

#pragma once

#include "CoreMinimal.h"
#include "SequencerCustomizationManager.h"
#include "Modules/ModuleManager.h"

class FAngryAnimationToolsEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void OnAssetOpening(const TArray<UObject*>& Objects, IAssetEditorInstance* Editor);

	void UpdateSequencer(TSharedRef<ISequencer> Sequencer);
	void FillSequenceEditorToolbar(FToolBarBuilder& ToolbarBuilder);
	void RepairSequence();

	TWeakPtr<ISequencer> GetSequencer() const { return CurrentSequencer; };

private:
	void ExtendSequencerActions(FMenuBuilder& MenuBuilder);
	TSharedRef<class SWidget> MakeModifiersMenu();
	void BindModifierCommands();

private:
	TSharedPtr<FExtender> ToolbarExtender;
	TArray<UAnimSequence*> OpenedAnims;

	FDelegateHandle OnSequencerCreatedHandle;
	TWeakPtr<ISequencer> CurrentSequencer;

	TSharedPtr<FUICommandList> ModifierCommandBindings;
	TSharedPtr<FExtender> ActionsMenuExtender;
};
