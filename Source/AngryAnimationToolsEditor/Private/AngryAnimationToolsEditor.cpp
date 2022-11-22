// Copyright Epic Games, Inc. All Rights Reserved.

#include "AngryAnimationToolsEditor.h"

#include "Modules/ModuleManager.h"
#include "Widgets/Docking/SDockTab.h"
#include "Modifiers/ModifierCommands.h"
#include "Modifiers/ModifierMirror.h"
#include "IControlRigEditorModule.h"
#include "LevelSequence.h"

#define LOCTEXT_NAMESPACE "FAngryAnimationToolsEditorModule"


FAngrySequencerCustomization::FAngrySequencerCustomization()
	: ModifierCommandBindings(new FUICommandList())
{
}

void FAngrySequencerCustomization::RegisterSequencerCustomization(FSequencerCustomizationBuilder& Builder)
{
	Sequencer = &Builder.GetSequencer();

	FSequencerCustomizationInfo Customization;

	TSharedRef<FExtender> ToolbarExtender = MakeShared<FExtender>();
	ToolbarExtender->AddToolBarExtension("Base Commands", EExtensionHook::After, nullptr, FToolBarExtensionDelegate::CreateRaw(this, &FAngrySequencerCustomization::ExtendSequencerToolbar));
	Customization.ToolbarExtender = ToolbarExtender;

	Customization.OnAssetsDrop.BindLambda([](const TArray<UObject*>&, const FAssetDragDropOp&) -> ESequencerDropResult { return ESequencerDropResult::DropDenied; });
	Customization.OnClassesDrop.BindLambda([](const TArray<TWeakObjectPtr<UClass>>&, const FClassDragDropOp&) -> ESequencerDropResult { return ESequencerDropResult::DropDenied; });
	Customization.OnActorsDrop.BindLambda([](const TArray<TWeakObjectPtr<AActor>>&, const FActorDragDropOp&) -> ESequencerDropResult { return ESequencerDropResult::DropDenied; });

	Builder.AddCustomization(Customization);
}

void FAngrySequencerCustomization::UnregisterSequencerCustomization()
{
}

void FAngrySequencerCustomization::ExtendSequencerToolbar(FToolBarBuilder& ToolbarBuilder)
{
	BindCommands();

	ToolbarBuilder.AddSeparator();

	ToolbarBuilder.AddComboButton(
		FUIAction(),
		FOnGetContent::CreateRaw(this, &FAngrySequencerCustomization::MakeModifiersMenu),
		LOCTEXT("Modifiers", "Modifiers"),
		LOCTEXT("ModifiersToolTip", "Modifiers"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "Sequencer.Actions")
	);
}

TSharedRef<SWidget> FAngrySequencerCustomization::MakeModifiersMenu()
{
	FAngryAnimationToolsEditorModule& AnimationEditor = FModuleManager::LoadModuleChecked<FAngryAnimationToolsEditorModule>("AngryAnimationToolsEditor");
	FMenuBuilder MenuBuilder(true, ModifierCommandBindings);

	// selection range actions
	MenuBuilder.BeginSection("Track", LOCTEXT("TrackHeader", "Track Modidifiers"));
	{
		MenuBuilder.AddMenuEntry(FModifierCommands::Get().MirrorX);
		MenuBuilder.AddMenuEntry(FModifierCommands::Get().Loop);
		MenuBuilder.AddMenuEntry(FModifierCommands::Get().TimeOffsetHalf);

		MenuBuilder.AddMenuEntry(FModifierCommands::Get().MirrorLeftToRight);
		MenuBuilder.AddMenuEntry(FModifierCommands::Get().FlipLeftToRight);

		MenuBuilder.AddMenuEntry(FModifierCommands::Get().MirrorRightToLeft);
		MenuBuilder.AddMenuEntry(FModifierCommands::Get().FlipRightToLeft);
	}
	MenuBuilder.EndSection();

	return MenuBuilder.MakeWidget();
}

void FAngrySequencerCustomization::BindCommands()
{
	const FModifierCommands& Commands = FModifierCommands::Get();

	ModifierCommandBindings->MapAction(Commands.MirrorX, FExecuteAction::CreateLambda([]() { FModifierMirror().Mirror(); }));
	ModifierCommandBindings->MapAction(Commands.Loop, FExecuteAction::CreateLambda([]() { FModifierMirror().Loop(); }));
	ModifierCommandBindings->MapAction(Commands.TimeOffsetHalf, FExecuteAction::CreateLambda([]() { FModifierMirror().TimeOffsetHalf(); }));

	ModifierCommandBindings->MapAction(Commands.MirrorLeftToRight, FExecuteAction::CreateLambda([]() { FModifierMirror().FlipLeftToRight(false); }));
	ModifierCommandBindings->MapAction(Commands.FlipLeftToRight, FExecuteAction::CreateLambda([]() { FModifierMirror().FlipLeftToRight(true); }));

	ModifierCommandBindings->MapAction(Commands.MirrorRightToLeft, FExecuteAction::CreateLambda([]() { FModifierMirror().FlipRightToLeft(false); }));
	ModifierCommandBindings->MapAction(Commands.FlipRightToLeft, FExecuteAction::CreateLambda([]() { FModifierMirror().FlipRightToLeft(true); }));
}


void FAngryAnimationToolsEditorModule::StartupModule()
{
	FModifierCommands::Register();

	ISequencerModule& SequencerModule = FModuleManager::LoadModuleChecked<ISequencerModule>("Sequencer");
	SequencerModule.GetSequencerCustomizationManager()->RegisterInstancedSequencerCustomization(ULevelSequence::StaticClass(),
		FOnGetSequencerCustomizationInstance::CreateLambda([]()
	{
		return new FAngrySequencerCustomization();
	}));

	OnSequencerCreatedHandle = SequencerModule.RegisterOnSequencerCreated(FOnSequencerCreated::FDelegate::CreateRaw(this, &FAngryAnimationToolsEditorModule::UpdateSequencer));
}

void FAngryAnimationToolsEditorModule::ShutdownModule()
{
	FModifierCommands::Unregister();

	ISequencerModule& SequencerModule = FModuleManager::LoadModuleChecked<ISequencerModule>("Sequencer");
	SequencerModule.UnregisterOnSequencerCreated(OnSequencerCreatedHandle);
}

void FAngryAnimationToolsEditorModule::UpdateSequencer(TSharedRef<ISequencer> Sequencer)
{
	CurrentSequencer = Sequencer;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAngryAnimationToolsEditorModule, AngryAnimationToolsEditor)