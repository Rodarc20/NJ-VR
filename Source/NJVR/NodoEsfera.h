// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nodo.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NodoEsfera.generated.h"

/**
 * 
 */
UCLASS()
class NJVR_API ANodoEsfera : public ANodo
{
	GENERATED_BODY()
	
public:
    ANodoEsfera();

    //Mesh del nodo
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    UStaticMeshComponent * NodoMesh; // debe tener este componente ya que en cualquier tipo de nodo, podre cambiar el color del mesh

    UPROPERTY(VisibleAnywhere, Category = "Visualization")
    USphereComponent * NodoCollision;// el detector de colisiones es diferente en cada nodo por lo tanto se manejan con funciones diferentes, este deberia ser un UShape para poder generalizar, pero por ahora lo dejameros a decision de las subclases

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization", Meta = (BlueprintPublic = "true"))
    UMaterialInstanceDynamic * NodoMaterialDynamic;

    virtual void Actualizar() override;

    virtual void MostrarNombre() override;

    virtual void OcultarNombre() override;

    virtual void MostrarNumero() override;

    virtual void OcultarNumero() override;

    virtual void MostrarContenido() override;

    virtual void OcultarContenido() override;

    virtual void ActivarResaltado() override;

    virtual void DesactivarResaltado() override;

    virtual void ActivarResaltadoContenido() override;

    virtual void DesactivarResaltadoContenido() override;

    virtual void DeterminarResaltado() override;

    virtual void CambiarColor(FLinearColor NewColor) override;

    virtual void ActualizarRotacionNombre(FVector Direccion) override;
	
	
};
