// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Arista.h"
#include "AristaCilindro.generated.h"

/**
 * 
 */
UCLASS()
class NJVR_API AAristaCilindro : public AArista
{
    GENERATED_BODY()

public:
    AAristaCilindro();

    virtual void Actualizar() override;

    virtual void ActualizarCollision() override;
	
	
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    UCapsuleComponent * AristaCollision;//este funciona bien a falta de un cilindro

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    UStaticMeshComponent * AristaMesh;//un capsule mesh es dificl de scalar de forma adecuada para que las semiesferas no se deformen
	
};
