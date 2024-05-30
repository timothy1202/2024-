#include "AIC_FriendlyBase.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "FriendlyBase.h"
#include "CustomCrowdFollowingComponent.h"

AAIC_FriendlyBase::AAIC_FriendlyBase(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
    PrimaryActorTick.bCanEverTick = true;
}

void AAIC_FriendlyBase::BeginPlay()
{
	Super::BeginPlay();
}

void AAIC_FriendlyBase::ExecuteBT(UBehaviorTree* BT)
{
	if (BT)
	{
		UBlackboardComponent* myBlackboard;
		UseBlackboard(BT->BlackboardAsset, myBlackboard);
		Blackboard = myBlackboard;
		RunBehaviorTree(BT);
	}
}

void AAIC_FriendlyBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (IsValid(myPawn))
    {
        if (myPawn->NpcAttackTarget != Closest_target)
        {
            Closest_target = myPawn->NpcAttackTarget;

            UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
            if (BlackboardComp)
            {
                BlackboardComp->SetValueAsObject("AttackTarget", Closest_target);
                StopMovement();
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("myPawn is not valid"));
    }
}


void AAIC_FriendlyBase::SetMyPawn(AFriendlyBase* my_Pawn1)
{
	myPawn = my_Pawn1;
}

USkeletalMeshComponent* AAIC_FriendlyBase::GetMeshComponent() const
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn)
	{
		USkeletalMeshComponent* MeshComponent = Cast<USkeletalMeshComponent>(ControlledPawn->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
		return MeshComponent;
	}
	return nullptr;
}

