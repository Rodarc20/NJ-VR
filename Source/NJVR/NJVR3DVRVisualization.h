// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VRVisualization.h"
#include "NJVR3DVRVisualization.generated.h"

/**
 * 
 */
UCLASS()
class NJVR_API ANJVR3DVRVisualization : public AVRVisualization
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
    ANJVR3DVRVisualization();

    virtual void CreateNodos() override;

    virtual void CreateAristas() override;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float RadioHoja;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float AreaHoja;

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Calculos(ANodo * V);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Calculos2();

    void Calc();

    void CalcularRadio(ANodo * V);

    void CalcularRadioMin(ANodo * V);

    void CalcularRadioHemiesfera(ANodo * V);

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void LayoutBase();

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void LayoutDistanciaReducida();

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void LayoutDistanciaAumentada();

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void LayoutDistanciaAumentadaAnguloReducido();

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void LayoutDistanciaAumentadaHijoAnguloReducido();

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void ActualizarLayout();

    FMatrix MatrizTraslacion(float x, float y, float z);

    FMatrix MatrizRotacionX(float angle);

    FMatrix MatrizRotacionY(float angle);

    FMatrix MatrizRotacionZ(float angle);

    FMatrix MultiplicacionMatriz(FMatrix a, FMatrix b);

    void ImprimirMatriz(FMatrix m);
	
	
	
	
	
};
