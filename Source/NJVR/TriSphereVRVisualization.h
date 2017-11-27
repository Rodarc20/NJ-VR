// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include "CoreMinimal.h"
#include "VRVisualization.h"
#include "TriSphereVRVisualization.generated.h"

/**
 * 
 */

class Triangulo {
    public:
        int IdA;
        int IdB;
        int IdC;
        int Nivel;
        bool Orientacion;// 0 ahcia arriga, 1 hacia abajo, esto no se si lo use
        FVector Centro;
        FVector A;
        FVector B;
        FVector C;
        float Phi;
        float Theta;
        Triangulo() {}
        Triangulo(int a, int b, int c, int niv): IdA(a), IdB(b), IdC(c), Nivel(niv) { }
        Triangulo(FVector va, FVector vb, FVector  vc, int niv, bool ori): A(va), B(vb), C(vc), Nivel(niv), Orientacion(ori) { }
};

class Region{
    public:
        std::vector<int> IdTriangulos;
        int IdTrianguloCentral;//no se si del vector de triangulos o del vector de idtriangulos de la region
};

UCLASS()
class NJVR_API ATriSphereVRVisualization : public AVRVisualization
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
    ATriSphereVRVisualization();

    virtual void CreateNodos() override;

    virtual void CreateAristas() override;

    std::vector<FVector> Vertices;
    std::vector<FVector> VerticesP;// phi, theta, radio

    std::vector<Triangulo> Triangulos;

    std::vector<Region> Regiones;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float Radio;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float PhiMax;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float PhiMin;

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void LayoutTD();

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void LayoutBU();

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void LayoutCasillas(float NewRadio);
	
    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Calculos(ANodo * V);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Calculos2();

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Calculos3(int & hojas, int & nivelMax);//el calculo de layout2

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Calculos4(int & hojas, int & nivelMax);

    void Calc();

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void ActualizarLayout();
	
    void GenerarIcosaedro(int Precision);

    void CalcularCentro(int IdTriangulo);
    
    void CalcularCentroTriangulo(Triangulo & t);

    FVector PuntoTFromAToB(FVector a, FVector b, float t);

    FVector PuntoTFromAToBEsferico(FVector a, FVector b, float t);

    void DividirTriangulo(Triangulo t);

    void DividirTriangulos();

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void DibujarVertices();
	
    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void DibujarTriangulos();

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void OrdenarTriangulos();

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void LlenarVectorsConIdTriangulos();

};
