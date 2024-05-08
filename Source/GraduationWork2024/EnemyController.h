// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyController.generated.h"

class AEnemyBase;

UCLASS()
class GRADUATIONWORK2024_API AEnemyController : public AActor
{
	GENERATED_BODY()

private:
	TArray<AEnemyBase*> Enemys;
	
public:	
	// Sets default values for this actor's properties
	AEnemyController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void CheckRendered(AEnemyBase* Enemy);

	UFUNCTION(BlueprintCallable)
	void RegisterRenderTarget(AEnemyBase* Enemy);

	UFUNCTION(BlueprintCallable)
	void RemoveFromRenderTarget(AEnemyBase* Enemy);

};
