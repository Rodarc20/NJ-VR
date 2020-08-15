// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VRVisualization.h"
#include "ProceduralMeshComponent.h"
#include <vector>
#include "Layout1PMVRVisualization.generated.h"

/**
 * 
 */

class Triangulo {
    public:
        int IdA;
        int IdB;
        int IdC;
        FVector A;
        FVector B;
        FVector C;
        int Nivel;
        bool Orientacion;// 0 ahcia arriga, 1 hacia abajo, esto no se si lo use
        FVector Centro;
        float Phi;
        float Theta;
        Triangulo() {}
        Triangulo(int a, int b, int c, int niv, bool ori): IdA(a), IdB(b), IdC(c), Nivel(niv), Orientacion(ori) { }
        Triangulo(FVector va, FVector vb, FVector  vc, int niv, bool ori): A(va), B(vb), C(vc), Nivel(niv), Orientacion(ori) { }
        Triangulo(FVector va, FVector vb, FVector  vc, int a, int b, int c, int niv, bool ori): IdA(a), IdB(b), IdC(c), A(va), B(vb), C(vc), Nivel(niv), Orientacion(ori) { }
};

UCLASS()
class NJVR_API ALayout1PMVRVisualization : public AVRVisualization
{
	GENERATED_BODY()
	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
    ALayout1PMVRVisualization();

    virtual void CreateNodos() override;

    virtual void CreateAristas() override;

    //int mod(int a, int b);

    //float modFloat(float a, float b);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float Radio;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float PhiMax;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float PhiMin;

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void Layout(float NewRadio);

    //Ubica el nivel mas denso de todo el arbol en el ecuador
    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void Layout3(float NewRadio);

    void UbicacionesLayout4(ANodo * Rama, int NivelDenso);
    float EncontrarRadio1(float PhiNivelDenso, int CantidadNodosNivel);//busca el nivel mas denso y hace un calculo sencillo
    float EncontrarRadio2(float PhiUltimoNivel);//este deberia recibir el phi el ultimo nivel, para poder hacer los calculos, 

    //ubica por ramas el nivel mas denso en la mitad de la esfera
    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void Layout4();

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void Layout2(ANodo * Node, float NewRadio, int NivelExp, float PhiInicial, float WInicioInicial, float WTamInicial);

    int EncontrarNivel(class TQueue<ANodo*>& cola, ANodo * Rama, int Nivel);//que retorne el numero de hojas en el nivel que retorne

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Calculos(ANodo * V);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void Calculos2();

    void Calc();

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void ActualizarLayout();


    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void ExpandirLayout();

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void ReducionDistanciaHojas();

    virtual void AplicarTraslacion(FVector Traslacion) override;
	
    virtual void AplicarRotacionRelativaANodo(ANodo* NodoReferencia, FVector PuntoReferencia) override;
	
    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void AplicarTraslacionEsferica(float TraslacionPhi, float TraslacionTheta);
    //interseccion con del laser con la esfera
    virtual FVector InterseccionLineaSuperficie() override;
	
    virtual void TraslacionConNodoGuia() override;
	
	
    UPROPERTY(EditAnywhere, Category = "Visualization - Parametros")
    int PrecisionNodos; //NumeroLadosArista;//este tambien indica el tamaño de los arreglos

    //ProceduralMesh para los nodos
	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent * NodosMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterial * NodosMeshMaterial;

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

    void AddNodoToMesh(FVector Posicion, float RadioNodo, int NumNodo);//NumNodo es solo para facilidad de calculo

    void AddNodoToMesh(FVector Posicion, float RadioNodo, FLinearColor Color, int NumNodo);

    void CreateNodosMesh();

    void UpdateNodosMesh();

    void UpdatePosicionNodoMesh(int IdNodo, FVector NewPosition);

    //ProceduralMesh para las aristas
	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent * AristasMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterial * AristasMeshMaterial;
	
	TArray<FVector> VerticesAristas;

	TArray<int32> TrianglesAristas;

	TArray<FVector> NormalsAristas;

	TArray<FVector2D> UV0Aristas;

	TArray<FProcMeshTangent> TangentsAristas;

	TArray<FLinearColor> VertexColorsAristas;

    UPROPERTY(EditAnywhere, Category = "Visualization - Parametros")
    int PrecisionAristas; //NumeroLadosArista;//este tambien indica el tamaño de los arreglos

    void AddAristaToMesh(FVector Source, FVector Target, int RadioArista, int NumArista);// precision es cuanto lados tendra el cilindo, minimo 3, radio, sera el radio del cilindro, este template no es tan adaptable como el de la esfera, por eso necesai dos parametros

    void CreateAristasMesh();

    void UpdateAristasMesh();

    void UpdatePosicionesAristaMesh(int IdArista, FVector Source, FVector Target);
};
