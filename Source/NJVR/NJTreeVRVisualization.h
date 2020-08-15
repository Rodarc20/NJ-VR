// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VRVisualization.h"
#include "NJTreeVRVisualization.generated.h"

/**
 * 
 */
UCLASS()
class NJVR_API ANJTreeVRVisualization : public AVRVisualization
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
    ANJTreeVRVisualization();

    virtual void CreateNodos() override;

    virtual void CreateAristas() override;
	
    virtual void AplicarTraslacion(FVector Traslacion) override;
	
    virtual void AplicarRotacionRelativaANodo(ANodo* NodoReferencia, FVector PuntoReferencia) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float DistanciaArista;

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void Layout();

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void ActualizarLayout();
	
    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Calculos(ANodo * V);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Calculos2();

    void Calc();

    void RotacionRama() override;

    virtual FVector InterseccionLineaSuperficie() override;

    virtual void TraslacionConNodoGuia() override;


};
