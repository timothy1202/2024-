// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "NiagaraSystem.h"
#include "SwordTrail_AnimNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class GRADUATIONWORK2024_API USwordTrail_AnimNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara", meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* Trail;
};
