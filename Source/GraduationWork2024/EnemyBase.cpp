// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "EnemyController.h"
#include "Kismet/GameplayStatics.h"
#include "MyEnemyAI_HealthWidgetBase.h"

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
	HealthWidget->SetWidgetClass(UMyEnemyAI_HealthWidgetBase::StaticClass());

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
			// ĳ���� ���� �� AEnemyController ��ü ���� �� �ʱ�ȭ
			AEnemyController* OtherObject = GetWorld()->SpawnActor<AEnemyController>(AEnemyController::StaticClass());
			if (OtherObject)
			{
				OtherObject->RegisterRenderTarget(this);
			}
		}
	}
	else
	{
		// FoundActor�� nullptr�� ��� �α� ���
		UE_LOG(LogTemp, Warning, TEXT("AEnemyController instance not found."));
	}

}

void AEnemyBase::Init()
{
	UMyEnemyAI_HealthWidgetBase* MyWidget = Cast<UMyEnemyAI_HealthWidgetBase>(HealthWidget->GetUserWidgetObject());
	if (MyWidget != nullptr)
	{
		MyWidget->Init(); // ĳ���� ���� �� �Լ� ȣ��
	}

}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	// ���⿡ �ʱ�ȭ �ڵ� �ۼ�
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay called!"));

	InitEnemyController();

	Init();
}