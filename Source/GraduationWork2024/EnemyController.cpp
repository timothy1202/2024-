// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "EnemyBase.h"

// Sets default values
AEnemyController::AEnemyController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyController::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyBase::StaticClass(), OutActors);
	
	for (AActor* actor : OutActors)
	{
		AEnemyBase* enemy = Cast<AEnemyBase>(actor);
		Enemys.AddUnique(enemy);
	}

}

// Called every frame
void AEnemyController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (AEnemyBase* enemy : Enemys)
	{
		CheckRendered(enemy);
	}
}

void AEnemyController::CheckRendered(AEnemyBase* Enemy)
{
	if (Enemy)
	{
		APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		FVector2D ScreenLocation = FVector2D::Zero();
		controller->ProjectWorldLocationToScreen(Enemy->GetActorLocation(), ScreenLocation, false);

		FVector2D ScreenSize = FVector2D::Zero();
		GEngine->GameViewport->GetViewportSize(ScreenSize);
		ScreenSize += FVector2D(250.0f, 250.0f);

		USkeletalMeshComponent* EnemyMesh = Enemy->GetMesh();
		UFloatingPawnMovement* EnemyMovement = Enemy->GetPawnMovement();

		if (EnemyMesh && EnemyMovement)
		{
			if (ScreenLocation.X > -250.0f && ScreenLocation.Y > -250.0f && ScreenLocation.X < ScreenSize.X && ScreenLocation.Y < ScreenSize.Y)
			{
				EnemyMesh->SetComponentTickInterval(0.0f);
				EnemyMovement->SetComponentTickInterval(0.0f);
			}
			else
			{
				Enemy->GetMesh()->SetComponentTickInterval(0.1f);
				EnemyMovement->SetComponentTickInterval(0.5f);
			}
		}
	}
}

void AEnemyController::RegisterRenderTarget(AEnemyBase* Enemy)
{
	Enemys.AddUnique(Enemy);
}

void AEnemyController::RemoveFromRenderTarget(AEnemyBase* Enemy)
{
	Enemys.Remove(Enemy);
}
