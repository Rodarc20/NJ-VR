// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Arista.h"
#include "Components/CapsuleComponent.h"
#include "AristaCilindroPM.generated.h"

/**
 * Esta clase interacuta con la clase nodo para ver si la arista debe modificarse para unir correctamente en caso de traslado y demas.
 * Por ello solo en caso que dectete que nodo ha cambiar , yo ordeno la actualizacion de mis componentes, en este caso el mesh se administra a traves de vrvirsualization, debo mandarle mi id para que me actualice, de pronto tambien le mando los FVector
 */
UCLASS()
class NJVR_API AAristaCilindroPM : public AArista
{
	GENERATED_BODY()
	
public:
    AAristaCilindroPM();

    virtual void Actualizar() override;

    virtual void ActualizarCollision() override;
	
    //Para detectar colisiones en caso de interactuar con aristas
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    UCapsuleComponent * AristaCollision;
	
	
	
};
