

#include "AngryAnimationToolsEditor.h"

#include "Modules/ModuleManager.h"
#include "Widgets/Docking/SDockTab.h"
#include "Modifiers/ModifierCommands.h"
#include "Modifiers/ModifierMirror.h"
#include "IControlRigEditorModule.h"
#include "IAnimationEditorModule.h"
#include "Animation/AnimData/IAnimationDataController.h"
#include "IAnimationEditor.h"
#include "LevelSequence.h"

#define LOCTEXT_NAMESPACE "FAngryAnimationToolsEditorModule"

void FAngryAnimationToolsEditorModule::StartupModule()
{
	FModifierCommands::Register();

	ISequencerModule& SequencerModule = FModuleManager::LoadModuleChecked<ISequencerModule>("Sequencer");

	ModifierCommandBindings = MakeShared<FUICommandList>();
	BindModifierCommands();

	ActionsMenuExtender = MakeShared<FExtender>();
	ActionsMenuExtender->AddMenuExtension("Transform", EExtensionHook::Position::After, ModifierCommandBindings, FMenuExtensionDelegate::CreateRaw(this, &FAngryAnimationToolsEditorModule::ExtendSequencerActions));
	SequencerModule.GetActionsMenuExtensibilityManager()->AddExtender(ActionsMenuExtender);

	OnSequencerCreatedHandle = SequencerModule.RegisterOnSequencerCreated(FOnSequencerCreated::FDelegate::CreateRaw(this, &FAngryAnimationToolsEditorModule::UpdateSequencer));
	GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OnEditorOpeningPreWidgets().AddRaw(this, &FAngryAnimationToolsEditorModule::OnAssetOpening);

}

void FAngryAnimationToolsEditorModule::ShutdownModule()
{
	FModifierCommands::Unregister();
}

void FAngryAnimationToolsEditorModule::OnAssetOpening(const TArray<UObject*>& Objects, IAssetEditorInstance* Editor)
{
	if (!FModuleManager::Get().GetModule("AnimationEditor"))
	{
		return;
	}
	
	OpenedAnims.Reset(Objects.Num());
	for (UObject* Object : Objects)
	{
		if (UAnimSequence* Sequence = Cast<UAnimSequence>(Object))
		{
			OpenedAnims.Emplace(Sequence);
		}
	}

	if (ToolbarExtender.IsValid())
	{
		return;
	}

	IAnimationEditorModule& AnimationEditorModule = FModuleManager::GetModuleChecked<IAnimationEditorModule>("AnimationEditor");
	ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension("Editing", EExtensionHook::After, nullptr, FToolBarExtensionDelegate::CreateRaw(this, &FAngryAnimationToolsEditorModule::FillSequenceEditorToolbar));
	AnimationEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
}

void FAngryAnimationToolsEditorModule::UpdateSequencer(TSharedRef<ISequencer> Sequencer)
{
	CurrentSequencer = Sequencer;
}

void FAngryAnimationToolsEditorModule::FillSequenceEditorToolbar(FToolBarBuilder& ToolbarBuilder)
{
	ToolbarBuilder.BeginSection(TEXT("Repair"));
	{
		ToolbarBuilder.AddToolBarButton(
			FUIAction(FExecuteAction::CreateRaw(this, &FAngryAnimationToolsEditorModule::RepairSequence)),
			FName(TEXT("Repair Miscaled Sequence")),
			LOCTEXT("Repair_label", "Repair"),
			LOCTEXT("Repair_ToolTipOverride", "Repair bad scaling from blender export"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "PlayWorld.EjectFromPlayer"),
			EUserInterfaceActionType::Button,
			FName(TEXT("Repair"))
		);
	}
	ToolbarBuilder.EndSection();
}

void FAngryAnimationToolsEditorModule::RepairSequence()
{
	IAnimationEditorModule& AnimationEditorModule = FModuleManager::GetModuleChecked<IAnimationEditorModule>("AnimationEditor");

	for (UAnimSequence* OpenedAnim : OpenedAnims)
	{
		USkeleton* Skeleton = OpenedAnim->GetSkeleton();
		if (!IsValid(Skeleton))
		{
			continue;
		}

		IAnimationDataModel* DataModel = OpenedAnim->GetDataModel();
		IAnimationDataController& Controller = OpenedAnim->GetController();
		const FReferenceSkeleton& RefSkeleton = OpenedAnim->GetSkeleton()->GetReferenceSkeleton();

		if (RefSkeleton.GetNum() == 0)
		{
			continue;
		}

		const FName RootName = RefSkeleton.GetBoneName(0);
		const FBoneAnimationTrack& RootTrack = DataModel->GetBoneTrackByName(RootName);
		if(RootTrack.InternalTrackData.ScaleKeys.Num() == 0)
		{
			continue;
		}

		const TArray<FBoneAnimationTrack>& Tracks = DataModel->GetBoneAnimationTracks();
		for (const FBoneAnimationTrack& Track : Tracks)
		{
			const int32 Num = Track.InternalTrackData.PosKeys.Num();

			TArray<FVector3f> PosKeys;
			PosKeys.SetNumUninitialized(Num);
			for (int32 Index = 0; Index < Num; Index++)
			{
				PosKeys[Index] = Track.InternalTrackData.PosKeys[Index] * RootTrack.InternalTrackData.ScaleKeys[Index];
			}

			Controller.SetBoneTrackKeys(Track.Name, PosKeys, Track.InternalTrackData.RotKeys, Track.InternalTrackData.ScaleKeys, false);
		}

		TArray<FVector3f> Ones;
		Algo::Transform(RootTrack.InternalTrackData.ScaleKeys, Ones, [](const FVector3f& Scale) { return FVector3f::OneVector; });
		Controller.SetBoneTrackKeys(RootName, RootTrack.InternalTrackData.PosKeys, RootTrack.InternalTrackData.RotKeys, Ones, true);

		OpenedAnim->MarkPackageDirty();
	}
}

void FAngryAnimationToolsEditorModule::ExtendSequencerActions(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(FModifierCommands::Get().MirrorX);
	MenuBuilder.AddMenuEntry(FModifierCommands::Get().Loop);
	MenuBuilder.AddMenuEntry(FModifierCommands::Get().TimeOffsetHalf);

	MenuBuilder.AddMenuEntry(FModifierCommands::Get().MirrorLeftToRight);
	MenuBuilder.AddMenuEntry(FModifierCommands::Get().FlipLeftToRight);

	MenuBuilder.AddMenuEntry(FModifierCommands::Get().MirrorRightToLeft);
	MenuBuilder.AddMenuEntry(FModifierCommands::Get().FlipRightToLeft);
}

void FAngryAnimationToolsEditorModule::BindModifierCommands()
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

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAngryAnimationToolsEditorModule, AngryAnimationToolsEditor)