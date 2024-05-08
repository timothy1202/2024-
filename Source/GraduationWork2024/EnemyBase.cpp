// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "EnemyController.h"
#include "Kismet/GameplayStatics.h"

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
	AActor * FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyController::StaticClass());

	enemy_controller = Cast<AEnemyController>(FoundActor);

	if (enemy_controller == nullptr)
	{

	}
	else
	{
		AEnemyController* otherObject = GetWorld()->SpawnActor<AEnemyController>(AEnemyController::StaticClass());
		if (otherObject)
		{
			otherObject->RegisterRenderTarget(this);
		}
	}
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	// 여기에 초기화 코드 작성
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay called!"));
}