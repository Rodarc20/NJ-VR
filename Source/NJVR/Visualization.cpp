// Fill out your copyright notice in the Description page of Project Settings.

#include "NJVR.h"
#include "Visualization.h"
#include "Nodo.h"
#include "Arista.h"


// Sets default values
AVisualization::AVisualization()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    static ConstructorHelpers::FClassFinder<ANodo> NodoClass(TEXT("/Script/NJVR.Nodo"));
    if (NodoClass.Succeeded()) {
        if(GEngine)//no hacer esta verificación provocaba error al iniciar el editor
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("TipoNodo encontrado."));
        TipoNodo = NodoClass.Class;
    }

    //Para buscar la clase arista que sera default en este codigo
    static ConstructorHelpers::FClassFinder<AArista> AristaClass(TEXT("/Script/NJVR.Arista"));
    if (AristaClass.Succeeded()) {
        if(GEngine)//no hacer esta verificación provocaba error al iniciar el editor
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("TipoArista encontrado."));
        TipoArista = AristaClass.Class;
    }

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

