// Fill out your copyright notice in the Description page of Project Settings.

#include "NJVR.h"
#include "NodoEsfera.h"


// establece los valores por defecto
ANodoEsfera::ANodoEsfera()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    //PrimaryActorTick.bCanEverTick = true;

    Escala = 1.0f;
    Radio = 3.0f;

    /*
    NodoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereStaticMeshComponent"));
    //NodoMesh->SetupAttachment(RootComponent);
    RootComponent = NodoMesh;
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Engine/BasicShapes/Sphere"));//de usar este creo que debo crear un obtener un  material y ponerselo, este tiene el pivot en el centro de la esfera
    if (SphereMeshAsset.Succeeded()) {
        NodoMesh->SetStaticMesh(SphereMeshAsset.Object);//este objeto tiene el pivot en la parte inferior
        //esto solo para cuando use los baisc de engine, si uso los del starter contento no
        static ConstructorHelpers::FObjectFinder<UMaterial> SphereMaterialAsset(TEXT("/Engine/BasicShapes/BasicShapeMaterial"));//de usar este creo que debo crear un obtener un  material y ponerselo, este tiene el pivot en el centro de la esfera
        if (SphereMaterialAsset.Succeeded()) {
            NodoMesh->SetMaterial(0, SphereMaterialAsset.Object);
        }
        NodoMesh->SetWorldScale3D(FVector(2 * Radio / 100));//0.06f//este valor se debe calcular en base al radio,  y escalas, esta funcoin toma el diametro, por lo tnto seria algo como 2*radio/100
    }

    NodoCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    NodoCollision->SetupAttachment(RootComponent);
    NodoCollision->InitSphereRadius(Radio);//tener cuidado con las unidades, tiene radio 6 quiza sea muy grande
*///no hay diferencia si lo hago de la otra forma

    NodoCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    RootComponent = NodoCollision;
    NodoCollision->InitSphereRadius(Radio);//tener cuidado con las unidades, tiene radio 6 quiza sea muy grande
    
    NodoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereStaticMeshComponent"));
    NodoMesh->SetupAttachment(RootComponent);
    //RootComponent = NodoMesh;
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Engine/BasicShapes/Sphere"));//de usar este creo que debo crear un obtener un  material y ponerselo, este tiene el pivot en el centro de la esfera
    if (SphereMeshAsset.Succeeded()) {
        NodoMesh->SetStaticMesh(SphereMeshAsset.Object);//este objeto tiene el pivot en la parte inferior
        //esto solo para cuando use los baisc de engine, si uso los del starter contento no
        static ConstructorHelpers::FObjectFinder<UMaterial> SphereMaterialAsset(TEXT("/Engine/BasicShapes/BasicShapeMaterial"));//de usar este creo que debo crear un obtener un  material y ponerselo, este tiene el pivot en el centro de la esfera
        if (SphereMaterialAsset.Succeeded()) {
            NodoMesh->SetMaterial(0, SphereMaterialAsset.Object);
        }
        NodoMesh->SetWorldScale3D(FVector(2 * Radio / 100));//0.06f//este valor se debe calcular en base al radio,  y escalas, esta funcoin toma el diametro, por lo tnto seria algo como 2*radio/100
    }


}
