// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRVisualization.h"
#include "SemiEsferaVRVisualization.generated.h"

/**
 * 
 */
UCLASS()
class NJVR_API ASemiEsferaVRVisualization : public AVRVisualization
{
	GENERATED_BODY()
public:
    ASemiEsferaVRVisualization();

protected:
    virtual void BeginPlay() override;

public:
    virtual void CreateNodos() override;

    virtual void CreateAristas() override;

    virtual void AplicarTraslacion(FVector Traslacion) override;
	
    virtual void AplicarRotacionRelativaANodo(ANodo* NodoReferencia, FVector PuntoReferencia) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    float distanciaMaxima;
	
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision)
    USphereComponent * Limit;
	
};
