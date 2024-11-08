// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PooledEnemy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPooledObjectDespawn, APooledEnemy*, PoolEnemy);

UCLASS()
class GRADUATIONWORK2024_API APooledEnemy : public APawn
{
	GENERATED_BODY()

private:


public:
	// Sets default values for this character's properties
	APooledEnemy();

	FOnPooledObjectDespawn OnPooledObjectDespawn;

protected:
	bool active;
	int poolIndex;

public:	
#pragma region Pooling_Method

	UFUNCTION(BlueprintCallable, Category = "Pooled Object")
	virtual void Deactivate();

	virtual void SetActive(bool IsActive);

	FORCEINLINE void SetPoolIndex(int Index) { poolIndex = Index; }
	FORCEINLINE bool IsActive() const { return active; }
	FORCEINLINE int GetPoolIndex() const { return poolIndex; }

#pragma endregion
};
