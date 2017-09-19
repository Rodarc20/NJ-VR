// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "HeadMountedDisplay.h"
#include "WidgetComponent.h"
#include "WidgetInteractionComponent.h"
#include "MotionControllerComponent.h"
#include "VRPawn.generated.h"

UCLASS()
class NJVR_API AVRPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AVRPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    //analizar si estos parametos deben ser posibles verlosd esde el blueprint
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    class AVRVisualization * Visualization;//esto no es practio llenarlo en el cosntructor, cuando esta clase pase a bluprint sera mejor

    //Root component
    UPROPERTY(VisibleAnywhere, Category = "Visualization")
    USceneComponent * DefaultSceneRoot;
    //Camara del HMD
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    UCameraComponent * VRCamera;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    class UMotionControllerComponent * MotionControllerLeft;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    class UMotionControllerComponent * MotionControllerRight;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    UStaticMeshComponent * ViveControllerLeft;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    UStaticMeshComponent * ViveControllerRight;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    UWidgetComponent * Menu;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    UWidgetComponent * Document;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    UWidgetInteractionComponent * Interaction;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    UParticleSystemComponent * Laser;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    UFloatingPawnMovement * Movimiento;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    TArray<UParticleSystem *> Lasers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    bool bPadDerecho;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    float Velocidad;

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void CambiarLaser(int Indice);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void CambiarPuntoFinal(FVector PuntFinal);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void PadDerechoPressed();

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void PadDerechoReleased();

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void SelectPressed();

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void SelectReleased();
};


//si no funciona lo del enable HMD, el blueprint que herda deesta clase peude hacer eso, y aqui planetar el resto de funcionalidad
