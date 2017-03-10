// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Arista.generated.h"

UCLASS()
class NJVR_API AArista : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AArista();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    //escala de la arista
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    float Escala;

    //Distancia de la arista
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    float Distancia;//la distancia no puede ser menor que el radio, esta es por la capsula la cual sera el cllider

    //radio o grosor de la arista
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    float Radio;

    //id de la arista, nada en particular solo para buscar
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    int Id;

    //id del nodo del que sale esta arista, es decir del padre
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    int Source;

    //id del nodo al que se dirige la arista, es decir el hijo
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    int Target;

    //referencia al nodo padre
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    class ANodo* SourceNodo;

    //referencia al nodo hijo
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
    class ANodo* TargetNodo;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    UCapsuleComponent * AristaCollision;//este funciona bien a falta de un cilindro

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    UStaticMeshComponent * AristaMesh;//un capsule mesh es dificl de scalar de forma adecuada para que las semiesferas no se deformen
	
};
