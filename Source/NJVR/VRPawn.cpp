// Fill out your copyright notice in the Description page of Project Settings.

#include "NJVR.h"
#include "VRPawn.h"
#include "Blueprint/UserWidget.h" 


// Sets default values
AVRPawn::AVRPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
    RootComponent = DefaultSceneRoot;

    VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
    VRCamera->SetupAttachment(RootComponent);

    MotionControllerLeft = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionControllerLeft"));
    MotionControllerLeft->SetupAttachment(RootComponent);
    MotionControllerLeft->SetRelativeLocation(FVector(50.0f, -40.0f, -20.0f));

    MotionControllerRight = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionControllerRight"));
    MotionControllerRight->SetupAttachment(RootComponent);
    MotionControllerRight->SetRelativeLocation(FVector(50.0f, 40.0f, 0.0f));

    ViveControllerLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ViveControllerLeft"));
    ViveControllerLeft->SetupAttachment(MotionControllerLeft);
    ViveControllerLeft->SetRelativeRotation(FRotator(0.0f, 90.0f, 90.0f));//ver cual es el yaw y el roll

    ViveControllerRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ViveControllerRight"));
    ViveControllerRight->SetupAttachment(MotionControllerRight);
    ViveControllerRight->SetRelativeRotation(FRotator(0.0f, 90.0f, 90.0f));


    static ConstructorHelpers::FObjectFinder<UStaticMesh> ViveControllerMeshAsset(TEXT("StaticMesh'/Game/ControllerSetup/vr_controller_vive_1_5.vr_controller_vive_1_5'"));//de usar este creo que debo crear un obtener un  material y ponerselo, este tiene el pivot en el centro de la esfera
    if (ViveControllerMeshAsset.Succeeded()) {
        ViveControllerLeft->SetStaticMesh(ViveControllerMeshAsset.Object);
        ViveControllerRight->SetStaticMesh(ViveControllerMeshAsset.Object);
        static ConstructorHelpers::FObjectFinder<UMaterial> ViveControllerMaterialAsset(TEXT("Material'/Game/ControllerSetup/ControlMaterial.ControlMaterial'"));//de usar este creo que debo crear un obtener un  material y ponerselo, este tiene el pivot en el centro de la esfera
        if (ViveControllerMaterialAsset.Succeeded()) {
            ViveControllerLeft->SetMaterial(0, ViveControllerMaterialAsset.Object);
            ViveControllerRight->SetMaterial(0, ViveControllerMaterialAsset.Object);
        }
    }

    static ConstructorHelpers::FClassFinder<UUserWidget> MenuClass(TEXT("WidgetBlueprintGeneratedClass'/Game/Visualization/Blueprints/Menu/ControlMenuVR.ControlMenuVR_C'"));
    Menu = CreateDefaultSubobject<UWidgetComponent>(TEXT("Menu"));
    Menu->SetWidgetSpace(EWidgetSpace::World);
    Menu->SetupAttachment(MotionControllerLeft);
    Menu->SetDrawSize(FVector2D(300.0f, 250.0f));
    Menu->SetPivot(FVector2D(0.5f, 1.0f));
    Menu->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
    Menu->SetRelativeRotation(FRotator(50.0f, 180.0f, 0.0f));
    Menu->SetRelativeScale3D(FVector(0.15f, 0.15f, 0.15f));
    if (MenuClass.Succeeded()) {
        Menu->SetWidgetClass(MenuClass.Class);
    }

    Interaction = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("Interaction"));
    Interaction->SetupAttachment(MotionControllerRight);

    static ConstructorHelpers::FClassFinder<UUserWidget> DocumentClass(TEXT("WidgetBlueprintGeneratedClass'/Game/Visualization/Blueprints/Menu/ControlMenu2VR.ControlMenu2VR_C'"));
    Document = CreateDefaultSubobject<UWidgetComponent>(TEXT("Document"));
    Document->SetWidgetSpace(EWidgetSpace::World);
    Document->SetupAttachment(MotionControllerRight);
    Document->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
    Document->SetRelativeRotation(FRotator(0.0f, 1800.0f, 0.0f));
    Document->SetRelativeScale3D(FVector(0.10f, 0.10f, 0.10f));
    if (DocumentClass.Succeeded()) {
        Document->SetWidgetClass(DocumentClass.Class);
    }
    Document->SetDrawSize(FVector2D(425.0f, 250.0f));
    Document->SetPivot(FVector2D(1.0f, 0.5f));
    Document->SetVisibility(false);
    //Document->SetCollisionProfileName("Custom");
    //Document->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    //Document->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);arreglar esto!

    Movimiento = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movimiento"));

    static ConstructorHelpers::FObjectFinder<UParticleSystem> LaserAsset(TEXT("ParticleSystem'/Game/Visualization/ParticleSystems/LaserBeam/LaserAzul.LaserAzul'"));//de usar este creo que debo crear un obtener un  material y ponerselo, este tiene el pivot en el centro de la esfera
    Laser = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Laser"));
    Laser->SetupAttachment(MotionControllerRight);
    if (LaserAsset.Succeeded()) {
        Laser->SetTemplate(LaserAsset.Object);
        Lasers.Add(LaserAsset.Object);
    }

    static ConstructorHelpers::FObjectFinder<UParticleSystem> LasersAsset(TEXT("ParticleSystem'/Game/Visualization/ParticleSystems/LaserBeam/LaserVerde.LaserVerde'"));
    if (LasersAsset.Succeeded()) {
        Lasers.Add(LasersAsset.Object);
    }
    static ConstructorHelpers::FObjectFinder<UParticleSystem> LasersAsset2(TEXT("ParticleSystem'/Game/Visualization/ParticleSystems/LaserBeam/LaserRojo.LaserRojo'"));
    if (LasersAsset2.Succeeded()) {
        Lasers.Add(LasersAsset2.Object);
    }
    static ConstructorHelpers::FObjectFinder<UParticleSystem> LasersAsset3(TEXT("ParticleSystem'/Game/Visualization/ParticleSystems/LaserBeam/LaserAmarillo.LaserAmarillo'"));
    if (LasersAsset3.Succeeded()) {
        Lasers.Add(LasersAsset3.Object);
    }
    static ConstructorHelpers::FObjectFinder<UParticleSystem> LasersAsset4(TEXT("ParticleSystem'/Game/Visualization/ParticleSystems/LaserBeam/LaserTurquesa.LaserTurquesa'"));
    if (LasersAsset4.Succeeded()) {
        Lasers.Add(LasersAsset4.Object);
    }
    static ConstructorHelpers::FObjectFinder<UParticleSystem> LasersAsset5(TEXT("ParticleSystem'/Game/Visualization/ParticleSystems/LaserBeam/LaserNaranja.LaserNaranja'"));
    if (LasersAsset5.Succeeded()) {
        Lasers.Add(LasersAsset5.Object);
    }
    static ConstructorHelpers::FObjectFinder<UParticleSystem> LasersAsset6(TEXT("ParticleSystem'/Game/Visualization/ParticleSystems/LaserBeam/LaserMorado.LaserMorado'"));
    if (LasersAsset6.Succeeded()) {
        Lasers.Add(LasersAsset6.Object);
    }

    Velocidad = 200.0f;
    bPadDerecho = false;
    LaserIndice = 0;
}

