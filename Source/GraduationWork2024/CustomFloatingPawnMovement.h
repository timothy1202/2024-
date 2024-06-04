// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "AI/Navigation/NavigationAvoidanceTypes.h"
#include "AI/RVOAvoidanceInterface.h"
#include "CustomFloatingPawnMovement.generated.h"

/**
 * 
 */
UCLASS()
class GRADUATIONWORK2024_API UCustomFloatingPawnMovement : public UFloatingPawnMovement, public IRVOAvoidanceInterface
{
	GENERATED_BODY()

protected:
	/** Character movement component belongs to */
	UPROPERTY(Transient, DuplicateTransient)
	class AEnemyBase* CharacterOwner;

public:
	UPROPERTY(Category = "Pawn Movement: Avoidance", EditAnywhere, BlueprintReadOnly, meta = (ForceUnits = cm))
	float AvoidanceConsiderationRadius;

	/** No default value, for now it's assumed to be valid if GetAvoidanceManager() returns non-NULL. */
	UPROPERTY(Category = "Pawn Movement: Avoidance", VisibleAnywhere, BlueprintReadOnly, AdvancedDisplay)
	int32 AvoidanceUID;

	/** Moving actor's group mask */
	UPROPERTY(Category = "Pawn Movement: Avoidance", EditAnywhere, BlueprintReadOnly, AdvancedDisplay)
	FNavAvoidanceMask AvoidanceGroup;

	/** Will avoid other agents if they are in one of specified groups */
	UPROPERTY(Category = "Pawn Movement: Avoidance", EditAnywhere, BlueprintReadOnly, AdvancedDisplay)
	FNavAvoidanceMask GroupsToAvoid;

	/** Will NOT avoid other agents if they are in one of specified groups, higher priority than GroupsToAvoid */
	UPROPERTY(Category = "Pawn Movement: Avoidance", EditAnywhere, BlueprintReadOnly, AdvancedDisplay)
	FNavAvoidanceMask GroupsToIgnore;

	/** De facto default value 0.5 (due to that being the default in the avoidance registration function), indicates RVO behavior. */
	UPROPERTY(Category = "Pawn Movement: Avoidance", EditAnywhere, BlueprintReadOnly)
	float AvoidanceWeight;

	virtual void PostLoad() override;
	virtual void SetRVOAvoidanceUID(int32 UID) override;
	virtual int32 GetRVOAvoidanceUID() override;
	virtual void SetRVOAvoidanceWeight(float Weight) override;
	virtual float GetRVOAvoidanceWeight() override;
	virtual FVector GetRVOAvoidanceOrigin() override;
	virtual float GetRVOAvoidanceRadius() override;
	virtual float GetRVOAvoidanceHeight() override;
	virtual float GetRVOAvoidanceConsiderationRadius() override;
	virtual FVector GetVelocityForRVOConsideration() override;
	virtual void SetAvoidanceGroupMask(int32 GroupFlags) override;
	virtual int32 GetAvoidanceGroupMask() override;
	virtual void SetGroupsToAvoidMask(int32 GroupFlags) override;
	virtual int32 GetGroupsToAvoidMask() override;
	virtual void SetGroupsToIgnoreMask(int32 GroupFlags) override;
	virtual int32 GetGroupsToIgnoreMask() override;
	

	FORCEINLINE AEnemyBase* GetCharacterOwner() const
	{
		return CharacterOwner;
	}
};
