// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseBuilding.h"

// Sets default values
ABaseBuilding::ABaseBuilding() : ATP(0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseBuilding::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseBuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int32 ABaseBuilding::GetBuildingATP()
{
	return ATP;
}

ABaseBuilding* ABaseBuilding::SpawnBuildinig(TSubclassOf<ABaseBuilding> OverrideBuilding)
{
	if (OverrideBuilding != nullptr)
		MyNextBuilding = OverrideBuilding;

	UWorld* const World = GetWorld();

	if (World != nullptr)
	{
		if (MyNextBuilding != nullptr)
		{
			FTransform SpawnTransform(GetActorRotation(), GetActorLocation());
			ABaseBuilding* NextBuilding = World->SpawnActorDeferred<ABaseBuilding>(MyNextBuilding, SpawnTransform);
			if (NextBuilding != nullptr)
			{
				NextBuilding->ToggleInit = false;
				NextBuilding->FinishSpawning(SpawnTransform);
				return NextBuilding;
			}
		}
	}
	return nullptr;
}
