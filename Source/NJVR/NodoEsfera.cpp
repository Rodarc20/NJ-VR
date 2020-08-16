// Fill out your copyright notice in the Description page of Project Settings.

#include "NodoEsfera.h"
#include "NJVR.h"
#include "Blueprint/UserWidget.h" 


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
    NodoCollision->SetCollisionProfileName(FName(TEXT("Nodo")));
    NodoCollision->InitSphereRadius(Radio);//tener cuidado con las unidades, tiene radio 6 quiza sea muy grande
    
    NodoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereStaticMeshComponent"));
    NodoMesh->SetupAttachment(RootComponent);
    NodoMesh->SetCollisionProfileName(FName(TEXT("Nodo")));
    //RootComponent = NodoMesh;
    //static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Engine/BasicShapes/Sphere"));//de usar este creo que debo crear un obtener un  material y ponerselo, este tiene el pivot en el centro de la esfera
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("StaticMesh'/Game/Visualization/Mesh/NodoPruena1.NodoPruena1'"));//de usar este creo que debo crear un obtener un  material y ponerselo, este tiene el pivot en el centro de la esfera
    if (SphereMeshAsset.Succeeded()) {
        NodoMesh->SetStaticMesh(SphereMeshAsset.Object);//este objeto tiene el pivot en la parte inferior
        //esto solo para cuando use los baisc de engine, si uso los del starter contento no
        static ConstructorHelpers::FObjectFinder<UMaterial> SphereMaterialAsset(TEXT("Material'/Game/Visualization/Materials/NodeValidMaterial.NodeValidMaterial'"));//de usar este creo que debo crear un obtener un  material y ponerselo, este tiene el pivot en el centro de la esfera
        if (SphereMaterialAsset.Succeeded()) {
            NodoMesh->SetMaterial(0, SphereMaterialAsset.Object);
        }
        NodoMesh->SetWorldScale3D(FVector(Radio / 2 * Escala));//0.06f//este valor se debe calcular en base al radio,  y escalas, esta funcoin toma el diametro, por lo tnto seria algo como 2*radio/100
        //NodoMesh->SetWorldScale3D(FVector(2 * Radio / 100 * Escala));//0.06f//este valor se debe calcular en base al radio,  y escalas, esta funcoin toma el diametro, por lo tnto seria algo como 2*radio/100
    }

    Nombre = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponent"));
    Nombre->SetupAttachment(NodoMesh);
    Nombre->SetTextRenderColor(FColor::Black);
    Nombre->SetWorldSize(10.0f*Escala);
    Nombre->SetVisibility(false);
    //Nombre->AddRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
    //Nombre->RelativeRotation = FRotator(0.0f, 0.0f, 180.0f);

    Numero = CreateDefaultSubobject<UTextRenderComponent>(TEXT("NumeroTextRenderComponent"));
    Numero->SetupAttachment(RootComponent);
    Numero->SetTextRenderColor(FColor::Black);
    Numero->SetWorldSize(5.0f*Escala);
    Numero->SetVisibility(false);

    //static ConstructorHelpers::FClassFinder<UUserWidget> ContenidoClass(TEXT("WidgetBlueprintGeneratedClass'/Game/Visualization/Blueprints/Menu/ControlMenu2VR.ControlMenu2VR_C'"));
    //static ConstructorHelpers::FClassFinder<UUserWidget> ContenidoClass(TEXT("WidgetBlueprint'/Game/Visualization/Blueprints/Menu/ContenidoNodo.ContenidoNodo'"));
    PuntoReferenciaContenido = CreateDefaultSubobject<USceneComponent>(TEXT("PuntoReferenciaContenido"));
    PuntoReferenciaContenido->SetupAttachment(RootComponent);
    PuntoReferenciaContenido->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
    PuntoReferenciaContenido->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

    static ConstructorHelpers::FClassFinder<UUserWidget> ContenidoClass(TEXT("WidgetBlueprintGeneratedClass'/Game/Visualization/Blueprints/Menu/ContenidoNodo.ContenidoNodo_C'"));
    Contenido = CreateDefaultSubobject<UWidgetComponent>(TEXT("Contenido"));
    Contenido->SetWidgetSpace(EWidgetSpace::World);
    Contenido->SetupAttachment(PuntoReferenciaContenido);
    //modificar las posiciones
    Contenido->SetRelativeLocation(FVector(20.0f, -30.0f, 0.0f));
    Contenido->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
    Contenido->SetRelativeScale3D(FVector(0.10f, 0.10f, 0.10f));
    if (ContenidoClass.Succeeded()) {
        Contenido->SetWidgetClass(ContenidoClass.Class);
    }
    Contenido->SetDrawSize(FVector2D(425.0f, 250.0f));
    Contenido->SetPivot(FVector2D(1.0f, 0.5f));
    Contenido->SetVisibility(false);

    EfectoResaltado = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EfectoResaltado"));
    EfectoResaltado->SetupAttachment(RootComponent);
    //static ConstructorHelpers::FObjectFinder<UParticleSystem> EfectoResaltadoAsset(TEXT("ParticleSystem'/Game/Visualization/ParticleSystems/LaserImpact/LaserImpactRotacion.LaserImpactRotacion'"));
    static ConstructorHelpers::FObjectFinder<UParticleSystem> EfectoResaltadoAsset(TEXT("ParticleSystem'/Game/Visualization/ParticleSystems/LaserImpact/NodoImpactRotacion.NodoImpactRotacion'"));
    //static ConstructorHelpers::FObjectFinder<UParticleSystem> EfectoResaltadoAsset(TEXT("ParticleSystem'/Game/Visualization/ParticleSystems/LaserImpact/NodoVImpactRotacion.NodoVImpactRotacion'"));
    if (EfectoResaltadoAsset.Succeeded()) {
        EfectoResaltado->SetTemplate(EfectoResaltadoAsset.Object);
    }
    EfectoResaltado->SetRelativeLocation(FVector::ZeroVector);
    EfectoResaltado->bAutoActivate = false;

    EfectoResaltadoContenido = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EfectoResaltadoContenido"));
    EfectoResaltadoContenido->SetupAttachment(RootComponent);
    //static ConstructorHelpers::FObjectFinder<UParticleSystem> EfectoResaltadoAsset(TEXT("ParticleSystem'/Game/Visualization/ParticleSystems/LaserImpact/LaserImpactRotacion.LaserImpactRotacion'"));
    static ConstructorHelpers::FObjectFinder<UParticleSystem> EfectoResaltadoContenidoAsset(TEXT("ParticleSystem'/Game/Visualization/ParticleSystems/LaserImpact/NodoImpactColor.NodoImpactColor'"));
    //static ConstructorHelpers::FObjectFinder<UParticleSystem> EfectoResaltadoAsset(TEXT("ParticleSystem'/Game/Visualization/ParticleSystems/LaserImpact/NodoVImpactRotacion.NodoVImpactRotacion'"));
    if (EfectoResaltadoContenidoAsset.Succeeded()) {
        EfectoResaltadoContenido->SetTemplate(EfectoResaltadoContenidoAsset.Object);
    }
    EfectoResaltadoContenido->SetRelativeLocation(FVector::ZeroVector);
    EfectoResaltadoContenido->bAutoActivate = false;

    LineaContenido = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LineaContenido"));
    LineaContenido->SetupAttachment(RootComponent);
    //static ConstructorHelpers::FObjectFinder<UParticleSystem> EfectoResaltadoAsset(TEXT("ParticleSystem'/Game/Visualization/ParticleSystems/LaserImpact/LaserImpactRotacion.LaserImpactRotacion'"));
    static ConstructorHelpers::FObjectFinder<UParticleSystem> LineaContenidoAsset(TEXT("ParticleSystem'/Game/Visualization/ParticleSystems/LaserBeam/LineaNegro.LineaNegro'"));
    //static ConstructorHelpers::FObjectFinder<UParticleSystem> EfectoResaltadoAsset(TEXT("ParticleSystem'/Game/Visualization/ParticleSystems/LaserImpact/NodoVImpactRotacion.NodoVImpactRotacion'"));
    if (LineaContenidoAsset.Succeeded()) {
        LineaContenido->SetTemplate(LineaContenidoAsset.Object);
    }
    LineaContenido->SetRelativeLocation(FVector::ZeroVector);
    LineaContenido->bAutoActivate = false;
}

