// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRVisualization.h"
#include "Layout1VRVisualization.generated.h"

/**
 * 
 */
UCLASS()
class NJVR_API ALayout1VRVisualization : public AVRVisualization
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
    ALayout1VRVisualization();

    virtual void CreateNodos() override;

    virtual void CreateAristas() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float Radio;

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Layout(float NewRadio);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Calculos(ANodo * V);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Calculos2();

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void ActualizarLayout();

    virtual void AplicarTraslacion(FVector Traslacion) override;
	
    virtual void AplicarRotacionRelativaANodo(ANodo* NodoReferencia, FVector PuntoReferencia) override;
	
	
};
