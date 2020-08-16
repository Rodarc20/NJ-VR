// Fill out your copyright notice in the Description page of Project Settings.

#include "NodoEsferaPM.h"
#include "NJVR.h"
#include "Blueprint/UserWidget.h" 


ANodoEsferaPM::ANodoEsferaPM() {
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    //PrimaryActorTick.bCanEverTick = true;

    Escala = 1.0f;
    Radio = 3.0f;

    NodoCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    RootComponent = NodoCollision;
    NodoCollision->SetCollisionProfileName(FName(TEXT("Nodo")));
    NodoCollision->InitSphereRadius(Radio);//tener cuidado con las unidades, tiene radio 6 quiza sea muy grande
    

    Nombre = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponent"));
    Nombre->SetupAttachment(RootComponent);
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

void ANodoEsferaPM::Actualizar() {
    NodoCollision->SetSphereRadius(Radio*Escala);//esta linea podria ser costosa, sacarla afuera solo deberia ejecutarse cuando se suele el boton de la escala, creo que deberia teer dos funcinces de acutalizacion
    CambiarColor(Color);
    if (!Valid) {
        EfectoResaltado->SetRelativeScale3D(FVector(0.7));
    }
}

void ANodoEsferaPM::MostrarNombre() {
    Nombre->SetVisibility(true);
}

void ANodoEsferaPM::OcultarNombre() {
    Nombre->SetVisibility(false);
}

void ANodoEsferaPM::MostrarNumero() {
    Numero->SetVisibility(true);
}

void ANodoEsferaPM::OcultarNumero() {
    Numero->SetVisibility(false);
}

void ANodoEsferaPM::MostrarContenido() {
    Contenido->SetVisibility(true);
    ActivarResaltadoContenido();
    LineaContenido->SetVisibility(true);
    LineaContenido->ActivateSystem();
    //aqui deberia aplicar lo de cambiar texto y demas
}

void ANodoEsferaPM::OcultarContenido() {
    Contenido->SetVisibility(false);
    DesactivarResaltadoContenido();
    LineaContenido->SetVisibility(false);
    LineaContenido->DeactivateSystem();
}

void ANodoEsferaPM::ActivarResaltado() {
    EfectoResaltado->SetVisibility(true);
    EfectoResaltado->ActivateSystem();
}

void ANodoEsferaPM::DesactivarResaltado() {
    EfectoResaltado->SetVisibility(false);
    EfectoResaltado->DeactivateSystem();
}

void ANodoEsferaPM::ActivarResaltadoContenido() {
    EfectoResaltadoContenido->SetVisibility(true);
    EfectoResaltadoContenido->ActivateSystem();
}

void ANodoEsferaPM::DesactivarResaltadoContenido() {
    EfectoResaltadoContenido->SetVisibility(false);
    EfectoResaltadoContenido->DeactivateSystem();
}

void ANodoEsferaPM::DeterminarResaltado() {
}

void ANodoEsferaPM::CambiarColor(FLinearColor NewColor) {
}

void ANodoEsferaPM::ActualizarRotacionNombre(FVector Direccion) {//deberia actualizarse solo, cuando este activo por alguna razon
    //mejorar esta funcion, posiblemente no funcione bien
    //buscar cuando es que se llama a esta cosa!!!!
    FRotator NewRotation = FRotationMatrix::MakeFromX(Direccion - NodoCollision->GetComponentLocation()).Rotator();
    NodoCollision->SetWorldRotation(NewRotation);
    NewRotation = FRotationMatrix::MakeFromX(Direccion - Nombre->GetComponentLocation()).Rotator();
    //NewRotation = FRotationMatrix::MakeFromX(Direccion).Rotator();
    Nombre->SetWorldRotation(NewRotation);
}




