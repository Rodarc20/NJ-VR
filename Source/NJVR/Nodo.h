// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Nodo.generated.h"

UCLASS()
class NJVR_API ANodo : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANodo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    //este conjunto de variables deberia ser privados,
    //id del vertice
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    int Id;
    
    //verdadero si es un nodo real, falso si es un nodo virtual
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    bool Valid;

    //la corrdenada x
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    float Xcoordinate;//esta es la coordenada original obtenida del xml, es posible que esto se mapee en otro eje, o en otra posición

    //la coordenada y
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    float Ycoordinate;

    //el path del archivo
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    FString Url;

    //el color que tiene
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    float Color;

    //los labels como titulo del documento, y nombre del archivo
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    TArray<FString> Labels;

    //id del padre
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    int ParentId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    ANodo * Parent;

    //id de los hijos
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    TArray<int> SonsId;//quiza esto despues se pueda convertir en puntero, para que no necesite acceder al padre u otra cosa

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    TArray<ANodo *> Sons;

    //eata es la clase base, no deberia tener componentes importantes aun

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    float Escala;

    //radio de representacion visula del nodo
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    float Radio;

    //Mesh del nodo
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    UStaticMeshComponent * NodoMesh; // debe tener este componente ya que en cualquier tipo de nodo, podre cambiar el color del mesh

    UPROPERTY(VisibleAnywhere, Category = "Visualization")
    USphereComponent * NodoCollision;// el detector de colisiones es diferente en cada nodo por lo tanto se manejan con funciones diferentes, este deberia ser un UShape para poder generalizar, pero por ahora lo dejameros a decision de las subclases

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization", Meta = (BlueprintPublic = "true"))
    UMaterialInstanceDynamic * NodoMaterialDynamic;
    
    //Cambiar el color del material
    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void CambiarColor(FLinearColor NewColor);
};
