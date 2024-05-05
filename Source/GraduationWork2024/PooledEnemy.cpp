// Fill out your copyright notice in the Description page of Project Settings.


#include "PooledEnemy.h"

// Sets default values
APooledEnemy::APooledEnemy() : active(false), poolIndex(0)
{
 	
}

void APooledEnemy::Deactivate()
{
    SetActive(false);
    OnPooledObjectDespawn.Broadcast(this);
}

void APooledEnemy::SetActive(bool IsActive)
{
    active = IsActive;
    SetActorHiddenInGame(IsActive);
}