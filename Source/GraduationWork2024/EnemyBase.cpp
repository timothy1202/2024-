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


AEnemyBase::AEnemyBase()
{

	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCylinder"));
	RootComponent = CapsuleComponent;
	CapsuleComponent->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(CapsuleComponent);
	Mesh->SetCollisionProfileName(TEXT("CharacterMesh"));
	Mesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->SetupAttachment(CapsuleComponent);

	PawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("PawnMovement"));

	RecognitionBoundary = CreateDefaultSubobject<USphereComponent>(TEXT("RecognitionBoundary"));
	RecognitionBoundary->SetupAttachment(CapsuleComponent);

	RecognitionBoundary->SetCollisionProfileName(TEXT("Custom"));

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
			AEnemyController* OtherObject = GetWorld()->SpawnActor<AEnemyController>(AEnemyController::StaticClass());
			if (OtherObject)
			{
				OtherObject->RegisterRenderTarget(this);
			}
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
	show_health_dis = SphereRadius;

	AAIController* MyController = UAIBlueprintHelperLibrary::GetAIController(this);
	AAIC_EnemyBase* EnemyController = Cast<AAIC_EnemyBase>(MyController);
	if (EnemyController)
	{
		EnemyController->ExecuteBT(MyBehaviorTree);
	}


}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("BeginPlay called!"));

	InitEnemyController();

	Init();
}