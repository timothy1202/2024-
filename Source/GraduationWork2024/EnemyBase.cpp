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
#include "BaseBuilding.h"
#include "FriendlyBase.h"
#include "CoreMinimal.h"


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

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	RecognitionBoundary->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::Recognition_OnOverlapBegin);
	RecognitionBoundary->OnComponentEndOverlap.AddDynamic(this, &AEnemyBase::Recognition_OnOverlapEnd);

	InitEnemyController();

}

void AEnemyBase::InitEnemyController()
{
	AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyController::StaticClass());

	if (FoundActor)
	{
		enemy_controller = Cast<AEnemyController>(FoundActor);

		if (enemy_controller)
		{
			enemy_controller->RegisterRenderTarget(this);
		}
	}
}

void AEnemyBase::SetPosibilities(FPosibilitesTray Tray)
{
	ResourceDropPosibilities.Add(Tray.firstPosibility);
	ResourceDropPosibilities.Add(Tray.secondPosibility);
	ResourceDropPosibilities.Add(Tray.thirdPosibility);
}

void AEnemyBase::Init()
{
	cur_health = EnemyNpc_MaxHealth;
	RecognitionBoundary->SetSphereRadius(EnemyNpc_AtkDist, true);

	FVector Origin;
	FVector BoxExtent;
	float SphereRadius = RecognitionBoundary->GetScaledSphereRadius();
	show_health_dis = SphereRadius * 1.5f;

	AAIController* MyController = UAIBlueprintHelperLibrary::GetAIController(this);
	AAIC_EnemyBase* EnemyAIController = Cast<AAIC_EnemyBase>(MyController);
	if (EnemyAIController)
	{
		EnemyAIController->ExecuteBT(MyBehaviorTree);
	}

	PawnMovement->MaxSpeed = EnemyNpc_RunSpeed;
	if (aggresive)
	{
		//PawnMovement->MaxSpeed = EnemyNpc_RunSpeed;

		UBlackboardComponent* BlackboardComp = MyController->GetBlackboardComponent();
		if (BlackboardComp)
		{
			BlackboardComp->SetValueAsBool("IsAggressive", true);
		}

		EnemyAIController->SetMyPawn(this);
	}
	else
	{
		switch (EnemyNpc_Tier)
		{
		case 1:
			SetPosibilities(FirstTierPosibility);
			break;
		case 2:
			SetPosibilities(SecondTierPosibility);
			break;
		case 3:
			SetPosibilities(ThirdTierPosibility);
			break;
		}
	}

	if (EnemyAIController)
	{
		UBlackboardComponent* BlackboardComp = EnemyAIController->GetBlackboardComponent();
		if (BlackboardComp)
		{
			if (is_long_range_npc)
			{
				if (aggresive)BlackboardComp->SetValueAsFloat("StopDistance", 800.0f);
				else BlackboardComp->SetValueAsFloat("StopDistance", 600.0f);
			}
			else
			{
				BlackboardComp->SetValueAsFloat("StopDistance", 100.0f);
			}
		}
	}
}

void AEnemyBase::Recognition_OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
}

void AEnemyBase::DetectOtherObject()
{
	FVector Start = ObjectDetectArrow->GetComponentLocation();
	FVector End = (ObjectDetectArrow->GetForwardVector() * 50.0f) + Start;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
	ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel4);

	FHitResult OutHit;

	GetWorld()->LineTraceSingleByObjectType(OutHit, Start, End, ObjectQueryParams);

	detect_other_objects = false;
	if (OutHit.bBlockingHit)
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, 0, 0);
		UPrimitiveComponent* HitComponent = OutHit.GetComponent();

		if (HitComponent != nullptr && HitComponent->ComponentHasTag(TEXT("EnemyDetect")))
		{
			if (HitComponent->ComponentHasTag(TEXT("FriendlyBuilding")))
			{
				ABaseBuilding* BuildingActor = Cast<ABaseBuilding>(OutHit.GetActor());
				if (BuildingActor)
				{
					if (!BuildingActor->GetBuilt())
					{
						return;
					}
				}
			}

			FString ComponentName = HitComponent->GetName();

			detect_other_objects = true;

			HighestATPTarget = OutHit.GetActor();
		}
	}
	else
	{
		detect_other_objects = false;
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
	}

	for (AActor* Actor : TaggedActors)
	{
		ABaseBuilding* BuildingActor = Cast<ABaseBuilding>(Actor);
		if (BuildingActor)
		{
			if (BuildingActor->GetBuilt())
			{
				UE_LOG(LogTemp, Warning, TEXT("Successfully Get BuildingATP!"));
				int32 CurrentATP = BuildingActor->GetBuildingATP();
				if (CurrentATP > HighestATP)
				{
					HighestATP = CurrentATP;
					HighestTarget = Actor;
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Building hasen't built!!!"));
			}
		}
	}

	return TPair<AActor*, int32>(HighestTarget, HighestATP);
}

