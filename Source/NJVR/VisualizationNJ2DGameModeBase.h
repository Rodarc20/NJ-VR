// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "XmlParser.h"
#include "GameFramework/GameModeBase.h"
#include "VisualizationNJ2DGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class NJVR_API AVisualizationNJ2DGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
	
public: 
    AVisualizationNJ2DGameModeBase();

    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    TSubclassOf<class ANodo> TipoNodo;//esto no es practio llenarlo en el cosntructor, cuando esta clase pase a bluprint sera mejor

    //el tipo de nodo que se instancia
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    TSubclassOf<class AArista> TipoArista;//esto no es practio llenarlo en el cosntructor, cuando esta clase pase a bluprint sera mejor

    TArray<class ANodo*> Nodos;

    TArray<class AArista*> Aristas;
	

    FXmlFile XmlSource;// o podria tener un puntero e ir genereando nuevos FXmlFile, todo debepnde, eso por que el contructor podria recibir el path, al ser creado, 
	
    void LoadNodos();//solo esto por que solo me interesa leer la informacion de los vertex, para saber quienes son hijos y padres, por eso tal vez no se trabaje como unity o si, probar
    void CreateNodos();
    void CreateAristas();
	
	
};
