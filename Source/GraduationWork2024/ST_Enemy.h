// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EnemyBase.h"
#include "ST_Enemy.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FEnemyType
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AEnemyBase> EnemyType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int EnemySize = 0;
};

USTRUCT(Atomic, BlueprintType)
struct FST_Enemy
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FName EnemyKind;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEnemyType FirstTierEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEnemyType SecondTierEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEnemyType ThirdTierEnemy;
};

USTRUCT(Atomic, BlueprintType)
struct FEnemyWave : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FST_Enemy SkeletonEnemys;

	UPROPERTY(EditAnywhere)
	FST_Enemy LizardmanEnemys;

	UPROPERTY(EditAnywhere)
	FST_Enemy OrcEnemys;
};

USTRUCT(Atomic, BlueprintType)
struct FEnemyStore
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<AEnemyBase*> Enemys;
	
	UPROPERTY(EditAnywhere)
	int NextEnemyNum;
};

USTRUCT(Atomic, BlueprintType)
struct FArmyStore
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FEnemyStore FirstTierStrage;

	UPROPERTY(EditAnywhere)
	FEnemyStore SecondTierStrage;

	UPROPERTY(EditAnywhere)
	FEnemyStore ThirdTierStrage;
};