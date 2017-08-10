// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRVisualization.h"
#include "Layout2VRVisualization.generated.h"

/**
 * 
 */
UCLASS()
class NJVR_API ALayout2VRVisualization : public AVRVisualization
{
	GENERATED_BODY()
	
public:
    ALayout2VRVisualization();

    virtual void CreateNodos() override;

    virtual void CreateAristas() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    float Rad;

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Layout(float Radio);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Calculos(ANodo * V);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Calculos2(int & hojas, int & nivelMax);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void ActualizarLayout();

    virtual void AplicarTraslacion(FVector Traslacion) override;
	
    virtual void AplicarRotacionRelativaANodo(ANodo* NodoReferencia, FVector PuntoReferencia) override;
	
	
	
};
