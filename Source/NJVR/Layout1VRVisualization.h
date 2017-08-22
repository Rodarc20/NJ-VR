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

    int mod(int a, int b);

    float modFloat(float a, float b);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float Radio;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float PhiMax;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float PhiMin;

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Layout(float NewRadio);

    //Ubica el nivel mas denso de todo el arbol en el ecuador
    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Layout3(float NewRadio);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Layout2(ANodo * Node, float NewRadio, int NivelExp, float PhiInicial, float WInicioInicial, float WTamInicial);

    int EncontrarNivel(class TQueue<ANodo*>& cola, ANodo * Rama, int Nivel);//que retorne el numero de hojas en el nivel que retorne

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Calculos(ANodo * V);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Calculos2();

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void ActualizarLayout();

    void Calc();

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void ExpandirLayout();

    virtual void AplicarTraslacion(FVector Traslacion) override;
	
    virtual void AplicarRotacionRelativaANodo(ANodo* NodoReferencia, FVector PuntoReferencia) override;
	
	
};