// Called when the game starts or when spawned
void AVRPawn::BeginPlay()
{
	Super::BeginPlay();
	
    //UHeadMountedDisplayFunctionLibrary::EnableHMD(true);//esto es lo que hago en el blueprint, deberia funcionar igual, de no hacerlo, se peude sobrescribir en el bluprint despues
    /*if (GEngine) {
        GEngine->HMDDevice->EnableHMD(true);
    }*/
}

// Called every frame
void AVRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    float MX = GetInputAxisValue("MoveForward");
    float MY = GetInputAxisValue("MoveRight");
    float MZ = GetInputAxisValue("MoveUp");
    if (bPadDerecho && (MX != 0.0f || MY != 0.0f || MZ != 0.0f)) {
        FVector Desplazamiento = GetActorForwardVector() * MX + GetActorRightVector() * MY + GetActorUpVector() * MZ;
        SetActorLocation(GetActorLocation() + Desplazamiento.GetSafeNormal() * Velocidad * DeltaTime);
    }

    float CX = GetInputAxisValue("ControllerForward");
    float CY = GetInputAxisValue("ControllerRight");
    float CZ = GetInputAxisValue("ControllerUp");
    if (CX != 0.0f || CY != 0.0f || CZ != 0.0f) {
        FVector Desplazamiento (CX, CY, CZ);
        MotionControllerRight->SetWorldLocation(MotionControllerRight->GetComponentLocation() + Desplazamiento.GetSafeNormal() * Velocidad/2 * DeltaTime);
    }
    float CRoll = GetInputAxisValue("ControllerRoll");
    float CPitch = GetInputAxisValue("ControllerPitch");
    float CYaw = GetInputAxisValue("ControllerYaw");
    if (CRoll != 0.0f || CPitch != 0.0f || CYaw != 0.0f) {
        FRotator Rotacion(CPitch, CYaw, CRoll);
        MotionControllerRight->AddRelativeRotation(Rotacion);
    }
}

