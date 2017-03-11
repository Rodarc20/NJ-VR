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

    AristaMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AristaMesh"));
    AristaMesh->SetupAttachment(RootComponent);

    //este estatic mesh es de 100 x100x100
    static ConstructorHelpers::FObjectFinder<UStaticMesh> AristaMeshAsset(TEXT("/Engine/BasicShapes/Cylinder"));//de usar este creo que debo crear un obtener un  material y ponerselo, este tiene el pivot en el centro de la esfera
    if (AristaMeshAsset.Succeeded()) {
        AristaMesh->SetStaticMesh(AristaMeshAsset.Object);//este objeto tiene el pivot en la parte inferior
        static ConstructorHelpers::FObjectFinder<UMaterial> AristaMaterialAsset(TEXT("/Engine/BasicShapes/BasicShapeMaterial"));//de usar este creo que debo crear un obtener un  material y ponerselo, este tiene el pivot en el centro de la esfera
        if (AristaMaterialAsset.Succeeded()) {
            AristaMesh->SetMaterial(0, AristaMaterialAsset.Object);
        }
        AristaMesh->SetWorldScale3D(FVector(2*Radio/100, 2*Radio/100, Distancia/100));//0.06f//este valor se debe calcular en base al radio,  y escalas, esta funcoin toma el diametro, por lo tnto seria algo como 2*radio/100
        //estos 2*, es por que el pivot esta enel centro de los static mesh
    }

}

void AAristaCilindro::Actualizar() {
    //los calculos de tamañao direccion y posición debe estar dentro de la arita solo deberia pasarle la información referente a los nodos, la rista sola debe autocalcular lo demas
    FVector Diferencia = TargetNodo->GetActorLocation() - SourceNodo->GetActorLocation();
    FVector Direccion = Diferencia.GetClampedToSize(1.0f, 1.0f);
    FVector NewLocation(Diferencia/2 + SourceNodo->GetActorLocation());//ejes invertidos a los recibidos
    float angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(FVector::UpVector, Direccion)));
    float sing = FVector::CrossProduct(FVector::UpVector, Direccion).X;//esto es por que el signo es impotante para saber si fue un angulo mayor de 180 o no
    if (sing >= 0) {
        angle = 360-angle;
    }
    FRotator NewRotation(0.0f, 0.0f, angle);
    SetActorLocationAndRotation(NewLocation, NewRotation);
    
    Distancia = Diferencia.Size()-2;
    AristaCollision->SetCapsuleHalfHeight(Distancia/2);
    AristaMesh->SetWorldScale3D(FVector(2*Radio/100, 2*Radio/100, Distancia/100));//0.06f//este valor se debe calcular en base al radio,  y escalas, esta funcoin toma el diametro, por lo tnto seria algo como 2*radio/100
}