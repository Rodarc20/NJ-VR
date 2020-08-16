// Fill out your copyright notice in the Description page of Project Settings.

#include "AristaCilindroPM.h"
#include "NJVR.h"
#include "Nodo.h"
#include "NJVR3DPMVRVisualization.h"


AAristaCilindroPM::AAristaCilindroPM() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

    Escala = 1.0f;
    Radio = 1.0f;
    Distancia = 6.0f;//esta distancia, inclue las semisferas
    //hay que instanciar y al final recien rotar, lamentablement estas aristas dependeran de los nodos, una vez instanciasdas debo llamar a una funcion apra que actualice sus posicion y rotaci�n, etc
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


    //debo ir a mi owner, ya que mi owner es la VRVisualization que me creo, no es necesario buscar, asi podre tener varios arboles a la vez si confusiones
    ANJVR3DPMVRVisualization * const VRVisualization = Cast<ANJVR3DPMVRVisualization>(GetOwner());
    if (VRVisualization) {
        UE_LOG(LogClass, Log, TEXT("Actualizando arita: %d"), Id);
        VRVisualization->UpdatePosicionesAristaMesh(Id, SourceNodo->GetTransform().GetLocation(), TargetNodo->GetTransform().GetLocation());
    }

    /*TArray<AActor *> RobotsEncontrados;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARobot::StaticClass(), RobotsEncontrados);
    //UE_LOG(LogClass, Log, TEXT("Numero de Partes Encontradas: %d"), PartesEncontradas.Num());
    for (int i = 0; i < RobotsEncontrados.Num(); i++) {
        ARobot * const RobotEncontrado = Cast<ARobot>(RobotsEncontrados[i]);
        Jerarquia = Jerarquias[JerarquiaCompleta];
        SetJerarquiaTask(EVRJerarquiaTask::ERotationTask);
        if (RobotEncontrado) {
            RobotEncontrado->SetJerarquiaTask(EVRJerarquiaTask::ERotationTask);
        }
    }*/
}

void AAristaCilindroPM::ActualizarCollision() {
    AristaCollision->SetCapsuleHalfHeight(Distancia/2);//modificar el tama�o del componete que hace la coslision, en este aso el capsule componente hace que actualizar el overlap aumente, en ese caso como no lo necesito por el momento al realizar traslaciones, esto puede estar desactivado hasta que lo necesite en laguna interaccion, y llamar a este cambio recien cuado suelte el boton de traslado
}
