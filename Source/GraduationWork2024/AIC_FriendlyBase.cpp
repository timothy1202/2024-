#include "AIC_FriendlyBase.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "FriendlyBase.h"

AAIC_FriendlyBase::AAIC_FriendlyBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{

}

void AAIC_FriendlyBase::BeginPlay()
{
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
}

void AAIC_FriendlyBase::SetMyPawn(AFriendlyBase* my_Pawn)
{
	myPawn = my_Pawn;
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

