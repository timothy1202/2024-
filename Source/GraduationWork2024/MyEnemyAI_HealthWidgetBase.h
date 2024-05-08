// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyEnemyAI_HealthWidgetBase.generated.h"

/**
 * EnemyAI_HealthWidget ±â¹Ý c++
 */
UCLASS()
class GRADUATIONWORK2024_API UMyEnemyAI_HealthWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Init();
	virtual void Init_Implementation();
	
};
