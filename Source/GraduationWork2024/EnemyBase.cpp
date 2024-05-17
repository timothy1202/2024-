// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "EnemyController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AIC_EnemyBase.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Components/SceneComponent.h"
#include "ATPInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/PrimitiveComponent.h"
#include "Animation/AnimInstance.h"
#include "DrawDebugHelpers.h"


AEnemyBase::AEnemyBase()
{

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

	PlayerAimCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PlayerAimCollision"));
	PlayerAimCollision->SetupAttachment(CapsuleComponent);

	PlayerAimCollision->SetCollisionProfileName(TEXT("Enemy"));

	ObjectDetectArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ObjectDetectArrow"));
	ObjectDetectArrow->SetupAttachment(CapsuleComponent);

	HealthWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
	HealthWidget->SetupAttachment(CapsuleComponent);

}

void AEnemyBase::InitEnemyController()
{
	AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyController::StaticClass());

	if (FoundActor)
	{
		enemy_controller = Cast<AEnemyController>(FoundActor);

		if (!enemy_controller)
		{
			UE_LOG(LogTemp, Warning, TEXT("FoundActor is not an AEnemyController."));
		}
		else
		{
			enemy_controller->RegisterRenderTarget(this);
		}
	}
	else
	{
		// FoundActor가 nullptr인 경우 로그 출력
		UE_LOG(LogTemp, Warning, TEXT("AEnemyController instance not found."));
	}

}

void AEnemyBase::Init()
{
	cur_health = EnemyNpc_MaxHealth;
	RecognitionBoundary->SetSphereRadius(EnemyNpc_AtkDist, true); 

	FVector Origin;
	FVector BoxExtent;
	float SphereRadius;
	UKismetSystemLibrary::GetComponentBounds(Mesh,Origin, BoxExtent, SphereRadius);
	show_health_dis = SphereRadius * 1.5f;

	AAIController* MyController = UAIBlueprintHelperLibrary::GetAIController(this);
	AAIC_EnemyBase* EnemyController = Cast<AAIC_EnemyBase>(MyController);
	if (EnemyController)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExecuteBT"));
		EnemyController->ExecuteBT(MyBehaviorTree);
	}

	if (aggresive)
	{
		PawnMovement->MaxSpeed = EnemyNpc_RunSpeed;

		UBlackboardComponent* BlackboardComp = MyController->GetBlackboardComponent();
		if (BlackboardComp)
		{
			BlackboardComp->SetValueAsBool("IsAggressive", true);
		}

		EnemyController->SetMyPawn(this);
	}

	if (MyController)
	{
		UBlackboardComponent* BlackboardComp = MyController->GetBlackboardComponent();
		if (BlackboardComp)
		{
			if (is_long_range_npc)
			{
				UE_LOG(LogTemp, Warning, TEXT("is_long_range_npc called!"));
				if (aggresive)BlackboardComp->SetValueAsFloat("StopDistance", 800.0f);
				else BlackboardComp->SetValueAsFloat("StopDistance", 600.0f);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Short called!"));
				BlackboardComp->SetValueAsFloat("StopDistance", 70.0f);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Blackboard Component is null!"));
		}
	}
}

void AEnemyBase::Recognition_OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp,int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!aggresive)
	{
		if (OtherComp->ComponentHasTag("Player"))
		{
			ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
			SetAttackTarget(PlayerCharacter);
		}
	}
}

void AEnemyBase::Recognition_OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!aggresive)
	{
		if (!NeverStopChasing)
		{
			if (OtherComp->ComponentHasTag("Player"))
			{
				SetAttackTarget(nullptr);
			}
		}
	}
}

