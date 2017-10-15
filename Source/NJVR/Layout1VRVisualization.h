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

    UFUNCTION(BlueprintCallable, Category = "Visualization - Layouts")
    void Layout5(float NewRadio);
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
    UFUNCTION(BlueprintCallable, Category = "Visualization")
    virtual FVector InterseccionLineaSuperficie() override;
	
    UFUNCTION(BlueprintCallable, Category = "Visualization")
    virtual void TraslacionConNodoGuia() override;
	
};

//este layout requiere dos fases una en la que se calcula las coordenadas esfericas, y otra en la que se convierten estas coordenadas al sistenma normal, en funcion a un radio.
//al inlcuir el caso del radio, no se debe recalcular todo, hacer la nuev transformacion con el uevo radio
//otra forma es antes de proceder a las transfomracion, es determinar cual sera el delta para el nivel desno, si ya se conoce el inicio finaldel rango, el nivle que tocara, el delta phi, etc.
//estas dos formas determinar dos estructuras de las funciones bastante distintas, escoger la mas optima, y comoda, quiza se deban quear ambas