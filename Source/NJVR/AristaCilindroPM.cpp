// Fill out your copyright notice in the Description page of Project Settings.

#include "NJVR.h"
#include "Nodo.h"
#include "AristaCilindroPM.h"


AAristaCilindroPM::AAristaCilindroPM() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

    Escala = 1.0f;
    Radio = 1.0f;
    Distancia = 6.0f;//esta distancia, inclue las semisferas
    //hay que instanciar y al final recien rotar, lamentablement estas aristas dependeran de los nodos, una vez instanciasdas debo llamar a una funcion apra que actualice sus posicion y rotación, etc
    AristaCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("AristaCollision"));
    RootComponent = AristaCollision;
    AristaCollision->InitCapsuleSize(Radio, Distancia/2);//multiplicarlos por la escala
    AristaCollision->SetCollisionProfileName(FName(TEXT("Arista")));

}

void AAristaCilindroPM::Actualizar() {
    FVector Diferencia = TargetNodo->GetTransform().GetLocation() - SourceNodo->GetTransform().GetLocation();//deberia ser solo los valores de y y z, funciona ahora por que en ambos x es 0, pero falla en el de abajo
    FVector NewLocation(SourceNodo->GetTransform().GetLocation() + Diferencia/2);//ejes invertidos a los recibidos
    FRotator NewRotation = FRotationMatrix::MakeFromZ(Diferencia).Rotator();
    SetActorLocation(NewLocation);
    //SetActorRelativeRotation(NewRotation);// debria ser world?
    SetActorRotation(NewRotation);

    Distancia = GetTransform().InverseTransformVector(Diferencia).Size();
}

void AAristaCilindroPM::ActualizarCollision() {
    AristaCollision->SetCapsuleHalfHeight(Distancia/2);//modificar el tamaño del componete que hace la coslision, en este aso el capsule componente hace que actualizar el overlap aumente, en ese caso como no lo necesito por el momento al realizar traslaciones, esto puede estar desactivado hasta que lo necesite en laguna interaccion, y llamar a este cambio recien cuado suelte el boton de traslado
}