void AEnemyBase::SetAttackTarget(AActor* AttackTarget)
{
	if (AttackTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *AttackTarget->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AttackTarget is null"));
	}

	HighestATPTarget = AttackTarget;
	AAIController* MyController = UAIBlueprintHelperLibrary::GetAIController(this);
	UBlackboardComponent* BlackboardComp = MyController ? MyController->GetBlackboardComponent() : nullptr;

	if (BlackboardComp)
	{
		if (IsValid(HighestATPTarget))
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


void AEnemyBase::DetectOtherObject()
{
	FVector Start = ObjectDetectArrow->GetComponentLocation();
	FVector End = (ObjectDetectArrow->GetForwardVector() * 100.0f)+Start;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this); 

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
	ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel4);

	FHitResult OutHit; 

	GetWorld()->LineTraceSingleByObjectType(OutHit, Start, End, ObjectQueryParams);

	if (OutHit.bBlockingHit)
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, 0, 0);
		UPrimitiveComponent* HitComponent = OutHit.GetComponent();
		if (HitComponent != nullptr && HitComponent->ComponentHasTag(TEXT("EnemyDetect")))
		{
			FString ComponentName = HitComponent->GetName();

			UE_LOG(LogTemp, Warning, TEXT("Hit Component Name: %s"), *ComponentName);

			UE_LOG(LogTemp, Warning, TEXT("detect_other_objects is true"));
			detect_other_objects = true;
			if (OutHit.GetActor() != nullptr)
			{
				FString ActorName = OutHit.GetActor()->GetName();
				UE_LOG(LogTemp, Warning, TEXT("Hit Actor Name: %s"), *ActorName);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No actor hit."));
			}
			HighestATPTarget = OutHit.GetActor();
			FString ActorName = HighestATPTarget->GetName();
			UE_LOG(LogTemp, Warning, TEXT("HighestATPTarget Actor Name: %s"), *ActorName);
		}
		else UE_LOG(LogTemp, Warning, TEXT("detect_other_objects is false2222"));
	}

	else
	{
		detect_other_objects = false;
		UE_LOG(LogTemp, Warning, TEXT("detect_other_objects is false"));
	}

}

TPair<AActor*, int32> AEnemyBase::GetPlayerATP()
{
	AActor* HighestTarget = nullptr;
	int32 ATP = 0;

	TArray<AActor*> OverlappingActors;
	TArray<AActor*> TaggedActors;

	RecognitionBoundary->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		TArray<UActorComponent*> PlayerTaggedComponents =
			Actor->GetComponentsByTag(UActorComponent::StaticClass(), TEXT("Player"));

		if (PlayerTaggedComponents.Num() > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Overlapping Actor with 'Player' tagged component: %s"), *Actor->GetName());
			TaggedActors.Add(Actor);
		}
	}

	if (TaggedActors.Num() == 1)
	{
		ACharacter* PlayerCharacter = Cast<ACharacter>(TaggedActors[0]);
		if (PlayerCharacter)
		{
			ATP = 110;
			HighestTarget = PlayerCharacter;
		}
	}
	else if (TaggedActors.Num() == 0)
	{
		HighestTarget = nullptr;
		ATP = 0;
	}

	return TPair<AActor*, int32>(HighestTarget, ATP);
}

//
//TPair<AActor*, int32> AEnemyBase::GetPlayerATP()
//{
//	AActor* HighestTarget;
//	int32 ATP=0;
//
//	TArray<AActor*> OverlappingActors;
//	TArray<AActor*> TaggedActors;
//
//	RecognitionBoundary->GetOverlappingActors(OverlappingActors);
//
//	for (AActor* Actor : OverlappingActors)
//	{
//		TArray<UActorComponent*> PlayerTaggedComponents = 
//			Actor->GetComponentsByTag(UActorComponent::StaticClass(), TEXT("Player"));
//
//		if (PlayerTaggedComponents.Num() > 0)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("Overlapping Actor with 'Player' tagged component: %s"), *Actor->GetName());
//			TaggedActors.Add(Actor);
//		}
//	}
//
//	int Value = 0;
//	if (TaggedActors.Num() >= 0)
//	{
//		Value = TaggedActors.Num();
//	}
//
//	switch (Value)
//	{
//	case 0:
//	{
//		HighestTarget = NULL;
//		ATP = 0;
//		break;
//	}
//	case 1:
//	{
//		ACharacter* PlayerCharacter = Cast<ACharacter>(TaggedActors[0]);
//
//		if (PlayerCharacter)
//		{
//			ATP = 110;
//			HighestTarget = PlayerCharacter;
//		}
//		break;
//	}
//	default:
//		break;
//	}
//
//	return TPair<AActor*, int32>(HighestTarget, ATP);
//}

