// Fill out your copyright notice in the Description page of Project Settings.


#include "AIC_EnemyBase.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "EnemyBase.h"
#include "CustomCrowdFollowingComponent.h"

AAIC_EnemyBase::AAIC_EnemyBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	PrimaryActorTick.bCanEverTick = true;

    auto PathFollowComp = FindComponentByClass<UCustomCrowdFollowingComponent>();
    if (PathFollowComp != nullptr)
    {
        SetPathFollowingComponent(PathFollowComp);
        auto CrowdFollowingComponentOverriden = Cast<UCustomCrowdFollowingComponent>(PathFollowComp);
        if (CrowdFollowingComponentOverriden != nullptr)
        {
            CrowdFollowingComponentOverriden->Initialize();
            CrowdFollowingComponentOverriden->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::High);
            CrowdFollowingComponentOverriden->SetCrowdSeparationWeight(200.0f);
            CrowdFollowingComponentOverriden->SetCrowdSeparation(true);
            CrowdFollowingComponentOverriden->SetCrowdCollisionQueryRange(10000.f);
            CrowdFollowingComponentOverriden->SetAvoidanceGroup(1);
            CrowdFollowingComponentOverriden->SetCrowdAnticipateTurns(true);
            CrowdFollowingComponentOverriden->UpdateCrowdAgentParams();
        }
    }
	
}

void AAIC_EnemyBase::BeginPlay()
{
	Super::BeginPlay();

}

void AAIC_EnemyBase::ExecuteBT(UBehaviorTree* const BT)
{
	if (BT)
	{
		UBlackboardComponent* myBlackboard;
		UseBlackboard(BT->BlackboardAsset, myBlackboard);
		Blackboard = myBlackboard;
		RunBehaviorTree(BT);
	}
}

void AAIC_EnemyBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime); 

   if (IsValid(myPawn))
    {
        if (myPawn->HighestATPTarget != highest_ATP_target)
        {
            highest_ATP_target = myPawn->HighestATPTarget;

            UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
            if (BlackboardComp)
            {
                BlackboardComp->SetValueAsObject("AttackTarget", highest_ATP_target);
				StopMovement();
            }
        }
    }
}

void AAIC_EnemyBase::SetMyPawn(AEnemyBase* my_Pawn)
{
	myPawn=my_Pawn;
}


USkeletalMeshComponent* AAIC_EnemyBase::GetMeshComponent() const
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn)
	{
		USkeletalMeshComponent* MeshComponent = Cast<USkeletalMeshComponent>(ControlledPawn->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
		return MeshComponent;
	}
	return nullptr;
}
