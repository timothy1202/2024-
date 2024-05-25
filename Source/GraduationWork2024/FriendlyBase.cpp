// Fill out your copyright notice in the Description page of Project Settings.


#include "FriendlyBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "AIC_FriendlyBase.h"
#include "Components/ActorComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"


// Sets default values
AFriendlyBase::AFriendlyBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCylinder"));
	RootComponent = CapsuleComponent;
	CapsuleComponent->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->SetupAttachment(CapsuleComponent);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(CapsuleComponent);
	Mesh->SetCollisionProfileName(TEXT("CharacterMesh"));
	Mesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	PawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("PawnMovement"));

	RecognitionBoundary = CreateDefaultSubobject<USphereComponent>(TEXT("RecognitionBoundary"));
	RecognitionBoundary->SetupAttachment(CapsuleComponent);

	HealthWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
	HealthWidget->SetupAttachment(CapsuleComponent);
}

// Called when the game starts or when spawned
void AFriendlyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFriendlyBase::Init()
{
	cur_health = Unit_Max_Health;
	RecognitionBoundary->SetSphereRadius(Unit_ATK_Rng, true);

	FVector Origin;
	FVector BoxExtent;
	float SphereRadius;
	UKismetSystemLibrary::GetComponentBounds(Mesh, Origin, BoxExtent, SphereRadius);
	show_health_dis = SphereRadius * 1.5f;

	AAIController* MyController = UAIBlueprintHelperLibrary::GetAIController(this);
	AAIC_FriendlyBase* FriendlyController = Cast<AAIC_FriendlyBase>(MyController);
	if (FriendlyController)
	{
		FriendlyController->ExecuteBT(MyBehaviorTree);
		UE_LOG(LogTemp, Warning, TEXT("BT Ready"));
	}

	PawnMovement->MaxSpeed = Unit_Walk_Spd;
	FriendlyController->SetMyPawn(this);

	if (MyController)
	{
		UBlackboardComponent* BlackboardComp = MyController->GetBlackboardComponent();
		if (BlackboardComp)
		{
			if (is_long_range_npc)
			{
				BlackboardComp->SetValueAsFloat("StopDistance", 600.0f);
			}
			else
			{
				BlackboardComp->SetValueAsFloat("StopDistance", 60.0f);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Blackboard Component is null!"));
		}
	}

}


void AFriendlyBase::SetAttackTarget(AActor* AttackTarget)
{
	NpcAttackTarget = AttackTarget;

	AAIController* MyController = UAIBlueprintHelperLibrary::GetAIController(this);
	UBlackboardComponent* BlackboardComp = MyController ? MyController->GetBlackboardComponent() : nullptr;

	if (BlackboardComp)
	{
		if (IsValid(NpcAttackTarget))
		{
			BlackboardComp->SetValueAsObject("AttackTarget", AttackTarget);
		}
		else
		{
			BlackboardComp->SetValueAsObject("AttackTarget", nullptr);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BlackboardComp is null"));
	}
}

void AFriendlyBase::CheckDistance()
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	float Distance = 0;
	if (PlayerCharacter != nullptr)
	{
		Distance = GetHorizontalDistanceTo(PlayerCharacter);
	}

	if (Distance < show_health_dis)
		HealthWidget->SetVisibility(true);
	else
		HealthWidget->SetVisibility(false);
}

void AFriendlyBase::NpcDead_Implementation()
{
	RecognitionBoundary->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	APawn::DetachFromControllerPendingDestroy();

	PlayDieEffectFun(); //Play Die Effect 노드용 함수

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	float MontageLength = 0;

	if (AnimInstance && DieMontage)
	{
		MontageLength = AnimInstance->Montage_Play(DieMontage, 1.0f, EMontagePlayReturnType::MontageLength);
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AFriendlyBase::NpcDeadAfterDelay, MontageLength, false);
}

void AFriendlyBase::NpcDeadAfterDelay()
{
	Mesh->bPauseAnims;
	DestroyActor();
}

void AFriendlyBase::UpdateDamagedHealthBar(float damage)
{
	cur_health = cur_health - damage;
}

void AFriendlyBase::DestroyActor_Implementation()
{
	AActor::Destroy();
}

AActor* AFriendlyBase::FindClosestTarget()
{
	AActor* closest_target = nullptr;
	float min_distance = FLT_MAX; //최대값을 초기화

	TArray<AActor*> overlapping_actors;
	TArray<AActor*> tagged_actors;

	RecognitionBoundary->GetOverlappingActors(overlapping_actors);

	for (AActor* Actor : overlapping_actors)
	{
		TArray<UActorComponent*> PlayerTaggedComponents =
			Actor->GetComponentsByTag(UActorComponent::StaticClass(), TEXT("Enemy"));

		FString ActorName = Actor->GetName();
		UE_LOG(LogTemp, Log, TEXT("Actor Name: %s"), *ActorName);
		// 액터의 이름을 가져옵니다.

		if (PlayerTaggedComponents.Num() > 0)
		{
			tagged_actors.Add(Actor);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Actor does not have 'Enemy' tag: %s"), *ActorName);
		}
	}


	for (AActor* Actor : tagged_actors)
	{
		float current_distance_squared = (Actor->GetActorLocation() - GetActorLocation()).SizeSquared();
		if (min_distance > current_distance_squared)
		{
			min_distance = current_distance_squared;
			closest_target = Actor;
		}
	}

	return closest_target;
}

void AFriendlyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UE_LOG(LogTemp, Warning, TEXT("NpcAttackTarget is null"));

	if (!IsNpcDead)
	{
		NpcAttackTarget = FindClosestTarget();
		CheckDistance();

		// NpcAttackTarget이 유효한
		if (NpcAttackTarget)
		{
			// NpcAttackTarget의 이름을 로그로 출력
			UE_LOG(LogTemp, Warning, TEXT("NpcAttackTarget: %s"), *NpcAttackTarget->GetName());
		}
		else
		{
			// NpcAttackTarget이 유효하지 않을 경우의 로그
			UE_LOG(LogTemp, Warning, TEXT("NpcAttackTarget is null"));
		}
	}
}


// Called to bind functionality to input
void AFriendlyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