void ANodoEsfera::Actualizar() {
    NodoCollision->SetSphereRadius(Radio*Escala);//esta linea podria ser costosa, sacarla afuera solo deberia ejecutarse cuando se suele el boton de la escala, creo que deberia teer dos funcinces de acutalizacion
    NodoMesh->SetWorldScale3D(FVector(Radio/2 * Escala));
    //NodoMesh->SetWorldScale3D(FVector(2 * Radio / 100 * Escala));
    CambiarColor(Color);
    if (!Valid) {
        EfectoResaltado->SetRelativeScale3D(FVector(0.7));
    }
}

void ANodoEsfera::MostrarNombre() {
    Nombre->SetVisibility(true);
}

void ANodoEsfera::OcultarNombre() {
    Nombre->SetVisibility(false);
}

void ANodoEsfera::MostrarNumero() {
    Numero->SetVisibility(true);
}

void ANodoEsfera::OcultarNumero() {
    Numero->SetVisibility(false);
}

void ANodoEsfera::MostrarContenido() {
    Contenido->SetVisibility(true);
    ActivarResaltadoContenido();
    LineaContenido->SetVisibility(true);
    LineaContenido->ActivateSystem();
    //aqui deberia aplicar lo de cambiar texto y demas
}

void ANodoEsfera::OcultarContenido() {
    Contenido->SetVisibility(false);
    DesactivarResaltadoContenido();
    LineaContenido->SetVisibility(false);
    LineaContenido->DeactivateSystem();
}

