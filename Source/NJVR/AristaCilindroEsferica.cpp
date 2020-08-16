// Fill out your copyright notice in the Description page of Project Settings.

#include "AristaCilindroEsferica.h"
#include "NJVR.h"
#include "Nodo.h"


AAristaCilindroEsferica::AAristaCilindroEsferica() {
    Escala = 1.0f;
    Radio = 1.0f;
    Distancia = 6.0f;//esta distancia, inclue las semisferas
    //hay que instanciar y al final recien rotar, lamentablement estas aristas dependeran de los nodos, una vez instanciasdas debo llamar a una funcion apra que actualice sus posicion y rotación, etc
    AristaCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("AristaCollision"));// no necesito este componente
    RootComponent = AristaCollision;
    AristaCollision->InitCapsuleSize(Radio, Distancia/2);//multiplicarlos por la escala
    AristaCollision->SetCollisionProfileName(FName(TEXT("Arista")));

    //este estatic mesh es de 100 x100x100
    static ConstructorHelpers::FObjectFinder<UStaticMesh> AristaMeshAsset(TEXT("/Engine/BasicShapes/Cylinder"));//de usar este creo que debo crear un obtener un  material y ponerselo, este tiene el pivot en el centro de la esfera
    static ConstructorHelpers::FObjectFinder<UMaterial> AristaMaterialAsset(TEXT("Material'/Game/Visualization/Materials/AristaMaterial.AristaMaterial'"));//de usar este creo que debo crear un obtener un  material y ponerselo, este tiene el pivot en el centro de la esfera

    AristaInicioMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AristaInicioMesh"));
    AristaInicioMesh->SetupAttachment(RootComponent);

    if (AristaMeshAsset.Succeeded()) {
        AristaInicioMesh->SetCollisionProfileName(FName(TEXT("Arista")));
        AristaInicioMesh->SetStaticMesh(AristaMeshAsset.Object);//este objeto tiene el pivot en la parte inferior
        if (AristaMaterialAsset.Succeeded()) {
            AristaInicioMesh->SetMaterial(0, AristaMaterialAsset.Object);
        }
        AristaInicioMesh->SetWorldScale3D(FVector(2*Radio*Escala/100, 2*Radio*Escala/100, Distancia/100));//0.06f//este valor se debe calcular en base al radio,  y escalas, esta funcoin toma el diametro, por lo tnto seria algo como 2*radio/100
        //estos 2*, es por que el pivot esta enel centro de los static mesh
        //la distancia no necesita ser escalada, se basa en las posiciones de otros
    }
    AristaMedioMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AristaMedioMesh"));
    AristaMedioMesh->SetupAttachment(RootComponent);

    //este estatic mesh es de 100 x100x100
    if (AristaMeshAsset.Succeeded()) {
        AristaMedioMesh->SetCollisionProfileName(FName(TEXT("Arista")));
        AristaMedioMesh->SetStaticMesh(AristaMeshAsset.Object);//este objeto tiene el pivot en la parte inferior
        if (AristaMaterialAsset.Succeeded()) {
            AristaMedioMesh->SetMaterial(0, AristaMaterialAsset.Object);
        }
        AristaMedioMesh->SetWorldScale3D(FVector(2*Radio*Escala/100, 2*Radio*Escala/100, Distancia/100));//0.06f//este valor se debe calcular en base al radio,  y escalas, esta funcoin toma el diametro, por lo tnto seria algo como 2*radio/100
        //estos 2*, es por que el pivot esta enel centro de los static mesh
        //la distancia no necesita ser escalada, se basa en las posiciones de otros
    }
    AristaFinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AristaFinMesh"));
    AristaFinMesh->SetupAttachment(RootComponent);

    //este estatic mesh es de 100 x100x100
    if (AristaMeshAsset.Succeeded()) {
        AristaFinMesh->SetCollisionProfileName(FName(TEXT("Arista")));
        AristaFinMesh->SetStaticMesh(AristaMeshAsset.Object);//este objeto tiene el pivot en la parte inferior
        if (AristaMaterialAsset.Succeeded()) {
            AristaFinMesh->SetMaterial(0, AristaMaterialAsset.Object);
        }
        AristaFinMesh->SetWorldScale3D(FVector(2*Radio*Escala/100, 2*Radio*Escala/100, Distancia/100));//0.06f//este valor se debe calcular en base al radio,  y escalas, esta funcoin toma el diametro, por lo tnto seria algo como 2*radio/100
        //estos 2*, es por que el pivot esta enel centro de los static mesh
        //la distancia no necesita ser escalada, se basa en las posiciones de otros
    }
}

