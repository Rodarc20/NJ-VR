// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Nodo.h"
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

    //UPROPERTY(VisibleAnywhere, Category = "Visualization")
    //USphereComponent * NodoCollision;
	
	
};
