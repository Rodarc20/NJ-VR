// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRVisualization.h"
#include "NJVRVisualization.generated.h"

/**
 * 
 */
UCLASS()
class NJVR_API ANJVRVisualization : public AVRVisualization
{
	GENERATED_BODY()
	
public:
    ANJVRVisualization();

    virtual void CreateNodos() override;

    virtual void CreateAristas() override;

    virtual void AplicarTraslacion(FVector Traslacion) override;
	
    virtual void AplicarRotacionRelativaANodo(ANodo* NodoReferencia, FVector PuntoReferencia) override;
	
    virtual FVector InterseccionLineaSuperficie() override;

    virtual void TraslacionConNodoGuia() override;

    virtual void RotacionRama() override;
};
