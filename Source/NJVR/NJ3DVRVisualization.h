// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRVisualization.h"
#include "NJ3DVRVisualization.generated.h"

/**
 * 
 */
UCLASS()
class NJVR_API ANJ3DVRVisualization : public AVRVisualization
{
	GENERATED_BODY()
	
public:
    ANJ3DVRVisualization();

    virtual void CreateNodos() override;

    virtual void CreateAristas() override;

    virtual void AplicarTraslacion(FVector Traslacion) override;
	
    virtual void AplicarRotacionRelativaANodo(ANodo* NodoReferencia, FVector PuntoReferencia) override;
	
};
