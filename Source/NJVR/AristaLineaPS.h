// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Arista.h"
#include "AristaLineaPS.generated.h"

/**
 * 
 */
UCLASS()
class NJVR_API AAristaLineaPS : public AArista
{
    GENERATED_BODY()

public:
    AAristaLineaPS();

    virtual void Actualizar() override;

    virtual void ActualizarCollision() override;
	
    virtual void Tick(float DeltaTime) override;
	
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    UParticleSystemComponent * Linea;
	
};
