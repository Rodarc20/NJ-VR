// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodo.h"
#include "Components/SphereComponent.h"
#include "NodoEsferaPM.generated.h"

/**
 * 
 */
UCLASS()
class NJVR_API ANodoEsferaPM : public ANodo
{
	GENERATED_BODY()
	
public:
    ANodoEsferaPM();

    UPROPERTY(VisibleAnywhere, Category = "Visualization")
    USphereComponent * NodoCollision;// el detector de colisiones es diferente en cada nodo por lo tanto se manejan con funciones diferentes, este deberia ser un UShape para poder generalizar, pero por ahora lo dejameros a decision de las subclases

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
