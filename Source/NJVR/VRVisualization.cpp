// Fill out your copyright notice in the Description page of Project Settings.

#include "NJVR.h"
#include "VRVisualization.h"
#include "Nodo.h"
#include "Arista.h"

// Sets default values
AVRVisualization::AVRVisualization()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));

    Escala = 1.0f;
    //static ConstructorHelpers::FClassFinder<ANodo> NodoClass(TEXT("/Script/NJVR.Nodo"));
    //static ConstructorHelpers::FClassFinder<ANodo> NodoClass(TEXT("Class'/Script/NJVR.NodoEsfera'"));
    static ConstructorHelpers::FClassFinder<ANodo> NodoClass(TEXT("BlueprintGeneratedClass'/Game/Visualization/Blueprints/Elements/NodoEsfera_BP.NodoEsfera_BP_C'"));
    if (NodoClass.Succeeded()) {
        if(GEngine)//no hacer esta verificación provocaba error al iniciar el editor
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("TipoNodo encontrado."));
        TipoNodo = NodoClass.Class;
    }

    //Para buscar la clase arista que sera default en este codigo
    //static ConstructorHelpers::FClassFinder<AArista> AristaClass(TEXT("/Script/NJVR.Arista"));
    //static ConstructorHelpers::FClassFinder<AArista> AristaClass(TEXT("Class'/Script/NJVR.AristaCilindro'"));
    static ConstructorHelpers::FClassFinder<AArista> AristaClass(TEXT("BlueprintGeneratedClass'/Game/Visualization/Blueprints/Elements/AristaCilindro_BP.AristaCilindro_BP_C'"));
    if (AristaClass.Succeeded()) {
        if(GEngine)//no hacer esta verificación provocaba error al iniciar el editor
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("TipoArista encontrado."));
        TipoArista = AristaClass.Class;
    }

    ColorSeleccion = FLinearColor::Green;
    ColorVirtual = FLinearColor::White;
    SetVisualizationMode(EVRVisualizationMode::ENoMode);// como inicia en ste mdo deberia parecer marcado, el boton correspondiente,
    SetVisualizationTask(EVRVisualizationTask::ENoTask);//esta bien que empiece en ninguno, asi ningun boton tarea estara marcado
}

// Called when the game starts or when spawned
void AVRVisualization::BeginPlay()
{
	Super::BeginPlay();
	
    //FString path = FString("D:\\UnrealProjects\\NJVR\\Content\\Resources\\cbr-ilp-ir-son.xml");//de esta forma funciona
    //FString path = FString("D:/UnrealProjects/NJVR/Content/Resources/cbr-ilp-ir-son.xml");//de esta forma tambien funciona
    //FString path = FString("D:/UnrealProjects/NJVR/Content/Resources/Sincbr-ilp-ir.xml");//de esta forma tambien funciona
    FString path = FString("D:/UnrealProjects/NJVR/Content/Resources/imagensCorel.xml");//de esta forma tambien funciona
    //FString path = FString("D:/UnrealProjects/NJVR/Content/Resources/SinimagensCorel.xml");//de esta forma tambien funciona
    //FString path = FString("/Game/Resources/cbr-ilp-ir-son.xml");//esto no funciona
    bool cargado = XmlSource.LoadFile(path, EConstructMethod::ConstructFromFile);//para construirlo como archivo
    if (GEngine) {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Probando."));
        if(cargado)
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Cargado."));
        else
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("No cargado."));
    }
    if (cargado) {
        //LoadNodos();
        CreateNodos();
        CreateAristas();
    }

	
}

