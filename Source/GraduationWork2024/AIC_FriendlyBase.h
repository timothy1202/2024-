// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UObjectGlobals.h"
#include "AIController.h"
#include "AIC_FriendlyBase.generated.h"

class AFriendlyBase;
/**
 * 
 */
UCLASS()
class GRADUATIONWORK2024_API AAIC_FriendlyBase : public AAIController
{
	GENERATED_BODY()

public:
	explicit AAIC_FriendlyBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Behavior Tree")
	void ExecuteBT(UBehaviorTree* BT);

	UFUNCTION(BlueprintCallable)
	virtual void Tick(float DeltaTime) override;

	void SetMyPawn(AFriendlyBase* my_Pawn);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AFriendlyBase* myPawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AActor* highest_ATP_target;

	USkeletalMeshComponent* GetMeshComponent() const;
	
};
