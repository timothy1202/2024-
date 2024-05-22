// Fill out your copyright notice in the Description page of Project Settings.


#include "LootItem.h"

// Sets default values
ALootItem::ALootItem() : angle(0.f), distance(0.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALootItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALootItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

