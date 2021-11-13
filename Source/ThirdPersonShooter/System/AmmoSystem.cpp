// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoSystem.h"

// Sets default values for this component's properties
UAmmoSystem::UAmmoSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAmmoSystem::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAmmoSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAmmoSystem::Reload()
{
	int32 AmmoChange = MaxAmmo - CurrentAmmo;
	if (AmmoInventory >= AmmoChange)
	{
		AmmoInventory -= AmmoChange;
		CurrentAmmo += AmmoChange;
	}
	else
	{
		AmmoInventory = 0;
		CurrentAmmo += AmmoInventory;
	}
}

