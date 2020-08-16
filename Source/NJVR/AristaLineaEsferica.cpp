// Fill out your copyright notice in the Description page of Project Settings.

#include "AristaLineaEsferica.h"
#include "NJVR.h"
#include "Nodo.h"


AAristaLineaEsferica::AAristaLineaEsferica() {
    Escala = 1.0f;
    Radio = 1.0f;
}

void AAristaLineaEsferica::Actualizar() {
    
}

void AAristaLineaEsferica::ActualizarCollision() {

}

void AAristaLineaEsferica::Tick(float DeltaTime) {
    FVector SourceRL = SourceNodo->GetRootComponent()->RelativeLocation;
    FVector TargetRL = TargetNodo->GetRootComponent()->RelativeLocation;
    float RadioEsfera = SourceRL.Size();
    float DeltaPhi = TargetNodo->Phi - SourceNodo->Phi;
    FVector P1;
    P1.X = RadioEsfera * FMath::Sin(SourceNodo->Phi + DeltaPhi/2) * FMath::Cos(SourceNodo->Theta);
    P1.Y = RadioEsfera * FMath::Sin(SourceNodo->Phi + DeltaPhi/2) * FMath::Sin(SourceNodo->Theta);
    P1.Z = RadioEsfera * FMath::Cos(SourceNodo->Phi + DeltaPhi/2);
    P1 -= SourceRL;
    P1 = SourceNodo->GetActorLocation() + P1;
    FVector P2;
    P2.X = RadioEsfera * FMath::Sin(SourceNodo->Phi + DeltaPhi/2) * FMath::Cos(TargetNodo->Theta);
    P2.Y = RadioEsfera * FMath::Sin(SourceNodo->Phi + DeltaPhi/2) * FMath::Sin(TargetNodo->Theta);
    P2.Z = RadioEsfera * FMath::Cos(SourceNodo->Phi + DeltaPhi/2);
    P2 -= TargetRL;
    P2 = TargetNodo->GetActorLocation() + P2;

    DrawDebugLine(GetWorld(), SourceNodo->GetActorLocation(), P1, FColor::Black, false, -1.0f, 0, Radio*Escala);
    DrawDebugLine(GetWorld(), P1, P2, FColor::Black, false, -1.0f, 0, Radio*Escala);
    DrawDebugLine(GetWorld(), P2, TargetNodo->GetActorLocation(), FColor::Black, false, -1.0f, 0, Radio*Escala);
}



