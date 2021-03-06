// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VRVisualization.h"
#include "H3VRVisualization.generated.h"

/**
 * 
 */
UCLASS()
class NJVR_API AH3VRVisualization : public AVRVisualization
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
    AH3VRVisualization();

    virtual void CreateNodos() override;

    virtual void CreateAristas() override;
	
    virtual void AplicarTraslacion(FVector Traslacion) override;
	
    virtual void TraslacionConNodoGuia() override;

    virtual void TrasladarRamaPressed() override;

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

    void CalcularRadioHemiesfera(ANodo * V);

    void CalcularRadioHemiesferaH3(ANodo * V);

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void LayoutBase();

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void LayoutBase2();

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void LayoutBaseH3();

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void ActualizarLayout();

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void ActualizarLayoutH3();

    FMatrix MatrizTraslacion(float x, float y, float z);

    FMatrix MatrizRotacionX(float angle);

    FMatrix MatrizRotacionY(float angle);

    FMatrix MatrizRotacionZ(float angle);

    FMatrix MultiplicacionMatriz(FMatrix a, FMatrix b);

    void ImprimirMatriz(FMatrix m);

    float ConvertirADistanciaEuclideana(float x);
	
    bool IsFinite(FMatrix M);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float K;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float HemisphereAreaScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float LeafArea;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float EscalaSalida;

    float CalcularDeltaPhi(float RadioHijo, float RadioPadre);

	
};
