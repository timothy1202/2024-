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

	InitializeEnemyStorage();
	SpawnArmy();
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

void AEnemyController::InitializeEnemyStorage()
{
	for (AEnemySpawnVolume* Spot : EnemySpawnSpots)
	{
		EnemyArmyStorage.Add(Spot->GetEnemyKind(), FArmyStore());
	}
}

void AEnemyController::SpawnByTier(const FEnemyType& EnemyInfo, const FName& EnemyKind, int Tier)
{
	UWorld* const World = GetWorld();

	if (World != nullptr)
	{
		if (EnemyInfo.EnemyType != nullptr)
		{
			for (int i = 0; i < EnemyInfo.EnemySize; i++)
			{
				FTransform SpawnTransform(FRotator().ZeroRotator, FVector().ZeroVector);
				AEnemyBase* Enemy = World->SpawnActorDeferred<AEnemyBase>(EnemyInfo.EnemyType, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
				if (Enemy != nullptr)
				{
					Enemy->aggresive = true;
					Enemy->FinishSpawning(SpawnTransform);
					Enemy->SetActive(false);
					Enemy->SetPoolIndex(i);
					//
					switch (Tier)
					{
					case 1:
						EnemyArmyStorage[EnemyKind].FirstTierStrage.Enemys.Add(Enemy);
						break;
					case 2:
						EnemyArmyStorage[EnemyKind].SecondTierStrage.Enemys.Add(Enemy);
						break;
					case 3:
						EnemyArmyStorage[EnemyKind].ThirdTierStrage.Enemys.Add(Enemy);
					}
				}
			}
		}
	}
}

void AEnemyController::SpawnEnemy(const FST_Enemy& EnemyInfo)
{
	if (EnemyArmyStorage.Find(EnemyInfo.EnemyKind))
	{
		SpawnByTier(EnemyInfo.FirstTierEnemy, EnemyInfo.EnemyKind, 1);
		SpawnByTier(EnemyInfo.SecondTierEnemy, EnemyInfo.EnemyKind, 2);
		SpawnByTier(EnemyInfo.ThirdTierEnemy, EnemyInfo.EnemyKind, 3);
	}
}

void AEnemyController::SpawnArmy()
{
	SpawnEnemy(EnemyArmyMaxSpawn.SkeletonEnemys);
	SpawnEnemy(EnemyArmyMaxSpawn.LizardmanEnemys);
	SpawnEnemy(EnemyArmyMaxSpawn.OrcEnemys);
}

void AEnemyController::StartWave(int WaveNum)
{

}

void AEnemyController::CheckRendered(AEnemyBase* Enemy)
{
	if (Enemy)
	{
		USkeletalMeshComponent* EnemyMesh = Enemy->GetMesh();
		UFloatingPawnMovement* EnemyMovement = Enemy->GetPawnMovement();

		if (Enemy->IsActive())
		{
			APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			FVector2D ScreenLocation = FVector2D::Zero();
			controller->ProjectWorldLocationToScreen(Enemy->GetActorLocation(), ScreenLocation, false);

			FVector2D ScreenSize = FVector2D::Zero();
			GEngine->GameViewport->GetViewportSize(ScreenSize);
			ScreenSize += FVector2D(250.0f, 250.0f);

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
}

void AEnemyController::RegisterRenderTarget(AEnemyBase* Enemy)
{
	Enemys.AddUnique(Enemy);
}

void AEnemyController::RemoveFromRenderTarget(AEnemyBase* Enemy)
{
	Enemys.Remove(Enemy);
}
