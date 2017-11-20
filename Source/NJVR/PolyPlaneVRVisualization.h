// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VRVisualization.h"
#include "PolyPlaneVRVisualization.generated.h"

/**
 * 
 */
UCLASS()
class NJVR_API APolyPlaneVRVisualization : public AVRVisualization
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
    APolyPlaneVRVisualization();

    virtual void CreateNodos() override;

    virtual void CreateAristas() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float Radio;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float PhiMax;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float PhiMin;

    //int mod(int a, int b);

    //float modFloat(float a, float b);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float DistanciaEntreNiveles;

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void Layout();

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void LayoutSubPlanoVertical(ANodo * Node, float PhiInicioInicial, float PhiTamInicial, float Theta);

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void LayoutSubPlanoHorizontal(ANodo * Node, float ThetaInicioInicial, float ThetaTamInicial, float Phi);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Calculos(ANodo * V);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Calculos2();

    void Calc();

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void ActualizarLayout();
	
	
	
};
