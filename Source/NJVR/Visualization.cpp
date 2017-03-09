// Fill out your copyright notice in the Description page of Project Settings.

#include "NJVR.h"
#include "Visualization.h"


// Sets default values
AVisualization::AVisualization()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AVisualization::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVisualization::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

