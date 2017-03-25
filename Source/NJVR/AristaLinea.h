// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Arista.h"
#include "AristaLinea.generated.h"

/**
 * 
 */
UCLASS()
class NJVR_API AAristaLinea : public AArista
{
	GENERATED_BODY()
	
public:
    AAristaLinea();

    virtual void Actualizar() override;

    virtual void ActualizarCollision() override;
	
    virtual void Tick(float DeltaTime) override;
	
};