// Called every frame
void AVRVisualization::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVRVisualization::LoadNodos() {
    FXmlNode * rootnode = XmlSource.GetRootNode();
    if (GEngine) {
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, rootnode->GetTag());
    }
    TArray<FXmlNode *> XMLnodos = rootnode->GetChildrenNodes();
    TArray<FXmlNode*> XMLvertexs;
    //for(FXmlNode* nodo : nodos){
        //if (nodo->GetTag() == "vertex") {
            //vertexs.Add(nodo);
        //}
    for (int i = 0; i < XMLnodos.Num(); ++i) {
        if (XMLnodos[i]->GetTag() == "vertex") {
            XMLvertexs.Add(XMLnodos[i]);
        }
    }
    //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::FromInt(vertexs.Num()));
    //tengo todos los vertices en ese array
    for (int i = 0; i < XMLvertexs.Num(); ++i) {
        //obteniendo el id
        FString id = XMLvertexs[i]->GetAttribute(FString("id"));//devuelve el valor del atributo que le doy, hay otra funocin que me devuelve todos los atributos en un arrya de un obejto especial//quiza deba esto guardarlo como int cuando genere la clase Vertex
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, id);
        UE_LOG(LogClass, Log, TEXT("Vertex id = %s"), *id);
        
        //obteniendo el valid
        FXmlNode * nodevalid = XMLvertexs[i]->FindChildNode(FString("valid"));
        FString valid = nodevalid->GetAttribute("value");
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, valid);
        UE_LOG(LogClass, Log, TEXT("valid = %s"), *valid);

        //obteniendo la x-coordinate
        FXmlNode * nodex = XMLvertexs[i]->FindChildNode(FString("x-coordinate"));
        FString xcoordinate = nodex->GetAttribute("value");
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, xcoordinate);
        UE_LOG(LogClass, Log, TEXT("x-coordinate = %s"), *xcoordinate);

        //obteniendo la y-coordinate
        FXmlNode * nodey = XMLvertexs[i]->FindChildNode(FString("y-coordinate"));
        FString ycoordinate = nodey->GetAttribute("value");
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, ycoordinate);
        UE_LOG(LogClass, Log, TEXT("y-coordinate = %s"), *ycoordinate);

        //obteniendo url
        FXmlNode * nodeurl = XMLvertexs[i]->FindChildNode(FString("url"));
        FString url = nodeurl->GetAttribute("value");
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, url);
        UE_LOG(LogClass, Log, TEXT("url = %s"), *url);

        //obteniendo parent 
        FXmlNode * nodeparent = XMLvertexs[i]->FindChildNode(FString("parent"));//quiza no sean necesario usar FString
        FString parent = nodeparent->GetAttribute("value");
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, parent);
        UE_LOG(LogClass, Log, TEXT("parent = %s"), *parent);

        //los hijos no estan dentro de un array por lo tanto es necesario reccorrer todos los child nose, es decir lo de aqui arruba fue por las puras, jeje
        TArray<FString> sons;
        TArray<FXmlNode*> childs = XMLvertexs[i]->GetChildrenNodes();
        for (int j = 0; j < childs.Num(); j++) {
            if (childs[j]->GetTag() == "son") {
                sons.Add(childs[j]->GetAttribute("value"));
            }
        }
        for (int j = 0; j < sons.Num(); j++) {
            //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, sons[j]);
            UE_LOG(LogClass, Log, TEXT("son = %s"), *sons[j]);
        }

        FXmlNode * nodelabels = XMLvertexs[i]->FindChildNode(FString("labels"));//quiza no sean necesario usar FString
        TArray<FXmlNode*> labelschilds = nodelabels->GetChildrenNodes();
        TArray<FString> labels;
        for (int j = 0; j < labelschilds.Num(); j++) {
            labels.Add(labelschilds[j]->GetAttribute("value"));
            //aqui faltaria definir que label es cada uno, para poder ponerlo en la variable que corresponda en el la calse vertex que creare
        }
        for (int j = 0; j < labels.Num(); j++) {
            //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, labels[j]);
            UE_LOG(LogClass, Log, TEXT("label = %s"), *labels[j]);
        }
        //el contenido de los nodos, es lo que hay en trexto plano dentro del tag de apertura y de cierre

        //TArray<FXmlNode*> childs = vertexs[i]->GetChildrenNodes();//para el caso de los vertexs sus hijos son unicos o son un array por lo tanto podria usar la funcion findchildren, para encontrar los que necesito

    }
}

void AVRVisualization::CreateNodos() {

}

void AVRVisualization::CreateAristas() {

}

EVRVisualizationTask AVRVisualization::GetVisualizationTask() {
    return CurrentVisualizationTask;
}

void AVRVisualization::SetVisualizationTask(EVRVisualizationTask NewVisualizationTask) {
    CurrentVisualizationTask = NewVisualizationTask;
}

EVRVisualizationMode AVRVisualization::GetVisualizationMode() {
    return CurrentVisualizationMode;
}

void AVRVisualization::SetVisualizationMode(EVRVisualizationMode NewVisualizationMode) {
    CurrentVisualizationMode = NewVisualizationMode;
}

//algunas de destas funciones son la misma para todos asl visualizaciones, otras no, esas sobrescribir las que estan aqui
void AVRVisualization::SeleccionarNodo(ANodo * NodoSeleccionado) {
    NodoSeleccionado->Selected = true;
    NodoSeleccionado->CambiarColor(ColorSeleccion);
    NodosSeleccionados.Add(NodoSeleccionado);
}
void AVRVisualization::DeseleccionarNodo(ANodo * NodoSeleccionado) {
    NodoSeleccionado->Selected = false;
    NodoSeleccionado->CambiarColor(NodoSeleccionado->Color);
    NodosSeleccionados.Remove(NodoSeleccionado);
}

