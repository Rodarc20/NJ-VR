// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Arista.h"
#include "AristaLineaEsferica.generated.h"

/**
 * 
 */
UCLASS()
class NJVR_API AAristaLineaEsferica : public AArista
{
	GENERATED_BODY()
	
public:
    AAristaLineaEsferica();

    virtual void Actualizar() override;

    virtual void ActualizarCollision() override;
	
    virtual void Tick(float DeltaTime) override;
	
	
	
};
