

#include "ControlRig/RigUnit_PreviewAnimation.h"

#include "Components/SceneComponent.h"
#include "Units/RigUnitContext.h"
#include "ControlRig.h"

FRigUnit_PreviewAnimation_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT();
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (!Hierarchy)
	{
		return;
	}

	if (!IsValid(PreviewSettings.PreviewAnimation))
	{
		UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("No preview animation was supplied"));
	}
	else
	{
		USkeleton* Skeleton = PreviewSettings.PreviewAnimation->GetSkeleton();
		const FReferenceSkeleton& Reference = Skeleton->GetReferenceSkeleton();
		const FBoneIndexType BoneNum = Reference.GetNum();

		// TODO: Make this work properly
		WorkData.bInitialized = false;
		if (!WorkData.bInitialized)
		{
			WorkData.ConversionRig.Reset();

			TArray<FBoneIndexType> InRequiredBoneIndexArray;
			InRequiredBoneIndexArray.Reset(BoneNum);
			for (FBoneIndexType BoneIndex = 0; BoneIndex < BoneNum; BoneIndex++)
			{
				InRequiredBoneIndexArray.Emplace(BoneIndex);
			}

			UE::Anim::FCurveFilterSettings CurveFilterSettings(UE::Anim::ECurveFilterMode::DisallowAll);
			WorkData.BoneContainer = FBoneContainer(InRequiredBoneIndexArray, CurveFilterSettings, *Skeleton);
			WorkData.Pose.Empty();
			WorkData.Pose.SetBoneContainer(&WorkData.BoneContainer);
			WorkData.bInitialized = true;

			WorkData.Curve.InitFrom(WorkData.BoneContainer);
			WorkData.Attributes = UE::Anim::FStackAttributeContainer();

			/// //////////////////////

			if (*PreviewSettings.ConversionRigClass)
			{
				if (!WorkData.ConversionRig.IsValid() || !WorkData.ConversionRig->IsA(PreviewSettings.ConversionRigClass))
				{
					WorkData.ConversionRig = NewObject<UControlRig>(PreviewSettings.PreviewAnimation, PreviewSettings.ConversionRigClass);
					WorkData.ConversionRig->Initialize(true);
					WorkData.ConversionRig->RequestInit();
				}

				// register skeletalmesh component
				// TODO: Reenable by doing something like AcquireSkeletonAndSkelMeshCompFromObject
				//WorkData.ConversionRig->GetDataSourceRegistry()->RegisterDataSource(UControlRig::OwnerComponent, ExecuteContext.GetOwningComponent());

				WorkData.ConversionRig->SetBoneInitialTransformsFromRefSkeleton(Reference);

				WorkData.ConversionRig->Execute(TEXT("Update"));
			}

			/// //////////////////////

			FAnimationPoseData PoseData(WorkData.Pose, WorkData.Curve, WorkData.Attributes);
			FAnimExtractContext ExtractContext(WorkData.Time, false);
			PreviewSettings.PreviewAnimation->GetBonePose(PoseData, ExtractContext);

			if (WorkData.ConversionRig.IsValid())
			{
				URigHierarchy* ConversionBoneHierarchy = WorkData.ConversionRig->GetHierarchy();

				for (FBoneIndexType BoneIndex = 0; BoneIndex < BoneNum; BoneIndex++)
				{
					const FName KeyName = Reference.GetBoneName(BoneIndex);
					const FRigElementKey Key = FRigElementKey(KeyName, ERigElementType::Bone);
					const FTransform& Transform = WorkData.Pose.GetBones()[BoneIndex];
					if (ConversionBoneHierarchy->GetIndex(Key) != INDEX_NONE)
					{
						ConversionBoneHierarchy->SetLocalTransform(Key, Transform, false, false);
					}
				}

				WorkData.ConversionRig->SetDeltaTime(ExecuteContext.GetDeltaTime());
				WorkData.ConversionRig->Evaluate_AnyThread();

				const URigHierarchy* ControlHierarchy = WorkData.ConversionRig->GetHierarchy();
				TArray<FRigControlElement*> Controls = ControlHierarchy->GetControls();

				const int32 ControlNum = Controls.Num();
				for (FBoneIndexType ControlIndex = 0; ControlIndex < ControlNum; ControlIndex++)
				{
					FRigControlElement* Control = Controls[ControlIndex];
					const FName KeyName = Control->GetName();
					const FTransform& Transform = ControlHierarchy->GetGlobalTransform(Control->GetKey());

					if (Hierarchy->GetIndex(FRigElementKey(KeyName, ERigElementType::Control)) != INDEX_NONE)
					{
						Hierarchy->SetGlobalTransform(FRigElementKey(KeyName, ERigElementType::Control), Transform, false, false);
					}
					else if (Hierarchy->GetIndex(FRigElementKey(KeyName, ERigElementType::Bone)) != INDEX_NONE)
					{
						Hierarchy->SetGlobalTransform(FRigElementKey(KeyName, ERigElementType::Bone), Transform, false, false);
					}
				}
			}
			else
			{
				for (FBoneIndexType BoneIndex = 0; BoneIndex < BoneNum; BoneIndex++)
				{
					const FName KeyName = Reference.GetBoneName(BoneIndex);
					const FTransform& Transform = WorkData.Pose.GetBones()[BoneIndex];
					if (Hierarchy->GetIndex(FRigElementKey(KeyName, ERigElementType::Control)) != INDEX_NONE)
					{
						Hierarchy->SetLocalTransform(FRigElementKey(KeyName, ERigElementType::Control), Transform, false, false);
					}
					else if (Hierarchy->GetIndex(FRigElementKey(KeyName, ERigElementType::Bone)) != INDEX_NONE)
					{
						Hierarchy->SetLocalTransform(FRigElementKey(KeyName, ERigElementType::Bone), Transform, false, false);
					}
				}
			}

			/// //////////////////////
		}

		WorkData.Time += ExecuteContext.GetDeltaTime();
		const float Duration = PreviewSettings.PreviewAnimation->GetPlayLength();
		if (WorkData.Time > Duration)
		{
			WorkData.Time -= Duration;
		}
	}
}

