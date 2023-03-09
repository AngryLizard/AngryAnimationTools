

#pragma once

#include "CoreMinimal.h"
#include "SequencerCustomizationManager.h"
#include "Modules/ModuleManager.h"

class FAngrySequencerCustomization : public ISequencerCustomization
{
public:
	FAngrySequencerCustomization();

	virtual void RegisterSequencerCustomization(FSequencerCustomizationBuilder& Builder) override;
	virtual void UnregisterSequencerCustomization() override;

private:
	void ExtendSequencerToolbar(FToolBarBuilder& ToolbarBuilder);
	TSharedRef<class SWidget> MakeModifiersMenu();
	void BindCommands();

	ISequencer* Sequencer;
	TSharedRef<FUICommandList> ModifierCommandBindings;
};

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
	TSharedPtr<FExtender> ToolbarExtender;
	TArray<UAnimSequence*> OpenedAnims;

	FDelegateHandle OnSequencerCreatedHandle;
	TWeakPtr<ISequencer> CurrentSequencer;
};