//TPair<AActor*, int32> AEnemyBase::GetHighestBuildingATP()
//{
//	AActor* HighestTarget;
//	int32 ATP;
//	TArray<int32> AtpArray;
//
//	TArray<AActor*> OverlappingActors;
//	TArray<AActor*> TaggedActors;
//
//	RecognitionBoundary->GetOverlappingActors(OverlappingActors);
//
//	for (AActor* Actor : OverlappingActors)
//	{
//		TArray<UActorComponent*> PlayerTaggedComponents =
//			Actor->GetComponentsByTag(UActorComponent::StaticClass(), TEXT("FriendlyBuilding"));
//
//		if (PlayerTaggedComponents.Num() > 0)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("Overlapping Actor with 'FriendlyBuilding' tagged component: %s"), *Actor->GetName());
//			TaggedActors.Add(Actor);
//		}
//		else UE_LOG(LogTemp, Warning, TEXT("PlayerTaggedComponents.Num() is null"));
//	}
//
//	int Value = 0;
//	if (TaggedActors.Num() >= 0)
//	{
//		Value = TaggedActors.Num();
//	}
//
//	switch (Value)
//	{
//		case 0:
//		{
//			HighestTarget = NULL;
//			ATP = 0;
//			break;
//		}
//		case 1:
//		{
//			AActor* SomeActor = TaggedActors[0];
//			if (SomeActor->GetClass()->ImplementsInterface(UATPInterface::StaticClass()))
//			{
//				int32 ActorATP = IATPInterface::Execute_GetATP(SomeActor);
//				ATP = ActorATP;
//				HighestTarget = SomeActor;
//			}
//			else
//			{
//				UE_LOG(LogTemp, Warning, TEXT("myInterface is null"));
//			}
//
//
//			break;
//		}
//		default:
//		{
//			for (int i = 0; i < TaggedActors.Num(); i++)
//			{
//				AActor* SomeActor = TaggedActors[i];
//				if (SomeActor->Implements<UATPInterface>())
//				{
//					IATPInterface* myInterface = Cast<IATPInterface>(SomeActor);
//					if (myInterface)
//					{
//						AtpArray.Add(myInterface->GetATP());
//					}
//				}
//				else UE_LOG(LogTemp, Warning, TEXT("myInterface is null"));
//			}
//
//
//			int32 IndexOfMaxValue;
//			UKismetMathLibrary::MaxOfIntArray(AtpArray, IndexOfMaxValue, ATP);
//			HighestTarget = TaggedActors[IndexOfMaxValue];
//			break;
//		}
//	}
//
//	return TPair<AActor*, int32>(HighestTarget, ATP);
//}

TPair<AActor*, int32> AEnemyBase::GetHighestBuildingATP()
{
	AActor* HighestTarget = nullptr;
	int32 HighestATP = 0;

	TArray<AActor*> OverlappingActors;
	TArray<AActor*> TaggedActors;

	RecognitionBoundary->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		TArray<UActorComponent*> PlayerTaggedComponents =
			Actor->GetComponentsByTag(UActorComponent::StaticClass(), TEXT("FriendlyBuilding"));

		if (PlayerTaggedComponents.Num() > 0)
		{
			TaggedActors.Add(Actor);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Actor does not have FriendlyBuilding tag"));
		}
	}

	for (AActor* Actor : TaggedActors)
	{
		if (Actor->Implements<UATPInterface>())
		{
			IATPInterface* myInterface = Cast<IATPInterface>(Actor);
			if (myInterface)
			{
				int32 CurrentATP = myInterface->GetATP();
				if (CurrentATP > HighestATP)
				{
					HighestATP = CurrentATP;
					HighestTarget = Actor;
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to cast to IATPInterface"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Actor does not implement ATPInterface"));
		}
	}

	return TPair<AActor*, int32>(HighestTarget, HighestATP);
}


void AEnemyBase::CheckDistance()
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	float Distance=0;
	if (PlayerCharacter != nullptr)
	{
		Distance = GetHorizontalDistanceTo(PlayerCharacter);
	}

	if (Distance < show_health_dis)
		HealthWidget->SetVisibility(true);
	else
		HealthWidget->SetVisibility(false);

}

void AEnemyBase::UpdateDamagedHealthBar(float damage)
{
	cur_health = cur_health - damage;
}

void AEnemyBase::NpcDead_Implementation()
{
	RecognitionBoundary->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PlayerAimCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	APawn::DetachFromControllerPendingDestroy();

	DropItem();
	PlayDieEffectFun(); //Play Die Effect 노드용 함수

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	float MontageLength=0;

	if (AnimInstance && DieMontage)
	{
		MontageLength = AnimInstance->Montage_Play(DieMontage, 1.0f, EMontagePlayReturnType::MontageLength);
		
	}
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AEnemyBase::NpcDeadAfterDelay, MontageLength, false);

}

void AEnemyBase::NpcDeadAfterDelay()
{
	Mesh->bPauseAnims;
	if (IsValid(enemy_controller))
	{
		enemy_controller->RemoveFromRenderTarget(this);
	}
	DestroyActor();
}

