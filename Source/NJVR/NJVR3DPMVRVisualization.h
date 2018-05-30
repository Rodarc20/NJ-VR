// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VRVisualization.h"
#include "ProceduralMeshComponent.h"
#include <vector>
#include "NJVR3DPMVRVisualization.generated.h"

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
        Triangulo(int a, int b, int c, int niv, bool ori): IdA(a), IdB(b), IdC(c), Nivel(niv), Orientacion(ori) { }
        Triangulo(FVector va, FVector vb, FVector  vc, int niv, bool ori): A(va), B(vb), C(vc), Nivel(niv), Orientacion(ori) { }
        Triangulo(FVector va, FVector vb, FVector  vc, int a, int b, int c, int niv, bool ori): A(va), B(vb), C(vc), IdA(a), IdB(b), IdC(c), Nivel(niv), Orientacion(ori) { }
};

UCLASS()
class NJVR_API ANJVR3DPMVRVisualization : public AVRVisualization
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
    ANJVR3DPMVRVisualization();

    virtual void CreateNodos() override;

    virtual void CreateAristas() override;

    virtual void AplicarLayout() override;

    virtual void AplicarTraslacion(FVector Traslacion) override;
	
    UFUNCTION(BlueprintCallable, Category = "Visualization")
    virtual void TraslacionConNodoGuia() override;

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    virtual void TrasladarRamaPressed() override;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float RadioHoja;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float DeltaDistanciaArista;

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
	
	
    //ProceduralMesh para los nodos
	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent * NodosMesh;

	TArray<FVector> VerticesNodos;

	TArray<FVector> VerticesPNodos;

	TArray<int32> TrianglesNodos;

	TArray<FVector> NormalsNodos;

	TArray<FVector2D> UV0Nodos;

	TArray<FProcMeshTangent> TangentsNodos;

	TArray<FLinearColor> VertexColorsNodos;

    std::vector<Triangulo> TriangulosNodoTemplate;

    std::vector<FVector> VerticesNodoTemplate;

    std::vector<FVector> VerticesPNodoTemplate;// phi, theta, radio

	std::vector<FVector> NormalsNodoTemplate;

	std::vector<FVector2D> UV0NodoTemplate;

	std::vector<FProcMeshTangent> TangentsNodoTemplate;

	std::vector<FLinearColor> VertexColorsNodoTemplate;

    void CreateSphereTemplate(int Precision);// crea esferas de radio 1

    FVector PuntoTFromAToB(FVector a, FVector b, float t);

    FVector PuntoTFromAToBEsferico(FVector a, FVector b, float t);

    void DividirTriangulo(Triangulo t);

    void DividirTriangulos();

    void AddNodoToMesh(FVector Posicion, float Radio, int NumNodo);//NumNodo es solo para facilidad de calculo

    void AddNodoToMesh(FVector Posicion, float Radio, FLinearColor Color, int NumNodo);

    void CreateNodosMesh();

    void UpdateNodosMesh();

    void UpdatePosicionNodoMesh(int IdNodo, FVector NewPosition);

    //ProceduralMesh para las aristas
	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent * AristasMesh;
	
	TArray<FVector> VerticesAristas;

	TArray<int32> TrianglesAristas;

	TArray<FVector> NormalsAristas;

	TArray<FVector2D> UV0Aristas;

	TArray<FProcMeshTangent> TangentsAristas;

	TArray<FLinearColor> VertexColorsAristas;

};
