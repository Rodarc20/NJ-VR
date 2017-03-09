// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Visualization.generated.h"

UCLASS()
class NJVR_API AVisualization : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVisualization();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    TSubclassOf<class ANodo> TipoNodo;//esto no es practio llenarlo en el cosntructor, cuando esta clase pase a bluprint sera mejor

    //el tipo de nodo que se instancia
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    TSubclassOf<class AArista> TipoArista;//esto no es practio llenarlo en el cosntructor, cuando esta clase pase a bluprint sera mejor

    TArray<class ANodo*> Vertices;
    TArray<class AArista*> Edges;
	
	
};
