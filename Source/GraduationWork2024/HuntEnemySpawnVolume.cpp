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
	RootComponent = Volume;

	Volume->SetCollisionProfileName("Volume");
	Volume->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
}

void AHuntEnemySpawnVolume::Volume_OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp->ComponentHasTag("Player"))
	{
		PlayerRef = OtherActor;
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		if (TimerManager.IsTimerActive(SpawnTimerHandle))
		{
			TimerManager.ClearTimer(SpawnTimerHandle);
		}
		TimerManager.SetTimer(SpawnTimerHandle, this, &AHuntEnemySpawnVolume::FindPositionAndSpawnHuntEnemy, 1.f, true);
	}
}

void AHuntEnemySpawnVolume::Volume_OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherComp->ComponentHasTag("Player"))
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

	if (World != nullptr)
	{
		FTransform SpawnTransform(FRotator().ZeroRotator, Pos);
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
		// 적NPC arr에서 제거 - 플레이어 화면 내에 없을 때
		AEnemyBase* Enemy = nullptr;
		float MaxDistance = 0.f;

		// 멀리 있는 적부터 제거
		for (AEnemyBase* Elem : EnemyArr)
		{
			FVector EnemyLocation = Elem->GetActorLocation();
			FVector PlayerLocation = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation();
			float PlayerToEnemyDist = UKismetMathLibrary::Vector_Distance2D(EnemyLocation, PlayerLocation);
			if (MaxDistance < PlayerToEnemyDist)
			{
				MaxDistance = PlayerToEnemyDist;
				Enemy = Elem;
			}
		}
		if (Enemy != nullptr)
		{
			// 3초 내에 렌더링 된 적이 있다면
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
	// 네비 상에서 가져올 수 있는 Volume의 한 위치를 구해서 적 NPC를 스폰 - 플레이어 화면 내에 없어야 함
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

	if (EnemyType != nullptr)
	{
		Volume->OnComponentBeginOverlap.AddDynamic(this, &AHuntEnemySpawnVolume::Volume_OnOverlapBegin);
		Volume->OnComponentEndOverlap.AddDynamic(this, &AHuntEnemySpawnVolume::Volume_OnOverlapEnd);
	}
}

// Called every frame
void AHuntEnemySpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

