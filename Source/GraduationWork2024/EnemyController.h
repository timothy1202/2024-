// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ST_Enemy.h"
#include "EnemySpawnVolume.h"
#include "EnemyController.generated.h"

class AEnemyBase;

UCLASS()
class GRADUATIONWORK2024_API AEnemyController : public AActor
{
	GENERATED_BODY()

private:
	TArray<AEnemyBase*> Enemys;

	UPROPERTY(EditInstanceOnly, Category = "Enemy", meta = (AllowPrivateAccess = "true"))
	TArray<AEnemySpawnVolume*> EnemySpawnSpots;
	
	UPROPERTY(EditInstanceOnly, Category = "Enemy", meta = (AllowPrivateAccess = "true"))
	FEnemyWave EnemyArmyMaxSpawn;

	UPROPERTY(EditInstanceOnly, Category = "Enemy", meta = (AllowPrivateAccess = "true"))
	TMap<FName, FArmyStore> EnemyArmyStorage;

public:	
	// Sets default values for this actor's properties
	AEnemyController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InitializeEnemyStorage();

	void SpawnByTier(const FEnemyType& EnemyInfo, const FName& EnemyKind, int Tier);

	void SpawnEnemy(const FST_Enemy& EnemyInfo);

	void SpawnArmy();

	void StartWave(int WaveNum);

	void CheckRendered(AEnemyBase* Enemy);

	UFUNCTION(BlueprintCallable)
	void RegisterRenderTarget(AEnemyBase* Enemy);

	UFUNCTION(BlueprintCallable)
	void RemoveFromRenderTarget(AEnemyBase* Enemy);

};
