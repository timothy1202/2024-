// Fill out your copyright notice in the Description page of Project Settings.


#include "HuntEnemySpawnVolume.h"
#include "Components/BoxComponent.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AHuntEnemySpawnVolume::AHuntEnemySpawnVolume() : bExecute(false)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Volume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	RootComponent = Volume;

	Volume->SetCollisionProfileName("Volume");
	Volume->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
}

void AHuntEnemySpawnVolume::Volume_OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp->ComponentHasTag("Player"))
	{
		bExecute = true;
		PlayerRef = OtherActor;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BeginTimer!"));
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		if (TimerManager.IsTimerActive(SpawnTimerHandle))
		{
			TimerManager.ClearTimer(SpawnTimerHandle);
		}
		TimerManager.SetTimer(SpawnTimerHandle, this, &AHuntEnemySpawnVolume::FindPositionAndSpawnHuntEnemy, 5.f, true);
	}
}

void AHuntEnemySpawnVolume::Volume_OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherComp->ComponentHasTag("Player"))
	{
		bExecute = false;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("EndOverlap!"));
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

			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Spawn!"));
		}
	}
}

void AHuntEnemySpawnVolume::DestroyEnemy()
{
	if (!EnemyArr.IsEmpty())
	{
		// 적NPC arr에서 제거 - 플레이어 화면 내에 없을 때
		AEnemyBase* Enemy = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("Enemy Ready To Destroy!"));
		float MaxDistance = 0.f;
		for (AEnemyBase* Elem : EnemyArr)
		{
			float PlayerToEnemyDist = Elem->GetHorizontalDistanceTo(PlayerRef);
			if (MaxDistance < PlayerToEnemyDist)
			{
				UE_LOG(LogTemp, Warning, TEXT("SetMaxDistance"));
				MaxDistance = PlayerToEnemyDist;
				Enemy = Elem;
			}
		}
		if (Enemy != nullptr)
		{
			FVector EnemyPos = Enemy->GetActorLocation();
			if (!CheckPointInCamera(EnemyPos))
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
	FVector2D Offset = FVector2D(500.0f, 500.0f);
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
					if (count > 10)
					{
						GetWorld()->GetTimerManager().PauseTimer(SpawnTimerHandle);
					}
					else
					{
						SpawnEnemy(FinalLocation);
					}
					return;
				}
				else
				{
					FindPositionAndSpawnHuntEnemy();
				}
			}
		}
	}
	// 네비 상에서 가져올 수 있는 Volume의 한 위치를 구해서 적 NPC를 스폰 - 플레이어 화면 내에 없어야 함
}

void AHuntEnemySpawnVolume::OnEnemyDead(AEnemyBase* Enemy)
{
	EnemyArr.Remove(Enemy);
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

