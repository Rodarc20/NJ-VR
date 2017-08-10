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
    static ConstructorHelpers::FObjectFinder<UStaticMesh> AristaMeshAsset(TEXT("/Engine/BasicShapes/Cylinder"));//de usar este creo que debo crear un obtener un  material y ponerselo, este tiene el pivot en el centro de la esfera
    if (AristaMeshAsset.Succeeded()) {
        AristaMesh->SetCollisionProfileName(FName(TEXT("Arista")));
        AristaMesh->SetStaticMesh(AristaMeshAsset.Object);//este objeto tiene el pivot en la parte inferior
        static ConstructorHelpers::FObjectFinder<UMaterial> AristaMaterialAsset(TEXT("Material'/Game/Visualization/Materials/AristaMaterial.AristaMaterial'"));//de usar este creo que debo crear un obtener un  material y ponerselo, este tiene el pivot en el centro de la esfera
        if (AristaMaterialAsset.Succeeded()) {
            AristaMesh->SetMaterial(0, AristaMaterialAsset.Object);
        }
        AristaMesh->SetWorldScale3D(FVector(2*Radio/100, 2*Radio/100, Distancia/100));//0.06f//este valor se debe calcular en base al radio,  y escalas, esta funcoin toma el diametro, por lo tnto seria algo como 2*radio/100
        //estos 2*, es por que el pivot esta enel centro de los static mesh
    }

}

/*void AAristaCilindro::Actualizar() {
    //los calculos de tamañao direccion y posición debe estar dentro de la arita solo deberia pasarle la información referente a los nodos, la rista sola debe autocalcular lo demas
    FVector Diferencia = TargetNodo->GetActorLocation() - SourceNodo->GetActorLocation();
    FVector Direccion = Diferencia.GetClampedToSize(1.0f, 1.0f);
    FVector NewLocation(Diferencia/2 + SourceNodo->GetActorLocation());//ejes invertidos a los recibidos
    float angleRoll = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(FVector::UpVector, Direccion)));
    float singRoll = FVector::CrossProduct(FVector::UpVector, Direccion).X;//esto es por que el signo es impotante para saber si fue un angulo mayor de 180 o no
    if (singRoll >= 0) {
        angleRoll = 360-angleRoll;
    }
    float angleYaw = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(FVector::ForwardVector, Direccion)));
    float singYaw = FVector::CrossProduct(FVector::ForwardVector, Direccion).Z;//esto es por que el signo es impotante para saber si fue un angulo mayor de 180 o no
    if (singYaw >= 0) {
        angleYaw = 360-angleYaw;
    }
    //FRotator NewRotation(0.0f, 0.0f, angleRoll);
    //UE_LOG(LogClass, Log, TEXT("angleYaw = %f"), angleYaw);
    FRotator NewRotation(0.0f, angleYaw, angleRoll);
    SetActorLocationAndRotation(NewLocation, NewRotation);
    
    Distancia = Diferencia.Size()-3*Escala;
    //AristaCollision->SetCapsuleHalfHeight(Distancia/2);//modificar el tamaño del componete que hace la coslision, en este aso el capsule componente hace que actualizar el overlap aumente, en ese caso como no lo necesito por el momento al realizar traslaciones, esto puede estar desactivado hasta que lo necesite en laguna interaccion, y llamar a este cambio recien cuado suelte el boton de traslado
    AristaMesh->SetWorldScale3D(FVector(2*Radio/100*Escala, 2*Radio/100*Escala, Distancia/100));//0.06f//este valor se debe calcular en base al radio,  y escalas, esta funcoin toma el diametro, por lo tnto seria algo como 2*radio/100
}*/

void AAristaCilindro::Actualizar() {
    //los calculos de tamañao direccion y posición debe estar dentro de la arita solo deberia pasarle la información referente a los nodos, la rista sola debe autocalcular lo demas
    //FVector Diferencia = TargetNodo->GetActorLocation() - SourceNodo->GetActorLocation();
    FVector Diferencia = TargetNodo->GetTransform().GetLocation() - SourceNodo->GetTransform().GetLocation();//deberia ser solo los valores de y y z, funciona ahora por que en ambos x es 0, pero falla en el de abajo
    FVector Direccion = Diferencia.GetClampedToSize(1.0f, 1.0f);
    //FVector NewLocation(Diferencia/2 + SourceNodo->GetActorLocation());//ejes invertidos a los recibidos
    FVector NewLocation(Diferencia/2 + SourceNodo->GetTransform().GetLocation());//ejes invertidos a los recibidos
    float angleRoll = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(FVector::UpVector, Direccion)));
    float singRoll = FVector::CrossProduct(FVector::UpVector, Direccion).X;//esto es por que el signo es impotante para saber si fue un angulo mayor de 180 o no
    if (singRoll >= 0) {
        angleRoll = 360-angleRoll;
    }
    float angleYaw = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(FVector::RightVector, FVector(Direccion.X, Direccion.Y, 0.0f))));//estaba solo direccion
    float singYaw = FVector::CrossProduct(FVector::RightVector, FVector(Direccion.X, Direccion.Y, 0.0f)).Z;//esto es por que el signo es impotante para saber si fue un angulo mayor de 180 o no
    //float singYaw = FVector::CrossProduct(FVector::ForwardVector, FVector(Direccion.X, Direccion.Y, 0.0f)).Z;//esto es por que el signo es impotante para saber si fue un angulo mayor de 180 o no
    if (singYaw >= 0) {
        angleYaw = 360 - angleYaw;
    }
    //FRotator NewRotation(0.0f, 0.0f, angleRoll);
    UE_LOG(LogClass, Log, TEXT("angleYaw = %f"), angleYaw);
    FRotator NewRotation(0.0f, angleYaw, angleRoll);
    //SetActorLocationAndRotation(NewLocation, NewRotation);
    //FTransform NuevoTransform = GetTransform();
    //NuevoTransform.SetLocation(NewLocation);
    //NuevoTransform.SetRotation(NewRotation.Quaternion());
    //SetActorRelativeTransform(NuevoTransform);
    SetActorRelativeLocation(NewLocation);
    SetActorRelativeRotation(NewRotation);

    
    Distancia = Diferencia.Size()-3;
    //AristaCollision->SetCapsuleHalfHeight(Distancia/2);//modificar el tamaño del componete que hace la coslision, en este aso el capsule componente hace que actualizar el overlap aumente, en ese caso como no lo necesito por el momento al realizar traslaciones, esto puede estar desactivado hasta que lo necesite en laguna interaccion, y llamar a este cambio recien cuado suelte el boton de traslado
    AristaMesh->SetWorldScale3D(FVector(2*Radio/100, 2*Radio/100, Distancia/100));//0.06f//este valor se debe calcular en base al radio,  y escalas, esta funcoin toma el diametro, por lo tnto seria algo como 2*radio/100
}

void AAristaCilindro::ActualizarCollision() {
    AristaCollision->SetCapsuleHalfHeight(Distancia/2);//modificar el tamaño del componete que hace la coslision, en este aso el capsule componente hace que actualizar el overlap aumente, en ese caso como no lo necesito por el momento al realizar traslaciones, esto puede estar desactivado hasta que lo necesite en laguna interaccion, y llamar a este cambio recien cuado suelte el boton de traslado
}
