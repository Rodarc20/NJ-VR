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

    //UPROPERTY(VisibleAnywhere, Category = "Visualization")
    //USphereComponent * NodoCollision;
	
	
};
