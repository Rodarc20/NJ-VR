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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Referencias")
    class UMotionControllerComponent* RightController;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Referencias")
    class UMotionControllerComponent* LeftController;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Auxiliar")
    bool bHitNodo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Auxiliar")
    bool bHitBoard;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Auxiliar")
    FVector ImpactPoint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Referencias")
    AActor* HitActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Referencias")
    class ANodo* HitNodo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Referencias")
    AActor* HitLimite;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Elementos Visuales")
    TSubclassOf<class ANodo> TipoNodo;//esto no es practio llenarlo en el cosntructor, cuando esta clase pase a bluprint sera mejor

    //Dimension que ocupara cada nodo
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Elementos Visuales")
    float RadioNodos;

    //Dimension que ocupara cada nodo
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Elementos Visuales")
    float RadioNodosVirtuales;

    //el tipo de nodo que se instancia
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Elementos Visuales")
    TSubclassOf<class AArista> TipoArista;//esto no es practio llenarlo en el cosntructor, cuando esta clase pase a bluprint sera mejor

    //Dimension que ocupara cada arista
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Elementos Visuales")
    float RadioAristas;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Auxiliar")
    TArray<class ANodo*> Nodos;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Auxiliar")
    TArray<class AArista*> Aristas;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Auxiliar")
    TArray<class ANodo*> NodosSeleccionados;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Auxiliar")
    bool bNodoGuia;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Referencias")
    ANodo* NodoGuia;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Elementos Visuales")
    TArray<FLinearColor> Colores;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Elementos Visuales")
    FLinearColor ColorSeleccion;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Elementos Visuales")
    FLinearColor ColorVirtual;

    FXmlFile XmlSource;// o podria tener un puntero e ir genereando nuevos FXmlFile, todo debepnde, eso por que el contructor podria recibir el path, al ser creado, 
	
    void LoadNodos();//solo esto por que solo me interesa leer la informacion de los vertex, para saber quienes son hijos y padres, por eso tal vez no se trabaje como unity o si, probar
    virtual void CreateNodos();
    virtual void CreateAristas();

    //Task
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Modos y Tareas")
    EVRVisualizationTask CurrentVisualizationTask;
	
    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void SetVisualizationTask(EVRVisualizationTask NewVisualizationTask);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    EVRVisualizationTask GetVisualizationTask();

    //Mode
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Modos y Tareas")
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

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    virtual void AplicarEscala(float NuevaEscala);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Auxiliar")
    TArray<FVector> PosicionesNodosInicialEscala;//debe tenere el mismo tamaño que el array de nodos

    //Para el modo general
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float DistanciaLaser;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Auxiliar")
    FVector OffsetToPointLaser;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Parametros")
    float Escala;

    //Rotacion del controll capturado al presionar el trigger
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Auxiliar")
    FRotator InitialRotationController;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Auxiliar")
    FRotator InitialRotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Auxiliar")
    bool Rotar;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Auxiliar")
    bool MostrarLabel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Auxiliar")
    bool MostrarNumero;

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    virtual void AplicarRotacionRelativaANodo(ANodo* NodoReferencia, FVector PuntoReferencia);

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    FString ObtenerContenidoNodo(ANodo* Nodo);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Referencias")
    class UWidgetComponent * Document;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization - Referencias")
    class UWidgetInteractionComponent * Interaction;

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void MostrarNumeracion();

    UFUNCTION(BlueprintCallable, Category = "Visualization")
    void OcultarNumeracion();

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization - Auxiliar")
    bool bGripIzquierdo;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization - Auxiliar")
    bool bGripDerecho;

    UPROPERTY(BlueprintReadWrite, Category = "Visualization - Auxiliar")
    bool bCalcularEscala;// si no quiero que aparezca en el editor

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization - Auxiliar")
    float DistanciaInicialControles;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization - Auxiliar")
    float EscalaTemp;
};

//la escala funcionara independiete de todo el sistema anterior, para que pueda ser usado como apoyo en las otras tares, por ejemplo si es dificl selecionar un nodo, al escalar podri ser mas facil, pero no quiero cambiar de modos solo para eso.
//Lo mismo deberia hacer con la rotacion