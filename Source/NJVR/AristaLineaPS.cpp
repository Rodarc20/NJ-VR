// Fill out your copyright notice in the Description page of Project Settings.

#include "NJVR.h"
#include "AristaLineaPS.h"
#include "Nodo.h"

AAristaLineaPS::AAristaLineaPS() {
    Escala = 1.0f;
    Radio = 1.0f;

    Linea = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Linea"));
    Linea->SetRelativeLocation(FVector::ZeroVector);
    static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("ParticleSystem'/Game/Visualization/ParticleSystems/LaserBeam/BeamPS.BeamPS'"));//de usar este creo que debo crear un obtener un  material y ponerselo, este tiene el pivot en el centro de la esfera
    if (ParticleAsset.Succeeded()) {
        Linea->SetTemplate(ParticleAsset.Object);
    }

}

void AAristaLineaPS::Actualizar() {
    
}

void AAristaLineaPS::ActualizarCollision() {

}

void AAristaLineaPS::Tick(float DeltaTime) {
    //DrawDebugLine(GetWorld(), SourceNodo->GetActorLocation(), TargetNodo->GetActorLocation(), FColor::Black, false, -1.0f, 0, Radio);
    //Linea->SetBeamSourcePoint(0, SourceNodo->GetActorLocation(), 0);
    //Linea->SetBeamTargetPoint(0, TargetNodo->GetActorLocation(), 0);
}


