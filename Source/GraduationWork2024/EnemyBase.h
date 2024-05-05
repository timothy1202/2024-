// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PooledEnemy.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "EnemyBase.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class UArrowComponent;

/**
 * 
 */
UCLASS()
class GRADUATIONWORK2024_API AEnemyBase : public APooledEnemy
{
	GENERATED_BODY()

public:
	AEnemyBase();

private:
	UPROPERTY(Category = Pawn, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY(Category = Pawn, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(Category = Pawn, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFloatingPawnMovement> PawnMovement;

	UPROPERTY(Category = Pawn, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UArrowComponent> ArrowComponent;

public:
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }
	FORCEINLINE UFloatingPawnMovement* GetPawnMovement() const { return PawnMovement; }

};
