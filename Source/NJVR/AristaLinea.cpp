// Fill out your copyright notice in the Description page of Project Settings.

#include "AristaLinea.h"
#include "NJVR.h"
#include "Nodo.h"

AAristaLinea::AAristaLinea() {
    Escala = 1.0f;
    Radio = 1.0f;
}

void AAristaLinea::Actualizar() {
    
}

void AAristaLinea::ActualizarCollision() {

}

void AAristaLinea::Tick(float DeltaTime) {
    DrawDebugLine(GetWorld(), SourceNodo->GetActorLocation(), TargetNodo->GetActorLocation(), FColor::Black, false, -1.0f, 0, Radio*Escala);
}

