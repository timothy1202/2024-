// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PooledEnemy.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "LootItem.h"
#include "EnemyBase.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class UArrowComponent;
class USphereComponent;
class UWidgetComponent;
class AEnemyController;
class UBehaviorTree;

USTRUCT(Atomic, BlueprintType)
struct FPosibilitesTray
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int firstPosibility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int secondPosibility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int thirdPosibility;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDestroyEnemy, AEnemyBase*, Enemy);

/**
 * 
 */
UCLASS()
class GRADUATIONWORK2024_API AEnemyBase : public APooledEnemy
{
	GENERATED_BODY()

public:
	AEnemyBase();

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FOnDestroyEnemy OnDetroyEnemy;

private:
	UPROPERTY(Category = Pawn, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY(Category = Pawn, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(Category = Pawn, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFloatingPawnMovement> PawnMovement;

	UPROPERTY(Category = Pawn, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UArrowComponent> ArrowComponent;

	UPROPERTY(Category = PawnCollision, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> RecognitionBoundary;

	UPROPERTY(Category = PawnCollision, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> PlayerAimCollision;

	UPROPERTY(Category = Pawn, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UArrowComponent> ObjectDetectArrow;

	UPROPERTY(Category = Pawn, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> HealthWidget;

//변수용 private
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AEnemyController* enemy_controller;

//데이터 테이블 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FString EnemyNpcType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FString EnemyNpcName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 EnemyNpc_Tier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 MgSk_SkillSpawnCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float EnemyNpc_MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float EnemyNpc_WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float EnemyNpc_RunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float EnemyNpc_AtkDmg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float EnemyNpc_AtkSpd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float EnemyNpc_AtkDist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float EnemyNpc_AtkRng;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Lizard_HpRegen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Lizard_RegenTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Lizard_NoHitTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float PT_AtkDmg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float PT_AtkRng;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float IronMace_SkillAtkDmg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Claw_SkillAtkDmg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float PT_AtkRngDmg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Skill_Atk_Spd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Skill_Cool_Time;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Aggression_Run_Speed;

//체력 관련
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float cur_health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float show_health_dis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool IsNpcDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DieMontage;

//BT 관련
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* MyBehaviorTree;

//침략 관련

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool detect_other_objects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool is_highest_atp_player;

//사냥 관련
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool NeverStopChasing;

//아이템 드롭 관련
	UPROPERTY(EditDefaultsOnly, Category = "Resource", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ALootItem> ResourceType;

	UPROPERTY(EditAnywhere, Category = "Resource", meta = (AllowPrivateAccess = "true"))
	FPosibilitesTray FirstTierPosibility;

	UPROPERTY(EditAnywhere, Category = "Resource", meta = (AllowPrivateAccess = "true"))
	FPosibilitesTray SecondTierPosibility;

	UPROPERTY(EditAnywhere, Category = "Resource", meta = (AllowPrivateAccess = "true"))
	FPosibilitesTray ThirdTierPosibility;

	UPROPERTY(EditAnywhere, Category = "Resource", meta = (AllowPrivateAccess = "true"))
	TArray<int32> ResourceDropPosibilities;

	UPROPERTY(EditDefaultsOnly, Category = "Resource", meta = (AllowPrivateAccess = "true"))
	float item_drop_distance;

//번개 관련
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AActor* FirstThunderTarget; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AActor* SecondThunderTarget;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AActor* TowerThunderTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AActor* WhatsTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float ThunderScale;

//공격 관련
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool is_long_range_npc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool is_page_two;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AActor* HighestATPTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool is_playing_skill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	bool aggresive;

	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }
	FORCEINLINE UFloatingPawnMovement* GetPawnMovement() const { return PawnMovement; }

	UFUNCTION(BlueprintCallable)
	void InitEnemyController();

	UFUNCTION(BlueprintCallable)
	void Init();

	UFUNCTION()
	void Recognition_OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void Recognition_OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION(BlueprintCallable)
	void SetAttackTarget(AActor * AttackTarget);

	UFUNCTION()
	void DetectOtherObject();
	
	TPair<AActor*, int32> GetPlayerATP();
	TPair<AActor*, int32> GetHighestBuildingATP();
	TPair<AActor*, int32> GetHighestAlliesATP();

	void CheckDistance();

	UFUNCTION(BlueprintCallable)
	void UpdateDamagedHealthBar(float damage);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void NpcDead();
	virtual void NpcDead_Implementation();

	void SetPosibilities(FPosibilitesTray Tray);
	int RandomResourceDrop();

	UFUNCTION(BlueprintImplementableEvent)
	void DropItem();

	UFUNCTION(BlueprintCallable)
	void DropResource();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayDieEffectFun();

	void NpcDeadAfterDelay();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void DestroyActor();

	UFUNCTION(BlueprintCallable)
	void FirstThunderAttacked();

	UFUNCTION(BlueprintCallable)
	void SecondThunderAttacked();

	UFUNCTION(BlueprintCallable)
	void FindSecondThunderTarget(AActor* tower_target);

	UFUNCTION(BlueprintCallable)
	void FindFirstThunderTarget();

	UFUNCTION(BlueprintCallable)
	bool CheckHighestATPAginstPlayer();

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnFirstThunder(AActor * tunder_target);

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnSecondThunder(AActor* tunder_target);

	virtual void Deactivate() override;

	virtual void SetActive(bool IsActive) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

};

