// Fill out your copyright notice in the Description page of Project Settings.

#include "NJVR.h"
#include "Nodo.h"


// Sets default values
ANodo::ANodo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    Color = FLinearColor::White;
}

// Called when the game starts or when spawned
void ANodo::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANodo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANodo::CambiarColor(FLinearColor NewColor) {
    if (NodoMaterialDynamic != nullptr) {
        NodoMaterialDynamic->SetVectorParameterValue(TEXT("Color"), NewColor);// a que pareametro del material se le asiganara el nuevo color
    }
}

