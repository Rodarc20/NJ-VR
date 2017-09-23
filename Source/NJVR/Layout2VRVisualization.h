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
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
    ALayout2VRVisualization();

    virtual void CreateNodos() override;

    virtual void CreateAristas() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float Radio;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float PhiMax;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float PhiMin;

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Layout(float NewRadio);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Calculos(ANodo * V);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Calculos2(int & hojas, int & nivelMax);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void ActualizarLayout();

    virtual void AplicarTraslacion(FVector Traslacion) override;
	
    virtual void AplicarRotacionRelativaANodo(ANodo* NodoReferencia, FVector PuntoReferencia) override;
	
    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void AplicarTraslacionEsferica(float TraslacionPhi, float TraslacionTheta);
    //interseccion con del laser con la esfera
    UFUNCTION(BlueprintCallable, Category = "Visualization")
    virtual FVector InterseccionLineaSuperficie() override;
	
    UFUNCTION(BlueprintCallable, Category = "Visualization")
    virtual void TraslacionConNodoGuia() override;

    /*UFUNCTION(BlueprintCallable, Category = "Visualization")
    void BuscandoNodoConLaser();*/

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void TraslacionVisualizacion();

};
