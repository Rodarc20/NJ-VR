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
        Actualizar();//deberia llamarse solo si han sido movidos no seleccinados, podria ser que los nodos tengan un vector posicionultima, y vea si hes diferente a la actual y camibar, o al moverme, por quemplo en el tick, ver si el punto de colision es diferente, a uno anterior, y actualizar solo cuando se mueva, de esta forma las aristas solo deben revisar o yo llamar a las arsitas involucardas, es otra forma
    }

}
void AArista::Actualizar() {

}

void AArista::ActualizarCollision() {

}