void AVRVisualization::SeleccionarRama(ANodo * NodoSeleccionado) {
    TQueue<ANodo*> Cola;
    Cola.Enqueue(NodoSeleccionado);
    while (!Cola.IsEmpty()) {
        Cola.Dequeue(NodoSeleccionado);
        NodoSeleccionado->Selected = true;
        NodoSeleccionado->CambiarColor(ColorSeleccion);
        NodosSeleccionados.Add(NodoSeleccionado);
        for (int i = 0; i < NodoSeleccionado->Sons.Num(); i++) {
            Cola.Enqueue(NodoSeleccionado->Sons[i]);
        }
    }
}

void AVRVisualization::DeseleccionarRama(ANodo * NodoSeleccionado) {
    TQueue<ANodo*> Cola;
    Cola.Enqueue(NodoSeleccionado);
    while (!Cola.IsEmpty()) {
        Cola.Dequeue(NodoSeleccionado);
        NodoSeleccionado->Selected = false;
        NodoSeleccionado->CambiarColor(NodoSeleccionado->Color);
        NodosSeleccionados.Remove(NodoSeleccionado);
        for (int i = 0; i < NodoSeleccionado->Sons.Num(); i++) {
            Cola.Enqueue(NodoSeleccionado->Sons[i]);
        }
    }
}

void AVRVisualization::SeleccionarTodo() {
    NodosSeleccionados = Nodos;// esta accion borra todos los elementos y le copia los elementos del segundo array //pero ahora debo marcarlos y 
    for (int i = 0; i < NodosSeleccionados.Num(); i++) {
        NodosSeleccionados[i]->Selected = true;
        NodosSeleccionados[i]->CambiarColor(ColorSeleccion);
    }
}

void AVRVisualization::DeseleccionarTodo() {
    for (int i = 0; i < NodosSeleccionados.Num(); i++) {
        NodosSeleccionados[i]->Selected = false;
        NodosSeleccionados[i]->CambiarColor(NodosSeleccionados[i]->Color);
    }
    NodosSeleccionados.Empty();
}

void AVRVisualization::AplicarTraslacion(FVector Traslacion) {

}

/*void AVRVisualization::AplicarEscala(float NuevaEscala) {
    for (int i = 0; i < Nodos.Num(); i++) {
        Nodos[i]->Escala = NuevaEscala;
        Nodos[i]->Actualizar();//este deberia actualziar tal vez también la posicion del nodo, o calcularlo afuera
        FTransform NewTransform = Nodos[i]->GetActorTransform();
        //NewTransform.SetLocation(Nodos[i]->GetTransform().GetLocation() * NuevaEscala);
        NewTransform.SetLocation(PosicionesNodosInicialEscala[i] * NuevaEscala);
        Nodos[i]->SetActorTransform(NewTransform);
    }
    for (int i = 0; i < Aristas.Num(); i++) {
        Aristas[i]->Escala = NuevaEscala;
        Aristas[i]->Actualizar();
    }
}*/
//Hya dos formas de aplicar la escala, una seria la que esta expresada, que los elemnetos y sus posiciones esten escalados respecto a esta clase
//la otra es que se escale esta clase, como los nodos y las aristas estan dentro, como hijos, tambien deberian ser escalados, eso evita mucho calculo y correciones
void AVRVisualization::AplicarEscala(float NuevaEscala) {
    //esta forma es menos costosa, y parece mas sencialla de manipular todos los elementos, solo hya que hacer que todo tenga concordancia, no como la arista actual
    SetActorScale3D(FVector(NuevaEscala));
    //FTransform NuevoTransform = GetTransform();
    //NuevoTransform.SetScale3D(FVector(NuevaEscala));
    //SetActorTransform(NuevoTransform);
}

void AVRVisualization::AplicarRotacionRelativaANodo(ANodo* NodoReferencia, FVector PuntoReferencia) {

}

FString AVRVisualization::ObtenerContenidoNodo(ANodo* Nodo) {
    FString contenido;
    FString archivo("D:/UnrealProjects/NJVR/Content/Resources/cbr-ilp-ir-son/");
    archivo += Nodo->Url;
    FFileHelper::LoadFileToString(contenido, *archivo);
    return contenido;
}

void AVRVisualization::MostrarNumeracion() {
    for (int i = 0; i < Nodos.Num(); i++)
        Nodos[i]->Numero->SetVisibility(true);

}

void AVRVisualization::OcultarNumeracion() {
    for (int i = 0; i < Nodos.Num(); i++)
        Nodos[i]->Numero->SetVisibility(false);
}