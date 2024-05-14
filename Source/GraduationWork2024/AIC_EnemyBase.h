// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UObjectGlobals.h"
#include "AIController.h"
#include "AIC_EnemyBase.generated.h"


class AEnemyBase;
/**
 * 
 */
UCLASS()
class GRADUATIONWORK2024_API AAIC_EnemyBase : public AAIController
{
	GENERATED_BODY()
	
public:
	explicit AAIC_EnemyBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Behavior Tree")
	void ExecuteBT(UBehaviorTree* BT);

	UFUNCTION(BlueprintCallable)
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AEnemyBase* myPawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AActor* highest_ATP_target;

	USkeletalMeshComponent* GetMeshComponent() const;
};
