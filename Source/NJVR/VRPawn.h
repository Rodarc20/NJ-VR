// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "HeadMountedDisplay.h"
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
    //Root component
    UPROPERTY(VisibleAnywhere)
    USceneComponent * DefaultSceneRoot;
    //Camara del HMD
    UPROPERTY(VisibleAnywhere)
    UCameraComponent * VRCamera;
    
    UPROPERTY(VisibleAnywhere)
    class UMotionControllerComponent * MotionControllerLeft;
    UPROPERTY(VisibleAnywhere)
    class UMotionControllerComponent * MotionControllerRight;
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent * ViveControllerLeft;
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent * ViveControllerRight;
};


//si no funciona lo del enable HMD, el blueprint que herda deesta clase peude hacer eso, y aqui planetar el resto de funcionalidad