TPair<AActor*, int32> AEnemyBase::GetHighestAlliesATP()
{
	AActor* HighestTarget = nullptr;
	int32 HighestATP = 0;

	TArray<AActor*> OverlappingActors;
	TArray<AActor*> TaggedActors;

	RecognitionBoundary->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		TArray<UActorComponent*> PlayerTaggedComponents =
			Actor->GetComponentsByTag(UActorComponent::StaticClass(), TEXT("PlayerAllies"));

		if (PlayerTaggedComponents.Num() > 0)
		{
			TaggedActors.Add(Actor);
		}
	}

	for (AActor* Actor : TaggedActors)
	{
		AFriendlyBase* AllyActor = Cast<AFriendlyBase>(Actor);
		if (AllyActor)
		{
			int32 CurrentATP = AllyActor->GetFriendlyATP();
			if (CurrentATP > HighestATP)
			{
				HighestATP = CurrentATP;
				HighestTarget = Actor;
			}
		}
	}

	return TPair<AActor*, int32>(HighestTarget, HighestATP);
}


void AEnemyBase::CheckDistance()
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

void AEnemyBase::UpdateDamagedHealthBar(float damage)
{
	cur_health = cur_health - damage;
}

void AEnemyBase::NpcDead_Implementation()
{
	CapsuleComponent->SetSimulatePhysics(false);
	RecognitionBoundary->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PlayerAimCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	APawn::DetachFromControllerPendingDestroy();

	DropItem();
	PlayDieEffectFun();
	OnDetroyEnemy.Broadcast(this);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	float MontageLength = 0;

	if (AnimInstance && DieMontage)
	{
		MontageLength = AnimInstance->Montage_Play(DieMontage, 1.0f, EMontagePlayReturnType::MontageLength);

	}
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AEnemyBase::NpcDeadAfterDelay, MontageLength, false);

}

int AEnemyBase::RandomResourceDrop()
{
	int sum = 0;
	int num = UKismetMathLibrary::RandomIntegerInRange(1, 100);
	for (int i = 0; i < ResourceDropPosibilities.Num(); i++)
	{
		bool InRange = UKismetMathLibrary::InRange_IntInt(num, sum, sum + ResourceDropPosibilities[i], false, true);

		if (InRange)
			return i;

		sum += ResourceDropPosibilities[i];
	}

	return -1;
}

void AEnemyBase::DropResource()
{
	int result = RandomResourceDrop();

	for (int i = 0; i < result; i++)
	{
		UWorld* const World = GetWorld();

		if (World != nullptr)
		{
			if (ResourceType != nullptr)
			{
				FTransform SpawnTransform(FRotator().ZeroRotator, GetActorLocation(), FVector(0.5f, 0.5f, 0.5f));
				ALootItem* ResourceInst = World->SpawnActorDeferred<ALootItem>(ResourceType, SpawnTransform);
				if (ResourceInst != nullptr)
				{
					ResourceInst->SetAngle(UKismetMathLibrary::RandomFloatInRange(0.f, 360.f));
					ResourceInst->SetDistance(item_drop_distance);
					ResourceInst->FinishSpawning(SpawnTransform);
				}
			}
		}
	}
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

void AEnemyBase::Deactivate()
{
	Super::Deactivate();
}

void AEnemyBase::SetActive(bool IsActive)
{
	Super::SetActive(IsActive);

	TArray<UActorComponent*> Comps;
	GetComponents<UActorComponent>(Comps);

	for (UActorComponent* component : Comps)
	{
		component->SetComponentTickEnabled(IsActive);
	}
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
				TPair<AActor*, int32> friendly_result = GetHighestAlliesATP();

				int32 HighestATPValue = -1;

				if (building_result.Value > HighestATPValue)
				{
					HighestATPValue = building_result.Value;
					HighestATPTarget = building_result.Key;
				}

				if (player_result.Value > HighestATPValue)
				{
					HighestATPValue = player_result.Value;
					HighestATPTarget = player_result.Key;
				}

				if (friendly_result.Value > HighestATPValue)
				{
					HighestATPValue = friendly_result.Value;
					HighestATPTarget = friendly_result.Key;
				}

				// HighestATPTarget을 로그로 출력
				if (HighestATPTarget)
				{
					UE_LOG(LogTemp, Warning, TEXT("Highest AT Target: %s"), *HighestATPTarget->GetName());
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("No valid target found."));
				}

				UE_LOG(LogTemp, Warning, TEXT("Successfully Get BuildingATP!"));
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
			TaggedActors.Add(Component->GetOwner());
		}
	}

	int Value = TaggedActors.Num();

	switch (Value)
	{
	case 0:
	{
		break;
	}
	case 1:
	{
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
		break;
	}
	}
}

bool AEnemyBase::CheckHighestATPAginstPlayer()
{
	if (HighestATPTarget == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		return true;
	}
	else
	{
		return false;
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