void AAristaCilindroEsferica::Actualizar() {
    FVector SourceRL = SourceNodo->GetRootComponent()->GetRelativeLocation();
    FVector TargetRL = TargetNodo->GetRootComponent()->GetRelativeLocation();
    float RadioEsfera = SourceRL.Size();
    float DeltaPhi = TargetNodo->Phi - SourceNodo->Phi;
    FVector P1;//se supone que ya esta en relativo
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
    //los puntos anteriores estan en el espacio del mundo, usaré el transform de este objeto para pasarlos al espacio local y calcuar o rotar de esa forma

    FVector Diferencia = TargetNodo->GetTransform().GetLocation() - SourceNodo->GetTransform().GetLocation();//deberia ser solo los valores de y y z, funciona ahora por que en ambos x es 0, pero falla en el de abajo
    FVector NewLocation(SourceNodo->GetTransform().GetLocation() + Diferencia/2);//ejes invertidos a los recibidos
    SetActorLocation(NewLocation);

    //FVector P1L = GetTransform().TransformPosition(P1);
    //FVector P2L = GetTransform().TransformPosition(P2);
    //estas lineas son muy costosas
    FVector P1L = GetTransform().InverseTransformPosition(P1);
    FVector P2L = GetTransform().InverseTransformPosition(P2);
    FVector SNL = GetTransform().InverseTransformPosition(SourceNodo->GetTransform().GetLocation());
    FVector TNL = GetTransform().InverseTransformPosition(TargetNodo->GetTransform().GetLocation());

    FRotator NewRotation = FRotationMatrix::MakeFromZ(P1L - P2L).Rotator();
    //AristaMedioMesh->SetRelativeLocation(FVector::ZeroVector);
    AristaMedioMesh->SetRelativeLocation(P2L + (P1L - P2L)/2);
    AristaMedioMesh->SetRelativeRotation(NewRotation);
    //Distancia = (P1-P2).Size();//el -1 deberia sser otro valor dianmico en funcion del tamaño de los nodos, esto deberia estar en relativo es decir usar los PL
    Distancia = (P1L-P2L).Size();//
    //AristaMedioMesh->SetWorldScale3D(FVector(2*Radio*Escala/100, 2*Radio*Escala/100, Distancia/100));//0.06f//este valor se debe calcular en base al radio,  y escalas, esta funcoin toma el diametro, por lo tnto seria algo como 2*radio/100
    AristaMedioMesh->SetRelativeScale3D(FVector(2*Radio*Escala/100, 2*Radio*Escala/100, Distancia/100));//0.06f//este valor se debe calcular en base al radio,  y escalas, esta funcoin toma el diametro, por lo tnto seria algo como 2*radio/100
     
    NewRotation = FRotationMatrix::MakeFromZ(P1L - SNL).Rotator();
    AristaInicioMesh->SetRelativeLocation(SNL + (P1L - SNL)/2);
    AristaInicioMesh->SetRelativeRotation(NewRotation);
    //Distancia = (P1-SNL).Size();//el -1 deberia sser otro valor dianmico en funcion del tamaño de los nodos
    Distancia = (P1L-SNL).Size();//el -1 deberia sser otro valor dianmico en funcion del tamaño de los nodos
    //AristaInicioMesh->SetWorldScale3D(FVector(2*Radio*Escala/100, 2*Radio*Escala/100, Distancia/100));//0.06f//este valor se debe calcular en base al radio,  y escalas, esta funcoin toma el diametro, por lo tnto seria algo como 2*radio/100
    AristaInicioMesh->SetRelativeScale3D(FVector(2*Radio*Escala/100, 2*Radio*Escala/100, Distancia/100));//0.06f//este valor se debe calcular en base al radio,  y escalas, esta funcoin toma el diametro, por lo tnto seria algo como 2*radio/100
    
    NewRotation = FRotationMatrix::MakeFromZ(TNL - P2L).Rotator();
    AristaFinMesh->SetRelativeLocation(P2L + (TNL - P2L)/2);
    AristaFinMesh->SetRelativeRotation(NewRotation);
    //Distancia = (TNL-P2).Size();//el -1 deberia sser otro valor dianmico en funcion del tamaño de los nodos
    Distancia = (TNL-P2L).Size();//el -1 deberia sser otro valor dianmico en funcion del tamaño de los nodos
    //AristaFinMesh->SetWorldScale3D(FVector(2*Radio*Escala/100, 2*Radio*Escala/100, Distancia/100));//0.06f//este valor se debe calcular en base al radio,  y escalas, esta funcoin toma el diametro, por lo tnto seria algo como 2*radio/100
    AristaFinMesh->SetRelativeScale3D(FVector(2*Radio*Escala/100, 2*Radio*Escala/100, Distancia/100));//0.06f//este valor se debe calcular en base al radio,  y escalas, esta funcoin toma el diametro, por lo tnto seria algo como 2*radio/100
}

void AAristaCilindroEsferica::ActualizarCollision() {

}

/*void AAristaCilindroEsferica::Tick(float DeltaTime) {
    FVector SourceRL = SourceNodo->GetRootComponent()->RelativeLocation;
    FVector TargetRL = TargetNodo->GetRootComponent()->RelativeLocation;
    float RadioEsfera = SourceRL.Size();
    float DeltaPhi = SourceNodo->Phi - TargetNodo
    FVector P1;
    Root->Parent->Xcoordinate = Radio * FMath::Sin(Root->Parent->Phi) * FMath::Cos(Root->Parent->Theta);
    Root->Parent->Ycoordinate = Radio * FMath::Sin(Root->Parent->Phi) * FMath::Sin(Root->Parent->Theta);
    Root->Parent->Zcoordinate = Radio * FMath::Cos(Root->Parent->Phi);
    P1.X = RadioEsfera * FMath::Sin(SourceRL->Phi) * FMath::Cos(Root->Parent->Theta);
    FVector P2;

    //DrawDebugLine(GetWorld(), SourceNodo->GetActorLocation(), TargetNodo->GetActorLocation(), FColor::Black, false, -1.0f, 0, Radio*Escala);
}*/