void ANodoEsfera::ActivarResaltado() {
    EfectoResaltado->SetVisibility(true);
    EfectoResaltado->ActivateSystem();
}

void ANodoEsfera::DesactivarResaltado() {
    EfectoResaltado->SetVisibility(false);
    EfectoResaltado->DeactivateSystem();
}

void ANodoEsfera::ActivarResaltadoContenido() {
    EfectoResaltadoContenido->SetVisibility(true);
    EfectoResaltadoContenido->ActivateSystem();
}

void ANodoEsfera::DesactivarResaltadoContenido() {
    EfectoResaltadoContenido->SetVisibility(false);
    EfectoResaltadoContenido->DeactivateSystem();
}

void ANodoEsfera::DeterminarResaltado() {
}

void ANodoEsfera::CambiarColor(FLinearColor NewColor) {
    if (NodoMaterialDynamic != nullptr) {
        NodoMaterialDynamic->SetVectorParameterValue(TEXT("Color"), NewColor);// a que pareametro del material se le asiganara el nuevo color
    }
}

void ANodoEsfera::ActualizarRotacionNombre(FVector Direccion) {//deberia actualizarse solo, cuando este activo por alguna razon
    FRotator NewRotation = FRotationMatrix::MakeFromX(Direccion - NodoMesh->GetComponentLocation()).Rotator();
    NodoMesh->SetWorldRotation(NewRotation);
    NewRotation = FRotationMatrix::MakeFromX(Direccion - Nombre->GetComponentLocation()).Rotator();
    //NewRotation = FRotationMatrix::MakeFromX(Direccion).Rotator();
    Nombre->SetWorldRotation(NewRotation);
}
