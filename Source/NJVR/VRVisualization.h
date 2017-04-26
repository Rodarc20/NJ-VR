// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "XmlParser.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "VRVisualization.generated.h"


UENUM(BlueprintType)
enum class EVRVisualizationTask : uint8 {
    ETraslationTask UMETA(DisplayName = "Trasladar"),
    ERotationTask UMETA(DisplayName = "Rotar"),
    EScaleTask UMETA(DisplayName = "Escalar"),
    EVisualizationTask UMETA(DisplayName = "Visualizar"),
    ENoTask UMETA(DisplayName = "Ninguno")
};

UENUM(BlueprintType)//para que nos permita usarlo en blueprint, en el blueprint del HUD
enum class EVRVisualizationMode : uint8 {//al parecer estos estados son predefinidos
    EAll UMETA(DisplayName = "Todo"),
    ENode UMETA(DisplayName = "Nodo"),
    EBranch UMETA(DisplayName = "Rama"),
    EArea UMETA(DisplayName = "Area"),
    ENoMode UMETA(DisplayName = "Niguno")//quiza este debe estar en el otro modo, cuado quiere ver lo de editar, debe haber un modo seleecion traslacion rotar, editar, y un ninguno, que seria este, por ahora lo usare como  es, pero despues este representara el deseleccionar todo lo que haya
};//estado por defecto cuando algunas cosas no se han establecido aun

UCLASS()
class NJVR_API AVRVisualization : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVRVisualization();

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
    class ANodo* HitNodo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    AActor* HitLimite;

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
    TArray<FLinearColor> Colores;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    FLinearColor ColorSeleccion;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    FLinearColor ColorVirtual;

    FXmlFile XmlSource;// o podria tener un puntero e ir genereando nuevos FXmlFile, todo debepnde, eso por que el contructor podria recibir el path, al ser creado, 
	
    void LoadNodos();//solo esto por que solo me interesa leer la informacion de los vertex, para saber quienes son hijos y padres, por eso tal vez no se trabaje como unity o si, probar
    virtual void CreateNodos();
    virtual void CreateAristas();

    //Task
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    EVRVisualizationTask CurrentVisualizationTask;
	
    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void SetVisualizationTask(EVRVisualizationTask NewVisualizationTask);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    EVRVisualizationTask GetVisualizationTask();

    //Mode
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    EVRVisualizationMode CurrentVisualizationMode;

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void SetVisualizationMode(EVRVisualizationMode NewVisualizationMode);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    EVRVisualizationMode GetVisualizationMode();
	
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
    virtual void AplicarTraslacion(FVector Traslacion);

    //Para el modo general
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    float DistanciaLaser;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    FVector OffsetToPointLaser;


    //Rotacion del controll capturado al presionar el trigger
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    FRotator InitialRotationController;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    FRotator InitialRotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    bool Rotar;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    bool MostrarLabel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    bool MostrarNumero;

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    virtual void AplicarRotacionRelativaANodo(ANodo* NodoReferencia, FVector PuntoReferencia);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    FString ObtenerContenidoNodo(ANodo* Nodo);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    class UWidgetComponent * Document;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    class UWidgetInteractionComponent * Interaction;

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void MostrarNumeracion();

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void OcultarNumeracion();
};
