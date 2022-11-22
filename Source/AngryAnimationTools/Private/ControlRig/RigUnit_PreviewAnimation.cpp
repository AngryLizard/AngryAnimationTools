// The Gateway of Realities: Planes of Existence.

#include "ControlRig/RigUnit_PreviewAnimation.h"

#include "ControlRig.h"
#include "Units/RigUnitContext.h"

FRigUnit_PreviewAnimation_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT();
	URigHierarchy* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		WorkData.bInitialized = false;
		WorkData.ConversionRig.Reset();
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		if (!IsValid(PreviewSettings.PreviewAnimation))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("No preview animation was supplied"));
		}
		else
		{
			USkeleton* Skeleton = PreviewSettings.PreviewAnimation->GetSkeleton();
			const FReferenceSkeleton& Reference = Skeleton->GetReferenceSkeleton();
			const FBoneIndexType BoneNum = Reference.GetNum();

			WorkData.bInitialized = false;
			if (!WorkData.bInitialized)
			{
				TArray<FBoneIndexType> InRequiredBoneIndexArray;
				InRequiredBoneIndexArray.Reset(BoneNum);
				for (FBoneIndexType BoneIndex = 0; BoneIndex < BoneNum; BoneIndex++)
				{
					InRequiredBoneIndexArray.Emplace(BoneIndex);
				}

				FCurveEvaluationOption CurveEvalOption(false);
				WorkData.BoneContainer = FBoneContainer(InRequiredBoneIndexArray, CurveEvalOption, *Skeleton);
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
						WorkData.ConversionRig = NewObject<UControlRig>(Context.OwningComponent, PreviewSettings.ConversionRigClass);
						WorkData.ConversionRig->Initialize(true);
						WorkData.ConversionRig->RequestInit();
					}

					// register skeletalmesh component

					WorkData.ConversionRig->GetDataSourceRegistry()->RegisterDataSource(UControlRig::OwnerComponent, Context.OwningComponent);

					WorkData.ConversionRig->SetBoneInitialTransformsFromRefSkeleton(Reference);

					WorkData.ConversionRig->Execute(EControlRigState::Init, TEXT("Update"));
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

					WorkData.ConversionRig->SetDeltaTime(Context.DeltaTime);
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

			WorkData.Time += Context.DeltaTime;
			const float Duration = PreviewSettings.PreviewAnimation->GetPlayLength();
			if (WorkData.Time > Duration)
			{
				WorkData.Time -= Duration;
			}
		}
	}
}