void AEnemyBase::DestroyActor_Implementation()
{
	AActor::Destroy();
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("BeginPlay called!"));

	RecognitionBoundary->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::Recognition_OnOverlapBegin);
	RecognitionBoundary->OnComponentEndOverlap.AddDynamic(this, &AEnemyBase::Recognition_OnOverlapEnd);

	InitEnemyController();

}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsNpcDead)
	{
		if (aggresive)
		{
			DetectOtherObject();
			if (!detect_other_objects)
			{
					TPair<AActor*, int32> building_result = GetHighestBuildingATP();
					TPair<AActor*, int32> player_result = GetPlayerATP();

					UE_LOG(LogTemp, Warning, TEXT("Building Result Value: %d"), building_result.Value);
					if (building_result.Key != nullptr)
					{
						UE_LOG(LogTemp, Warning, TEXT("Building Result Key: %s"), *building_result.Key->GetName());
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Building Result Key: NULL"));
					}

					UE_LOG(LogTemp, Warning, TEXT("Player Result Value: %d"), player_result.Value);
					if (player_result.Key != nullptr)
					{
						UE_LOG(LogTemp, Warning, TEXT("Player Result Key: %s"), *player_result.Key->GetName());
					}
					else
					{
							UE_LOG(LogTemp, Warning, TEXT("Player Result Key: NULL"));
					}

					if (building_result.Value > player_result.Value)
						HighestATPTarget = building_result.Key;
					else
						HighestATPTarget = player_result.Key;
			}
		}
		CheckDistance();
	}
}


void AEnemyBase::FirstThunderAttacked()
{
	TowerThunderTarget = this;
	FindFirstThunderTarget();
	if (IsValid(FirstThunderTarget))
	{   
		SpawnFirstThunder(FirstThunderTarget);
		UE_LOG(LogTemp, Warning, TEXT("FirstThunderAttacked called!"));
	}
}

void AEnemyBase::FindFirstThunderTarget()
{
	float Distance;
	TArray<float> DisArray;

	TArray<UPrimitiveComponent*> OverlappingComponents;
	TArray<AActor*> TaggedActors;

	RecognitionBoundary->GetOverlappingComponents(OverlappingComponents);

	for (UPrimitiveComponent* Component : OverlappingComponents)
	{
		if (Component->ComponentHasTag(TEXT("Enemy")))
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy Detected"));
			TaggedActors.Add(Component->GetOwner());
		}
	}

	int Value = TaggedActors.Num();

	switch (Value)
	{
		case 0:
		{
			UE_LOG(LogTemp, Warning, TEXT("FindFirstThunderTarget called! 000"));
			break;
		}
		case 1:
		{
			UE_LOG(LogTemp, Warning, TEXT("FindFirstThunderTarget called! 11"));
			break;
		}
		default:
		{
			for (int i = 0; i < TaggedActors.Num(); i++)
			{
				if (TaggedActors[i] != this)
				{
					Distance = UKismetMathLibrary::Vector_Distance(TaggedActors[i]->GetActorLocation(), GetActorLocation());
					DisArray.Add(Distance);
				}
				else
				{
					DisArray.Add(1000000000.0f);
				}
			}

			int32 IndexOfMinValue;
			UKismetMathLibrary::MinOfFloatArray(DisArray, IndexOfMinValue, Distance);
			FirstThunderTarget = TaggedActors[IndexOfMinValue];
			UE_LOG(LogTemp, Warning, TEXT("FindFirstThunderTarget called! default"));
			break;
		}
	}
}

void AEnemyBase::SecondThunderAttacked()
{
	FindSecondThunderTarget(TowerThunderTarget);
	if (IsValid(SecondThunderTarget))
	{
		SpawnSecondThunder(SecondThunderTarget);
	}
}

void AEnemyBase::FindSecondThunderTarget(AActor* tower_target)
{
	float Distance;
	TArray<float> DisArray;

	TArray<UPrimitiveComponent*> OverlappingComponents;
	TArray<AActor*> TaggedActors;

	RecognitionBoundary->GetOverlappingComponents(OverlappingComponents);

	for (UPrimitiveComponent* Component : OverlappingComponents)
	{
		if (Component->ComponentHasTag(TEXT("Enemy")))
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy Detected"));
			TaggedActors.Add(Component->GetOwner());
		}
	}

	int Value = TaggedActors.Num();

	switch (Value)
	{
		case 0:
			break;
		case 1:
			break;
		case 2:
		{
			if (TaggedActors[1] != this && TaggedActors[1] != tower_target)
			{
				SecondThunderTarget = TaggedActors[1];
			}
			break;
		}
		default:
		{
			for (int i = 0; i < TaggedActors.Num(); i++)
			{
				if (TaggedActors[i] == this || TaggedActors[i] == tower_target)
				{
					DisArray.Add(10000000000.0f);
				}
				else
				{
					Distance = UKismetMathLibrary::Vector_Distance(TaggedActors[i]->GetActorLocation(), GetActorLocation());
					DisArray.Add(Distance);
				}
			}

			int32 IndexOfMinValue;
			UKismetMathLibrary::MinOfFloatArray(DisArray, IndexOfMinValue, Distance);
			SecondThunderTarget = TaggedActors[IndexOfMinValue];
			break;
		}
	}

}
