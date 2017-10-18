// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VRVisualization.h"
#include "NJTree3DVRVisualization.generated.h"

/**
 * 
 */
UCLASS()
class NJVR_API ANJTree3DVRVisualization : public AVRVisualization
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
    ANJTree3DVRVisualization();

    virtual void CreateNodos() override;

    virtual void CreateAristas() override;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float DistanciaArista;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float PhiValido;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float PhiInvalido;

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void Layout();

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void Layout2();

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void LayoutEsferico();

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void ActualizarLayout();
	
    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Calculos(ANodo * V);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Calculos2();

    void Calc();
	
	
	
};
