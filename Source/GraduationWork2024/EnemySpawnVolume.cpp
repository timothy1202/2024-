// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawnVolume.h"
#include "Components/BoxComponent.h"

// Sets default values
AEnemySpawnVolume::AEnemySpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Volume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	RootComponent = Volume;
}