// Called to bind functionality to input
void AVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    InputComponent->BindAction("Moverse", IE_Pressed, this, &AVRPawn::PadDerechoPressed);
    InputComponent->BindAction("Moverse", IE_Released, this, &AVRPawn::PadDerechoReleased);
    InputComponent->BindAction("Select", IE_Pressed, this, &AVRPawn::SelectPressed);
    InputComponent->BindAction("Select", IE_Released, this, &AVRPawn::SelectReleased);

    InputComponent->BindAxis("MoveForward");
    InputComponent->BindAxis("MoveRight");
    InputComponent->BindAxis("MoveUp");

    InputComponent->BindAxis("ControllerForward");
    InputComponent->BindAxis("ControllerRight");
    InputComponent->BindAxis("ControllerUp");

    InputComponent->BindAxis("ControllerRoll");
    InputComponent->BindAxis("ControllerPitch");
    InputComponent->BindAxis("ControllerYaw");
}

void AVRPawn::CambiarLaser(int Indice) {
    if (Indice < Lasers.Num()) {
        Laser->SetTemplate(Lasers[Indice]);
    }
}

void AVRPawn::CambiarPuntoFinal(FVector PuntoFinal) {
    if (Interaction->IsOverHitTestVisibleWidget()) {
        FHitResult HitInteraction = Interaction->GetLastHitResult();
        if ((MotionControllerRight->GetComponentLocation() - HitInteraction.ImpactPoint).Size() < (MotionControllerRight->GetComponentLocation() - PuntoFinal).Size()) {
            Laser->SetBeamTargetPoint(0, HitInteraction.ImpactPoint, 0);//o target
        }
    }
    else {
        //Laser->SetBeamEndPoint(0, PuntoFinal);//o target
        Laser->SetBeamTargetPoint(0, PuntoFinal, 0);//o target
        //Laser->SetBeamTargetPoint(0, GetTransform().InverseTransformPosition(PuntoFinal), 0);//o target
    }
}

int AVRPawn::LaserActual() {
    return LaserIndice;
}

void AVRPawn::PadDerechoPressed() {
    bPadDerecho = true;
}

void AVRPawn::PadDerechoReleased() {
    bPadDerecho = false;
}

void AVRPawn::SelectPressed() {
    Interaction->PressPointerKey(EKeys::LeftMouseButton);
}

void AVRPawn::SelectReleased() {
    Interaction->ReleasePointerKey(EKeys::LeftMouseButton);
}
