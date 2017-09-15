// Fill out your copyright notice in the Description page of Project Settings.

#include "NJVR.h"
#include "Nodo.h"
#include "AristaCilindro.h"



AAristaCilindro::AAristaCilindro()
{
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

    AristaMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AristaMesh"));
    AristaMesh->SetupAttachment(RootComponent);

    //este estatic mesh es de 100 x100x100
    //static ConstructorHelpers::FObjectFinder<UStaticMesh> AristaMeshAsset(TEXT("/Engine/BasicShapes/Cylinder"));//de usar este creo que debo crear un obtener un  material y ponerselo, este tiene el pivot en el centro de la esfera
    static ConstructorHelpers::FObjectFinder<UStaticMesh> AristaMeshAsset(TEXT("StaticMesh'/Game/Visualization/Mesh/AristaPrueba2.AristaPrueba2'"));//de usar este creo que debo crear un obtener un  material y ponerselo, este tiene el pivot en el centro de la esfera
    if (AristaMeshAsset.Succeeded()) {
        AristaMesh->SetCollisionProfileName(FName(TEXT("Arista")));
        AristaMesh->SetStaticMesh(AristaMeshAsset.Object);//este objeto tiene el pivot en la parte inferior
        static ConstructorHelpers::FObjectFinder<UMaterial> AristaMaterialAsset(TEXT("Material'/Game/Visualization/Materials/AristaMaterial.AristaMaterial'"));//de usar este creo que debo crear un obtener un  material y ponerselo, este tiene el pivot en el centro de la esfera
        if (AristaMaterialAsset.Succeeded()) {
            AristaMesh->SetMaterial(0, AristaMaterialAsset.Object);
        }
        AristaMesh->SetWorldScale3D(FVector(2*Radio*Escala/2, 2*Radio*Escala/2, Distancia/6));//0.06f//este valor se debe calcular en base al radio,  y escalas, esta funcoin toma el diametro, por lo tnto seria algo como 2*radio/100
        //AristaMesh->SetWorldScale3D(FVector(2*Radio*Escala/100, 2*Radio*Escala/100, Distancia/100));//0.06f//este valor se debe calcular en base al radio,  y escalas, esta funcoin toma el diametro, por lo tnto seria algo como 2*radio/100
        //estos 2*, es por que el pivot esta enel centro de los static mesh
        //la distancia no necesita ser escalada, se basa en las posiciones de otros
    }


}

void AAristaCilindro::Actualizar() {
    //los calculos de tamañao direccion y posición debe estar dentro de la arita solo deberia pasarle la información referente a los nodos, la rista sola debe autocalcular lo demas
    //FVector Diferencia = TargetNodo->GetActorLocation() - SourceNodo->GetActorLocation();
    FVector Diferencia = TargetNodo->GetTransform().GetLocation() - SourceNodo->GetTransform().GetLocation();//deberia ser solo los valores de y y z, funciona ahora por que en ambos x es 0, pero falla en el de abajo
    FVector NewLocation(SourceNodo->GetTransform().GetLocation() + Diferencia/2);//ejes invertidos a los recibidos
    FRotator NewRotation = FRotationMatrix::MakeFromZ(Diferencia).Rotator();
    SetActorLocation(NewLocation);
    SetActorRelativeRotation(NewRotation);

    Distancia = Diferencia.Size()-1;//el -1 deberia sser otro valor dianmico en funcion del tamaño de los nodos
    //AristaCollision->SetCapsuleHalfHeight(Distancia/2);//modificar el tamaño del componete que hace la coslision, en este aso el capsule componente hace que actualizar el overlap aumente, en ese caso como no lo necesito por el momento al realizar traslaciones, esto puede estar desactivado hasta que lo necesite en laguna interaccion, y llamar a este cambio recien cuado suelte el boton de traslado
    AristaMesh->SetWorldScale3D(FVector(2*Radio*Escala/2, 2*Radio*Escala/2, Distancia/6));//0.06f//este valor se debe calcular en base al radio,  y escalas, esta funcoin toma el diametro, por lo tnto seria algo como 2*radio/100
    //AristaMesh->SetWorldScale3D(FVector(2*Radio*Escala/100, 2*Radio*Escala/100, Distancia/100));//0.06f//este valor se debe calcular en base al radio,  y escalas, esta funcoin toma el diametro, por lo tnto seria algo como 2*radio/100
}

void AAristaCilindro::ActualizarCollision() {
    AristaCollision->SetCapsuleHalfHeight(Distancia/2);//modificar el tamaño del componete que hace la coslision, en este aso el capsule componente hace que actualizar el overlap aumente, en ese caso como no lo necesito por el momento al realizar traslaciones, esto puede estar desactivado hasta que lo necesite en laguna interaccion, y llamar a este cambio recien cuado suelte el boton de traslado
}
