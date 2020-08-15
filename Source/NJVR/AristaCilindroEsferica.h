// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Arista.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AristaCilindroEsferica.generated.h"

/**
 * 
 */
UCLASS()
class NJVR_API AAristaCilindroEsferica : public AArista
{
	GENERATED_BODY()
public:
    AAristaCilindroEsferica();

    virtual void Actualizar() override;

    virtual void ActualizarCollision() override;
	
    //virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    UCapsuleComponent * AristaCollision;//este funciona bien a falta de un cilindro

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    UStaticMeshComponent * AristaInicioMesh;//un capsule mesh es dificl de scalar de forma adecuada para que las semiesferas no se deformen
	
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    UStaticMeshComponent * AristaMedioMesh;//un capsule mesh es dificl de scalar de forma adecuada para que las semiesferas no se deformen
	
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    UStaticMeshComponent * AristaFinMesh;//un capsule mesh es dificl de scalar de forma adecuada para que las semiesferas no se deformen
	
};
