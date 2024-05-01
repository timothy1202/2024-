// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PooledEnemy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPooledObjectDespawn, APooledEnemy*, PoolEnemy);

UCLASS()
class GRADUATIONWORK2024_API APooledEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APooledEnemy();

	FOnPooledObjectDespawn OnPooledObjectDespawn;

protected:
	bool active;
	float LifeSpan = 0.0f;
	int poolIndex;

	FTimerHandle LifeSpanTimer;

public:	
	UFUNCTION(BlueprintCallable, Category = "Pooled Object")
	void Deactivate();

	void SetActive(bool IsActive);

	FORCEINLINE void SetLifeSpan(float LifeTime) { LifeSpan = LifeTime; }
	FORCEINLINE void SetPoolIndex(int Index) { poolIndex = Index; }

	FORCEINLINE bool IsActive() const { return active; }
	FORCEINLINE int GetPoolIndex() const { return poolIndex; }
};
