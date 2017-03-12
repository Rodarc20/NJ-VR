// Fill out your copyright notice in the Description page of Project Settings.

#include "NJVR.h"
#include "Arista.h"
#include "Nodo.h"


// Sets default values
AArista::AArista()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AArista::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AArista::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    if (SourceNodo->Selected || TargetNodo->Selected) {//solo debo llamar a esto ccuando este seleccionado, algo, en el modo deseleccionado, se supone que seta la visiulizacion, y se mueve la visualizacion, es decir todos los elementso, se arrastan
        Actualizar();
    }

}
void AArista::Actualizar() {

}

