// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ST_Enemy.h"
#include "EnemySpawnVolume.generated.h"

class UBoxComponent;

UCLASS()
class GRADUATIONWORK2024_API AEnemySpawnVolume : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> Volume;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "EnemyTypes")
	FName EnemyKind;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "EnemyTypes")
	FST_Enemy Enemys;
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawnVolume();

	FORCEINLINE FName GetEnemyKind() { return EnemyKind; }
};
