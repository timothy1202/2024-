// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LootItem.generated.h"

UCLASS()
class GRADUATIONWORK2024_API ALootItem : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot Item", meta = (AllowPrivateAccess = "true", ExposeOnSpawn = "true"))
	float angle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot Item", meta = (AllowPrivateAccess = "true", ExposeOnSpawn = "true"))
	float distance;
	
public:	
	// Sets default values for this actor's properties
	ALootItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE void SetAngle(float targetAngle) { angle = targetAngle; }
	FORCEINLINE void SetDistance(float targetDistance) { distance = targetDistance; }

};
