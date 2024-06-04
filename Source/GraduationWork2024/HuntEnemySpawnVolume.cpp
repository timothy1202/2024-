// Fill out your copyright notice in the Description page of Project Settings.


#include "HuntEnemySpawnVolume.h"
#include "Components/BoxComponent.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"

// Sets default values
AHuntEnemySpawnVolume::AHuntEnemySpawnVolume()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Volume = CreateDefaultSubobject<UBoxComponent>(TEXT("Volume"));
	RootComponent = Volume;

	Volume->SetCollisionProfileName("Volume");
	Volume->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
}

void AHuntEnemySpawnVolume::Initialize()
{
	PlayerRef = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void AHuntEnemySpawnVolume::StartLogic()
{
	if (PlayerRef != nullptr)
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		if (TimerManager.IsTimerActive(SpawnTimerHandle))
		{
			TimerManager.ClearTimer(SpawnTimerHandle);
		}
		TimerManager.SetTimer(SpawnTimerHandle, this, &AHuntEnemySpawnVolume::FindPositionAndSpawnHuntEnemy, 5.f, true);
	}
}

void AHuntEnemySpawnVolume::EndLogic()
{
	if (PlayerRef != nullptr)
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		if (TimerManager.IsTimerActive(SpawnTimerHandle))
		{
			TimerManager.ClearTimer(SpawnTimerHandle);
		}
		TimerManager.SetTimer(SpawnTimerHandle, this, &AHuntEnemySpawnVolume::DestroyEnemy, 1.f, true);
	}
}

void AHuntEnemySpawnVolume::SpawnEnemy(FVector Pos)
{
	UWorld* const World = GetWorld();
	FVector Offset = FVector(0.f, 0.f, 100.f);

	if (World != nullptr)
	{
		FTransform SpawnTransform(FRotator().ZeroRotator, Pos + Offset);
		AEnemyBase* Enemy = GetWorld()->SpawnActorDeferred<AEnemyBase>(EnemyType, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
		if (Enemy != nullptr)
		{
			Enemy->aggresive = false;
			Enemy->FinishSpawning(SpawnTransform);
			Enemy->OnDetroyEnemy.AddDynamic(this, &AHuntEnemySpawnVolume::OnEnemyDead);
			EnemyArr.Add(Enemy);
			count++;
		}
	}
}

void AHuntEnemySpawnVolume::DestroyEnemy()
{
	if (!EnemyArr.IsEmpty())
	{
		AEnemyBase* Enemy = nullptr;
		float MaxDistance = 0.f;

		for (AEnemyBase* Elem : EnemyArr)
		{
			FVector EnemyLocation = Elem->GetActorLocation();
			FVector PlayerLocation = PlayerRef->GetActorLocation();
			float PlayerToEnemyDist = UKismetMathLibrary::Vector_Distance2D(EnemyLocation, PlayerLocation);
			if (MaxDistance < PlayerToEnemyDist)
			{
				MaxDistance = PlayerToEnemyDist;
				Enemy = Elem;
			}
		}
		if (Enemy != nullptr)
		{
			if (!Enemy->WasRecentlyRendered(3.f))
			{
				EnemyArr.Remove(Enemy);
				Enemy->Destroy();
				count--;
			}
		}
	}
	else
	{
		PlayerRef = nullptr;
		GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
	}
}

bool AHuntEnemySpawnVolume::CheckPointInCamera(FVector Point)
{
	APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	FVector2D ScreenLocation = FVector2D::Zero();
	controller->ProjectWorldLocationToScreen(Point, ScreenLocation, false);

	FVector2D ScreenSize = FVector2D::Zero();
	FVector2D Offset = FVector2D(250.0f, 250.0f);
	GEngine->GameViewport->GetViewportSize(ScreenSize);
	ScreenSize += Offset;

	if (ScreenLocation.X > -Offset.X && ScreenLocation.Y > -Offset.Y && ScreenLocation.X < ScreenSize.X && ScreenLocation.Y < ScreenSize.Y)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AHuntEnemySpawnVolume::FindPositionAndSpawnHuntEnemy()
{
	if (!isCameraOut)
	{
		if (PlayerRef != nullptr)
		{
			FVector Origin = PlayerRef->GetActorLocation();
			if (auto* const NavSys = UNavigationSystemV1::GetCurrent(GetWorld()))
			{
				FNavLocation Loc;
				FVector BoxExtent = Volume->GetScaledBoxExtent();
				float MaxDistance = UKismetMathLibrary::Max(BoxExtent.X, BoxExtent.Y);

				if (NavSys->GetRandomReachablePointInRadius(Origin, MaxDistance, Loc))
				{
					FVector FinalLocation = Loc.Location;
					const FBoxSphereBounds& Box = Volume->Bounds;

					if (Box.GetBox().IsInside(FinalLocation) && !CheckPointInCamera(FinalLocation))
					{
						if (count >= MaxSpawnCount)
						{
							GetWorld()->GetTimerManager().PauseTimer(SpawnTimerHandle);
						}
						else
						{
							SpawnEnemy(FinalLocation);
						}
					}
					else
					{
						FindPositionAndSpawnHuntEnemy();
					}
				}
			}
		}
	}
}

void AHuntEnemySpawnVolume::OnEnemyDead(AEnemyBase* Enemy)
{
	EnemyArr.Remove(Enemy);
	count--;
	GetWorld()->GetTimerManager().UnPauseTimer(SpawnTimerHandle);
}

void AHuntEnemySpawnVolume::OnSpectator_Implementation(bool isEnable)
{
	if (isEnable)
		isCameraOut = true;
	else
		isCameraOut = false;
}

void AHuntEnemySpawnVolume::OnSequencer_Implementation(bool isEnable)
{
	if (isEnable)
		isCameraOut = true;
	else
		isCameraOut = false;
}

// Called when the game starts or when spawned
void AHuntEnemySpawnVolume::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AHuntEnemySpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

