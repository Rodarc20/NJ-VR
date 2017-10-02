// Fill out your copyright notice in the Description page of Project Settings.

#include "NJVR.h"
#include "Nodo.h"
#include "VRPawn.h"


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
    if (Contenido->IsVisible() && Usuario) {
        /*FVector NewLocation = GetActorLocation() + (Usuario->VRCamera->GetComponentLocation() - GetActorLocation()).GetSafeNormal()*10;
        Contenido->SetWorldLocation(NewLocation);*/
        FRotator NewRotation = FRotationMatrix::MakeFromX(Usuario->VRCamera->GetComponentLocation() - PuntoReferenciaContenido->GetComponentLocation()).Rotator();
        PuntoReferenciaContenido->SetWorldRotation(NewRotation);
        NewRotation = FRotationMatrix::MakeFromX(Usuario->VRCamera->GetComponentLocation() - Contenido->GetComponentLocation()).Rotator();
        Contenido->SetWorldRotation(NewRotation);
        LineaContenido->SetWorldLocation(GetActorLocation() + (Contenido->GetComponentLocation() - GetActorLocation()).GetSafeNormal()*Radio*2);
        LineaContenido->SetBeamTargetPoint(0, Contenido->GetComponentLocation(), 0);//o target
    }
}

void ANodo::CambiarColor(FLinearColor NewColor) {
    if (NodoMaterialDynamic != nullptr) {
        NodoMaterialDynamic->SetVectorParameterValue(TEXT("Color"), NewColor);// a que pareametro del material se le asiganara el nuevo color
    }
}

void ANodo::Actualizar() {

}

void ANodo::MostrarNombre() {

}

void ANodo::OcultarNombre() {

}

void ANodo::ActualizarRotacionNombre(FVector Direccion) {//deberia actualizarse solo, cuando este activo por alguna razon
    FRotator NewRotation = FRotationMatrix::MakeFromX(Direccion - NodoMesh->GetComponentLocation()).Rotator();
    NodoMesh->SetWorldRotation(NewRotation);
    NewRotation = FRotationMatrix::MakeFromX(Direccion - Nombre->GetComponentLocation()).Rotator();
    //NewRotation = FRotationMatrix::MakeFromX(Direccion).Rotator();
    Nombre->SetWorldRotation(NewRotation);
}

void ANodo::MostrarNumero() {

}

void ANodo::OcultarNumero() {

}

void ANodo::MostrarContenido() {
}

void ANodo::OcultarContenido() {
}

void ANodo::ActivarResaltado() {
}

void ANodo::ActivarResaltadoContenido() {
}

void ANodo::DesactivarResaltadoContenido() {
}

void ANodo::DesactivarResaltado() {
}

void ANodo::DeterminarResaltado() {
}
