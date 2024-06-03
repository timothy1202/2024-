// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyBase.h"
#include "HuntEnemySpawnVolume.generated.h"

class UBoxComponent;

UCLASS()
class GRADUATIONWORK2024_API AHuntEnemySpawnVolume : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> Volume;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AEnemyBase> EnemyType;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<AEnemyBase*> EnemyArr;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 MaxSpawnCount = 0;

	FTimerHandle SpawnTimerHandle;
	AActor* PlayerRef = nullptr;
	bool bExecute;
	int32 count = 0;
	
public:	
	// Sets default values for this actor's properties
	AHuntEnemySpawnVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void Volume_OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void Volume_OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SpawnEnemy(FVector Pos);

	void DestroyEnemy();

	bool CheckPointInCamera(FVector Point);

	void FindPositionAndSpawnHuntEnemy();

	UFUNCTION()
	void OnEnemyDead(AEnemyBase* Enemy);
};
