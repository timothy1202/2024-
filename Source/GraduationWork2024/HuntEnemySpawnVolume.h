// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyBase.h"
#include "ICameraController.h"
#include "HuntEnemySpawnVolume.generated.h"

class UBoxComponent;

UCLASS()
class GRADUATIONWORK2024_API AHuntEnemySpawnVolume : public AActor, public IICameraController
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
	bool isCameraOut = false;
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

	UFUNCTION(BlueprintCallable)
	void Initialize();

	UFUNCTION(BlueprintCallable)
	void StartLogic();

	UFUNCTION(BlueprintCallable)
	void EndLogic();

	UFUNCTION()
	void OnEnemyDead(AEnemyBase* Enemy);

	void SpawnEnemy(FVector Pos);

	void DestroyEnemy();

	bool CheckPointInCamera(FVector Point);

	void FindPositionAndSpawnHuntEnemy();

	virtual void OnSpectator_Implementation(bool isEnable) override;

	virtual void OnSequencer_Implementation(bool isEnable) override;
};
