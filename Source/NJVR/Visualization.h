// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "XmlParser.h"
#include "GameFramework/Actor.h"
#include "Visualization.generated.h"


UENUM(BlueprintType)
enum class EVisualizationTask : uint8 {
    ESelectionTask UMETA(DisplayName = "Seleccionar"),
    ETraslationTask UMETA(DisplayName = "Trasladar"),
    ERotationTask UMETA(DisplayName = "Rotar"),
    EVisualizationTask UMETA(DisplayName = "Visualizar"),
    ENoTask UMETA(DisplayName = "Ninguno")
};

UENUM(BlueprintType)//para que nos permita usarlo en blueprint, en el blueprint del HUD
enum class EVisualizationMode : uint8 {//al parecer estos estados son predefinidos
    EAll UMETA(DisplayName = "Todo"),
    EIndividual UMETA(DisplayName = "Nodo por nodo"),
    EBranch UMETA(DisplayName = "Rama"),
    EArea UMETA(DisplayName = "Area"),
    ENoSelection UMETA(DisplayName = "No seleccion")//quiza este debe estar en el otro modo, cuado quiere ver lo de editar, debe haber un modo seleecion traslacion rotar, editar, y un ninguno, que seria este, por ahora lo usare como  es, pero despues este representara el deseleccionar todo lo que haya
};//estado por defecto cuando algunas cosas no se han establecido aun

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
    class UMotionControllerComponent* RightController;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    class UMotionControllerComponent* LeftController;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    bool bHitNodo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    bool bHitBoard;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    FVector ImpactPoint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    AActor* HitActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    TSubclassOf<class ANodo> TipoNodo;//esto no es practio llenarlo en el cosntructor, cuando esta clase pase a bluprint sera mejor

    //el tipo de nodo que se instancia
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    TSubclassOf<class AArista> TipoArista;//esto no es practio llenarlo en el cosntructor, cuando esta clase pase a bluprint sera mejor

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    TArray<class ANodo*> Nodos;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    TArray<class AArista*> Aristas;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    TArray<class ANodo*> NodosSeleccionados;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    bool bNodoGuia;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    ANodo* NodoGuia;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    FLinearColor ColorSeleccion;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    FLinearColor ColorVirtual;

    FXmlFile XmlSource;// o podria tener un puntero e ir genereando nuevos FXmlFile, todo debepnde, eso por que el contructor podria recibir el path, al ser creado, 
	
    void LoadNodos();//solo esto por que solo me interesa leer la informacion de los vertex, para saber quienes son hijos y padres, por eso tal vez no se trabaje como unity o si, probar
    void CreateNodos();
    void CreateAristas();

    //Task
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    EVisualizationTask CurrentVisualizationTask;
	
    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void SetVisualizationTask(EVisualizationTask NewVisualizationTask);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    EVisualizationTask GetVisualizationTask();

    //Mode
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    EVisualizationMode CurrentVisualizationMode;

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void SetVisualizationMode(EVisualizationMode NewVisualizationMode);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    EVisualizationMode GetVisualizationMode();

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void SeleccionarNodo(ANodo * NodoSeleccionado);


    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void DeseleccionarNodo(ANodo * NodoSeleccionado);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void SeleccionarRama(ANodo * NodoSeleccionado);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void DeseleccionarRama(ANodo * NodoSeleccionado);
    
    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void SeleccionarTodo();

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void DeseleccionarTodo();

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void AplicarTraslacion(FVector Traslacion);

    //Para el modo general
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    float DistanciaLaser;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    FVector OffsetToPointLaser;
    /*UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    UStaticMeshComponent* Limite;*/
    //por alguna razón no funciona cuando creo esto aqui, pero funciona bien si lo creo en el blueprint
    //quiza es que no puedo asignar lo de los perfiles bien
};

//por ahora usar los nodos blueprint, ya que estos pueden hacer el cambio de color de forma adecuada, pero usar la variable MaterialDynaimc para almacenar la referencai, y no promover variables


//tanto nodos como arista coo limite estan ignorando todo
//pero deberian tener overlaps entre elllos, almenos nodos y aristas con sigo mismos
//eto deberéactivarlo de acuerdo a la necesidad o al momento en que lo necesite, y desactivarlo cuadno no lo necesite