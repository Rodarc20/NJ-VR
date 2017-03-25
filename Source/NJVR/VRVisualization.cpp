// Fill out your copyright notice in the Description page of Project Settings.

#include "NJVR.h"
#include "VRVisualization.h"


// Sets default values
AVRVisualization::AVRVisualization()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AVRVisualization::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVRVisualization::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVRVisualization::LoadNodos() {

}
void AVRVisualization::CreateNodos() {

}
void AVRVisualization::CreateAristas() {

}
EVRVisualizationTask AVRVisualization::GetVisualizationTask() {
    return CurrentVisualizationTask;
}

void AVRVisualization::SetVisualizationTask(EVRVisualizationTask NewVisualizationTask) {

}

EVRVisualizationMode AVRVisualization::GetVisualizationMode() {
    return CurrentVisualizationMode;
}

void AVRVisualization::SetVisualizationMode(EVRVisualizationMode NewVisualizationMode) {

}


void AVRVisualization::DeseleccionarNodo(ANodo * NodoSeleccionado) {

}

void AVRVisualization::SeleccionarRama(ANodo * NodoSeleccionado) {

}

void AVRVisualization::DeseleccionarRama(ANodo * NodoSeleccionado) {
    
}

void AVRVisualization::SeleccionarTodo() {

}

void AVRVisualization::DeseleccionarTodo() {

}

void AVRVisualization::AplicarTraslacion(FVector Traslacion) {

}

void AVRVisualization::AplicarRotacionRelativaANodo(ANodo* NodoReferencia, FVector PuntoReferencia) {

}


FString AVRVisualization::ObtenerContenidoNodo(ANodo* Nodo) {
    return FString();
}
