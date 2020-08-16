// Fill out your copyright notice in the Description page of Project Settings.

#include "Layout1PMVRVisualization.h"
#include "NJVR.h"
#include "Nodo.h"
#include "Arista.h"
#include "VRPawn.h"
#include "MotionControllerComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include <stack>
#include <queue>

ALayout1PMVRVisualization::ALayout1PMVRVisualization(){

    Radio = 300.0f;
    PhiMax = PI;
    PhiMin = 0;
    PhiMax = FMath::DegreesToRadians(150);
    PhiMin = FMath::DegreesToRadians(40);

    PrecisionAristas = 8;
    PrecisionNodos = 2;

	NodosMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedNodoMesh"));
    NodosMesh->SetupAttachment(RootComponent);
	/**
	*	Create/replace a section for this procedural mesh component.
	*	@param	SectionIndex		Index of the section to create or replace.
	*	@param	Vertices			Vertex buffer of all vertex positions to use for this mesh section.
	*	@param	Triangles			Index buffer indicating which vertices make up each triangle. Length must be a multiple of 3.
	*	@param	Normals				Optional array of normal vectors for each vertex. If supplied, must be same length as Vertices array.
	*	@param	UV0					Optional array of texture co-ordinates for each vertex. If supplied, must be same length as Vertices array.
	*	@param	VertexColors		Optional array of colors for each vertex. If supplied, must be same length as Vertices array.
	*	@param	Tangents			Optional array of tangent vector for each vertex. If supplied, must be same length as Vertices array.
	*	@param	bCreateCollision	Indicates whether collision should be created for this section. This adds significant cost.
	*/
	//UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh", meta = (AutoCreateRefTerm = "Normals,UV0,VertexColors,Tangents"))
	//	void CreateMeshSection(int32 SectionIndex, const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector>& Normals,
	// const TArray<FVector2D>& UV0, const TArray<FColor>& VertexColors, const TArray<FProcMeshTangent>& Tangents, bool bCreateCollision);
 
	// New in UE 4.17, multi-threaded PhysX cooking.
	NodosMesh->bUseAsyncCooking = true;

	AristasMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedAristaMesh"));
    AristasMesh->SetupAttachment(RootComponent);
	AristasMesh->bUseAsyncCooking = true;

    CreateSphereTemplate(PrecisionNodos);

    static ConstructorHelpers::FObjectFinder<UMaterial> NodosMaterialAsset(TEXT("Material'/Game/Visualization/Materials/NodosMeshMaterial.NodosMeshMaterial'"));//de usar este creo que debo crear un obtener un  material y ponerselo, este tiene el pivot en el centro de la esfera
    if (NodosMaterialAsset.Succeeded()) {
        NodosMeshMaterial = NodosMaterialAsset.Object;
    }

    static ConstructorHelpers::FObjectFinder<UMaterial> AristasMaterialAsset(TEXT("Material'/Game/Visualization/Materials/AristasMeshMaterial.AristasMeshMaterial'"));//de usar este creo que debo crear un obtener un  material y ponerselo, este tiene el pivot en el centro de la esfera
    if (AristasMaterialAsset.Succeeded()) {
        AristasMeshMaterial = AristasMaterialAsset.Object;
    }

}

void ALayout1PMVRVisualization::BeginPlay() {
    Super::BeginPlay();
    Layout(Radio);
    //Layout5(Radio);
    ActualizarLayout();
}

void ALayout1PMVRVisualization::CreateNodos() {
    FXmlNode * rootnode = XmlSource.GetRootNode();
    //if (GEngine) {
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, rootnode->GetTag());
    //}
    TArray<FXmlNode *> XMLnodes = rootnode->GetChildrenNodes();
    TArray<FXmlNode*> XMLvertexs;
    for (int i = 0; i < XMLnodes.Num(); ++i) {
        if (XMLnodes[i]->GetTag() == "vertex") {
            XMLvertexs.Add(XMLnodes[i]);
        }
    }
    //obtenienod la unicaion del ulitmo nodo para centrar todo el arbol
    FXmlNode * nodexorigen = XMLvertexs[XMLvertexs.Num()-1]->FindChildNode(FString("x-coordinate"));
    FString xorigen = nodexorigen->GetAttribute("value");
    float OrigenX = FCString::Atof(*xorigen);
    FXmlNode * nodeyorigen = XMLvertexs[XMLvertexs.Num()-1]->FindChildNode(FString("y-coordinate"));
    FString yorigen = nodeyorigen->GetAttribute("value");
    float OrigenY = FCString::Atof(*yorigen) * -1;
    //tengo todos los vertices en ese array
    TArray<int> numerocolores;
    for (int i = 0; i < XMLvertexs.Num(); ++i) {
        //obteniendo el id
        FString id = XMLvertexs[i]->GetAttribute(FString("id"));//devuelve el valor del atributo que le doy, hay otra funocin que me devuelve todos los atributos en un arrya de un obejto especial//quiza deba esto guardarlo como int cuando genere la clase Vertex
        
        //obteniendo el valid
        FXmlNode * nodevalid = XMLvertexs[i]->FindChildNode(FString("valid"));
        FString valid = nodevalid->GetAttribute("value");

        //obteniendo la x-coordinate
        FXmlNode * nodex = XMLvertexs[i]->FindChildNode(FString("x-coordinate"));
        FString xcoordinate = nodex->GetAttribute("value");

        //obteniendo la y-coordinate
        FXmlNode * nodey = XMLvertexs[i]->FindChildNode(FString("y-coordinate"));
        FString ycoordinate = nodey->GetAttribute("value");

        //obteniendo url
        FXmlNode * nodeurl = XMLvertexs[i]->FindChildNode(FString("url"));
        FString url = nodeurl->GetAttribute("value");

        //obteniendo parent 
        FXmlNode * nodeparent = XMLvertexs[i]->FindChildNode(FString("parent"));//quiza no sean necesario usar FString
        FString parent = nodeparent->GetAttribute("value");

        //los hijos no estan dentro de un array por lo tanto es necesario reccorrer todos los child nose, es decir lo de aqui arruba fue por las puras, jeje
        TArray<FString> sons;
        TArray<FXmlNode*> childs = XMLvertexs[i]->GetChildrenNodes();
        for (int j = 0; j < childs.Num(); j++) {
            if (childs[j]->GetTag() == "son") {
                sons.Add(childs[j]->GetAttribute("value"));
            }
        }

        FXmlNode * nodelabels = XMLvertexs[i]->FindChildNode(FString("labels"));//quiza no sean necesario usar FString
        TArray<FXmlNode*> labelschilds = nodelabels->GetChildrenNodes();
        TArray<FString> labels;
        for (int j = 0; j < labelschilds.Num(); j++) {
            labels.Add(labelschilds[j]->GetAttribute("value"));
            //aqui faltaria definir que label es cada uno, para poder ponerlo en la variable que corresponda en el la calse vertex que creare
        }
        FXmlNode * nodescalars = XMLvertexs[i]->FindChildNode(FString("scalars"));//quiza no sean necesario usar FString
        TArray<FXmlNode*> scalarschilds = nodescalars->GetChildrenNodes();
        FString colorcdata;
        for (int j = 0; j < scalarschilds.Num(); j++) {
            if (scalarschilds[j]->GetAttribute("name") == "cdata") {
                colorcdata = scalarschilds[j]->GetAttribute("value");
            }
        }
        //el contenido de los nodos, es lo que hay en trexto plano dentro del tag de apertura y de cierre

        //TArray<FXmlNode*> childs = vertexs[i]->GetChildrenNodes();//para el caso de los vertexs sus hijos son unicos o son un array por lo tanto podria usar la funcion findchildren, para encontrar los que necesito

        //creando un objeto nodo, instanciando y llenando sus datos
        UWorld * const World = GetWorld();
        if (World) {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.Instigator = GetInstigator();

            FVector SpawnLocation(0.0f, FCString::Atof(*xcoordinate) - OrigenX, -1*FCString::Atof(*ycoordinate) - OrigenY);//ejes invertidos a los recibidos//-1 al y
            SpawnLocation = GetTransform().TransformPosition(SpawnLocation);
            ANodo * const NodoInstanciado = World->SpawnActor<ANodo>(TipoNodo, SpawnLocation * Escala, FRotator::ZeroRotator, SpawnParams);
            NodoInstanciado->Id = FCString::Atoi(*id);
            NodoInstanciado->Numero->SetText(FText::FromString(*id));//para el texto del numero, quiza este tipo de funcionalidad deberia estar encapsulada en alguna funcion de la clase nodo
            NodoInstanciado->Valid = FCString::ToBool(*valid);
            NodoInstanciado->Xcoordinate = FCString::Atof(*xcoordinate);
            NodoInstanciado->Ycoordinate = FCString::Atof(*ycoordinate);
            NodoInstanciado->Url = url;
            for (int j = 0; j < labels.Num(); j++) {
                NodoInstanciado->Labels.Add(labels[j]);
            }
            if(labels.Num()){
                NodoInstanciado->Nombre->SetText(FText::FromString(labels[0]));
            }
            NodoInstanciado->ParentId = FCString::Atoi(*parent);
            for (int j = 0; j < sons.Num(); j++) {
                NodoInstanciado->SonsId.Add(FCString::Atoi(*sons[j]));
                NodoInstanciado->Sons.Add(Nodos[NodoInstanciado->SonsId[j]]);//para agregar la referencia, esto o se peude con el padre, por que en toeria aun no existe, habria que realizar una segunda pasada, alli podiasmos incluir esto para evtar algun fallo
            }
            NodoInstanciado->Selected = false;
            if (NodoInstanciado->Valid) {//aqui a los nodos reales se le debe asiganar algun colo de acerud a algun criterio, por ahora dejar asi
                NodoInstanciado->Color = FLinearColor::Black;
                NodoInstanciado->ColorNum = FCString::Atoi(*colorcdata);
                NodoInstanciado->Radio = RadioNodos;
                numerocolores.AddUnique(NodoInstanciado->ColorNum);
                //UE_LOG(LogClass, Log, TEXT("Color = %d"), NodoInstanciado->ColorNum);
            }
            else {
                NodoInstanciado->Color = ColorVirtual;//tambien debo cambiarle el tamaño
                NodoInstanciado->ColorNum = FCString::Atoi(*colorcdata);
                NodoInstanciado->Radio = RadioNodosVirtuales;
            }
            //actualizar nodo, para cambiar el color o el tamaño si es necesario
            NodoInstanciado->Escala = Escala;
            NodoInstanciado->Actualizar();
            //NodoInstanciado->AttachRootComponentToActor(this);
            NodoInstanciado->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);//segun el compilador de unral debo usar esto
            Nodos.Add(NodoInstanciado);
            //NodoInstanciado->GetRootComponent()->SetupAttachment(RootComponent);// para hacerlo hioj de la visualización, aunque parece que esto no es suficient
        }
    }
    //UE_LOG(LogClass, Log, TEXT("NumeroColor = %d"), numerocolores.Num());
    int variacion = 360 / numerocolores.Num();
    for (int k = 0; k < numerocolores.Num(); k++) {
        int h = (k*variacion) % 360;
        Colores.Add(UKismetMathLibrary::HSVToRGB(h, 1.0f, IntensidadColor, 1.0f));
    }
    for (int i = 0; i < Nodos.Num(); i++) {
        Nodos[i]->Parent = Nodos[Nodos[i]->ParentId];
        Nodos[i]->Usuario = Usuario;
        if (Nodos[i]->Valid) {
            if (bColorear)
                Nodos[i]->Color = Colores[Nodos[i]->ColorNum];
            else
                Nodos[i]->Color = ColorReal;
            Nodos[i]->Actualizar();
        }
    }

    //añandiendo nodos al procedular mesh
    for (int i = 0; i < Nodos.Num(); i++) {
        if (Nodos[i]->Valid) {
            AddNodoToMesh(Nodos[i]->GetActorLocation(), RadioNodos, Nodos[i]->Color, i);
        }
        else {
            AddNodoToMesh(Nodos[i]->GetActorLocation(), RadioNodosVirtuales, Nodos[i]->Color, i);
        }
    }
    CreateNodosMesh();
}


void ALayout1PMVRVisualization::CreateAristas() {
    int count = 0;
    UWorld * const World = GetWorld();
    if (World) {//este if deberia estar afuera
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = GetInstigator();
        for (int i = 0; i < Nodos.Num(); i++) {
            for (int j = 0; j < Nodos[i]->Sons.Num(); j++) {
                int padre = i;
                int hijo = Nodos[i]->SonsId[j];


                FVector Diferencia = Nodos[hijo]->GetActorLocation() - Nodos[padre]->GetActorLocation();
                FVector Direccion = Diferencia.GetClampedToSize(1.0f, 1.0f);
                //FVector SpawnLocation(Diferencia/2 + Nodos[padre]->GetActorLocation());//ejes invertidos a los recibidos, esto tal vez deba ser la posicion relaitva, no la general
                FVector SpawnLocation(Diferencia/2 + Nodos[padre]->GetActorLocation() - GetActorLocation());//ejes invertidos a los recibidos, esto tal vez deba ser la posicion relaitva, no la general
                //necesito las posiones de los nodos relativas a esta clase
                SpawnLocation = GetTransform().TransformPosition(SpawnLocation);
                //FRotator r(angle, 0.0f, 0.0f);
                //FRotator r(0.0f, angle, 0.0f);//rotacion con el eje up
                //UE_LOG(LogClass, Log, TEXT("Arista id = %d, angle= %f, sing = %f"), count, angle, sing);
                /*float angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(FVector::UpVector, Direccion)));
                float sing = FVector::CrossProduct(FVector::UpVector, Direccion).X;//esto es por que el signo es impotante para saber si fue un angulo mayor de 180 o no
                if (sing >= 0) {
                    angle = 360-angle;
                }
                FRotator SpawnRotation(0.0f, 0.0f, angle);*/
                FRotator SpawnRotation(0.0f, 0.0f, 0.0f);

                AArista * const AristaInstanciado = World->SpawnActor<AArista>(TipoArista, SpawnLocation, SpawnRotation, SpawnParams);//creo que es mejor si yo hago los calculos de los angulos, para generar el rotator
                //AArista * const AristaInstanciado = World->SpawnActor<AArista>(TipoArista, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);//creo que es mejor si yo hago los calculos de los angulos, para generar el rotator
                AristaInstanciado->Id = count;
                AristaInstanciado->SourceId = padre;
                AristaInstanciado->TargetId = hijo;
                AristaInstanciado->SourceNodo = Nodos[padre];
                AristaInstanciado->TargetNodo = Nodos[hijo];
                AristaInstanciado->Escala = Escala;
                AristaInstanciado->Radio = RadioAristas;
                AristaInstanciado->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
                //AristaInstanciado->AttachRootComponentToActor(this);

                Aristas.Add(AristaInstanciado);
                count++;
            }
        }
        //instancia de la ultima arista
        int padre = Nodos.Num() - 1;
        int hijo = Nodos[Nodos.Num() - 1]->ParentId;
        FVector Diferencia = Nodos[hijo]->GetActorLocation() - Nodos[padre]->GetActorLocation();
        FVector Direccion = Diferencia.GetClampedToSize(1.0f, 1.0f);
        FVector SpawnLocation(Diferencia/2 + Nodos[padre]->GetActorLocation());//ejes invertidos a los recibidos
        //FRotator r(angle, 0.0f, 0.0f);
        //FRotator r(0.0f, angle, 0.0f);//rotacion con el eje up
        //UE_LOG(LogClass, Log, TEXT("Arista id = %d, angle= %f, sing = %f"), count, angle, sing);
        float angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(FVector::UpVector, Direccion)));
        float sing = FVector::CrossProduct(FVector::UpVector, Direccion).X;//esto es por que el signo es impotante para saber si fue un angulo mayor de 180 o no
        if (sing >= 0) {
            angle = 360-angle;
        }
        FRotator SpawnRotation(0.0f, 0.0f, angle);

        AArista * const AristaInstanciado = World->SpawnActor<AArista>(TipoArista, SpawnLocation, SpawnRotation, SpawnParams);//creo que es mejor si yo hago los calculos de los angulos, para generar el rotator
        //los calculos de tamañao direccion y posición debe estar dentro de la arita solo deberia pasarle la información referente a los nodos, la rista sola debe autocalcular lo demas
        //AArista * const AristaInstanciado = World->SpawnActor<AArista>(TipoArista, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);//creo que es mejor si yo hago los calculos de los angulos, para generar el rotator
        AristaInstanciado->Id = count;
        AristaInstanciado->SourceId = padre;
        AristaInstanciado->TargetId = hijo;
        AristaInstanciado->SourceNodo = Nodos[padre];
        AristaInstanciado->TargetNodo = Nodos[hijo];
        AristaInstanciado->Escala = Escala;
        AristaInstanciado->Radio = RadioAristas;
        AristaInstanciado->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
        //AristaInstanciado->AttachRootComponentToActor(this);

        Aristas.Add(AristaInstanciado);
        count++;
    }
    //añandiendo nodos al procedular mesh
    for (int i = 0; i < Aristas.Num(); i++) {
        AddAristaToMesh(Aristas[i]->SourceNodo->GetTransform().GetLocation(), Aristas[i]->TargetNodo->GetTransform().GetLocation(), RadioAristas, i);
    }
    CreateAristasMesh();
    for (int i = 0; i < Aristas.Num(); i++) {
        Aristas[i]->Actualizar();//estas funciones asumen que ya hay una mes creado, es decir ya existen vertices y demas, por eso debo  crear antes de usar
        Aristas[i]->ActualizarCollision();
    }
}

/*int ALayout1VRVisualization::mod(int a, int b) {
    int d = a / b;
    int m = a - b*d;
    if (m < 0)
        m += b;
    return m;
}

float ALayout1VRVisualization::modFloat(float a, float b) {
    //por ahora es solo para el exceso en resta, para el esferico nunca exceden el doblre asi que sera solo la suma para hacerlo positivo
    if (a > b) {
        return a - b;
    }
    if (a < 0) {
        return a + b;
    }
    return a;
}*/

void ALayout1PMVRVisualization::Calculos(ANodo * V) {//lo uso dentro de claculos 2, por alguna razon
    //calcular hojas, altura,
    if (V->Sons.Num() == 0) {//deberia usar si es virtual o no
        V->Hojas = 1;
        V->Altura = 0;
    }
    else {
        V->Hojas = 0;
        V->Altura = 0;
        for (int i = 0; i < V->Sons.Num(); i++) {
            Calculos(V->Sons[i]);
            V->Hojas += V->Sons[i]->Hojas;
            V->Altura = FMath::Max<float>(V->Altura, V->Sons[i]->Altura);
        }
        V->Altura++;
    }
    //si el arbol tiene 4 niveles, el valor de altura de la raiz es 3
}

void ALayout1PMVRVisualization::Calculos2() {//calcula hojas y altura, de otra forma
    ANodo * Root = Nodos[Nodos.Num() - 1];
    Calculos(Root->Parent);
    Root->Altura = Root->Parent->Altura;
    Root->Hojas = Root->Parent->Hojas;
    for (int i = 0; i < Root->Sons.Num(); i++) {
        Calculos(Root->Sons[i]);
        Root->Hojas += Root->Sons[i]->Hojas;
        Root->Altura = FMath::Max<float>(Root->Altura, Root->Sons[i]->Altura);
    }
    Root->Altura++;
}

void ALayout1PMVRVisualization::Layout(float NewRadio) {//en este algoritmo puedo asignar el nivel
    TQueue<ANodo *> Cola;
    //la raiz es el ultimo nodo
    ANodo * Root = Nodos[Nodos.Num() - 1];
    Calculos2();
    Calc();//no estaba antes
    float DeltaPhi = (PhiMax-PhiMin) / (Root->Altura+1);
    UE_LOG(LogClass, Log, TEXT("DeltaPhi = %f"), DeltaPhi);
    //agregado para el nuevo radio
    //int NivelDenso, CantidadNodosNivelDenso;
    //NivelMasDenso(NivelDenso, CantidadNodosNivelDenso);
    //NewRadio = EncontrarRadio1(DeltaPhi * NivelDenso, CantidadNodosNivelDenso); 

    //Radio = EncontrarRadio2(DeltaPhi * Root->Altura); //este es el que estoy usando, no recibo radio por la funcion
    
    Root->Theta = 0;
    Root->Phi = 0;
    Root->WTam = 2*PI;
    Root->WInicio = 0;
    Root->Xcoordinate = Radio * FMath::Sin(Root->Phi) * FMath::Cos(Root->Theta);
    Root->Ycoordinate = Radio * FMath::Sin(Root->Phi) * FMath::Sin(Root->Theta);
    Root->Zcoordinate = Radio * FMath::Cos(Root->Phi);
    UE_LOG(LogClass, Log, TEXT("Root id = %d, (%f,%f,%f)"), Root->Id, Root->Xcoordinate, Root->Ycoordinate, Root->Zcoordinate);
    //float DeltaPhi = PI / Root->Altura;
    float WTemp = Root->WInicio;
    //debo tener en cuenta al padre para hacer los calculos, ya que esto esta como arbol sin raiz

    //Root->Parent->Phi = Root->Phi + DeltaPhi;//estaba dividido /2
    Root->Parent->Phi = PhiMin;//estaba dividido /2
    Root->Parent->WTam = Root->WTam * (float(Root->Parent->Hojas) / Root->Hojas);
    Root->Parent->WInicio = WTemp;
    Root->Parent->Theta = WTemp + Root->Parent->WTam / 2;
    Root->Parent->Xcoordinate = Radio * FMath::Sin(Root->Parent->Phi) * FMath::Cos(Root->Parent->Theta);
    Root->Parent->Ycoordinate = Radio * FMath::Sin(Root->Parent->Phi) * FMath::Sin(Root->Parent->Theta);
    Root->Parent->Zcoordinate = Radio * FMath::Cos(Root->Parent->Phi);
    WTemp += Root->Parent->WTam;
    Cola.Enqueue(Root->Parent);
    for (int i = 0; i < Root->Sons.Num(); i++) {
        //Root->Sons[i]->Phi = Root->Phi + DeltaPhi;//estaba dividido por 2
        Root->Sons[i]->Phi = PhiMin;
        Root->Sons[i]->WTam = Root->WTam * (float(Root->Sons[i]->Hojas) / Root->Hojas);
        Root->Sons[i]->WInicio = WTemp;
        Root->Sons[i]->Theta = WTemp + Root->Sons[i]->WTam / 2;
        Root->Sons[i]->Xcoordinate = Radio * FMath::Sin(Root->Sons[i]->Phi) * FMath::Cos(Root->Sons[i]->Theta);
        Root->Sons[i]->Ycoordinate = Radio * FMath::Sin(Root->Sons[i]->Phi) * FMath::Sin(Root->Sons[i]->Theta);
        Root->Sons[i]->Zcoordinate = Radio * FMath::Cos(Root->Sons[i]->Phi);
        WTemp += Root->Sons[i]->WTam;
        Cola.Enqueue(Root->Sons[i]);
    }

    while (!Cola.IsEmpty()) {
        ANodo * V;
        Cola.Dequeue(V);
        WTemp = V->WInicio;
        for (int i = 0; i < V->Sons.Num(); i++) {
            V->Sons[i]->Phi = V->Phi + DeltaPhi;
            V->Sons[i]->WTam = V->WTam * (float(V->Sons[i]->Hojas) / V->Hojas);
            V->Sons[i]->WInicio = WTemp;
            V->Sons[i]->Theta = WTemp + V->Sons[i]->WTam / 2;
            V->Sons[i]->Xcoordinate = Radio * FMath::Sin(V->Sons[i]->Phi) * FMath::Cos(V->Sons[i]->Theta);
            V->Sons[i]->Ycoordinate = Radio * FMath::Sin(V->Sons[i]->Phi) * FMath::Sin(V->Sons[i]->Theta);
            V->Sons[i]->Zcoordinate = Radio * FMath::Cos(V->Sons[i]->Phi);
            WTemp += V->Sons[i]->WTam;
            Cola.Enqueue(V->Sons[i]);
        }
    }
}


void ALayout1PMVRVisualization::ActualizarLayout() {//este actulizar deberia ser general
    for (int i = 0; i < Nodos.Num(); i++) {
        FVector NuevaPosicion;
        NuevaPosicion.X = Nodos[i]->Xcoordinate;
        NuevaPosicion.Y = Nodos[i]->Ycoordinate;
        NuevaPosicion.Z = Nodos[i]->Zcoordinate;
        UE_LOG(LogClass, Log, TEXT("Nodo id = %d, (%f,%f,%f)"), Nodos[i]->Id, NuevaPosicion.X, NuevaPosicion.Y, NuevaPosicion.Z);
        //Nodos[i]->SetActorLocation(NuevaPosicion);
        Nodos[i]->SetActorRelativeLocation(NuevaPosicion);
        UpdatePosicionNodoMesh(Nodos[i]->Id, NuevaPosicion);
    }
    for (int i = 0; i < Aristas.Num(); i++) {
        Aristas[i]->Actualizar();
        UpdatePosicionesAristaMesh(Aristas[i]->Id, Aristas[i]->SourceNodo->GetRootComponent()->GetRelativeTransform().GetLocation(), Aristas[i]->TargetNodo->GetRootComponent()->GetRelativeTransform().GetLocation());
    }

    UpdateNodosMesh();
    UpdateAristasMesh();

}
void ALayout1PMVRVisualization::Calc() {//para hallar niveles
    std::stack<ANodo *> pila;
    //la raiz es el ultimo nodo
    ANodo * Root = Nodos[Nodos.Num() - 1];
    Root->Nivel = 0;
    //pila.push(Root);//no deberia dsencolarlo
    Root->Parent->Nivel = 1;
    pila.push(Root->Parent);
    Root->Sons[1]->Nivel = 1;
    pila.push(Root->Sons[1]);
    Root->Sons[0]->Nivel = 1;
    pila.push(Root->Sons[0]);
    while (!pila.empty()) {
        ANodo * V = pila.top();
        pila.pop();
        if (V->Sons.Num()) {
            for (int i = V->Sons.Num()-1; i >= 0; i--) {
                V->Sons[i]->Nivel = V->Nivel + 1;
                pila.push(V->Sons[i]);
            }
        }
    }
}

int ALayout1PMVRVisualization::EncontrarNivel(TQueue<ANodo *> & cola, ANodo * Rama, int Nivel) {//este requiere que los nodos tenga el nivel pueso, o que este layout calcule el nivel
    //esta funcion deberia ser generica, para todos los layouts, es util econtrar los nodos de un nivel den determinada rama, crear una para con queue que es mas util
    //encuentra los nodos del nivel respectivo
    int NumHojas = 0;
    ANodo * V;
    cola.Enqueue(Rama);
    NumHojas += Rama->Hojas;
    while (!cola.IsEmpty() && cola.Peek(V) && V->Nivel != Nivel) {
        cola.Dequeue(V);
        NumHojas -= V->Hojas;
        for (int i = 0; i < V->Sons.Num(); i++) {
            cola.Enqueue(V->Sons[i]);
            NumHojas += V->Sons[i]->Hojas;
        }
    }
    cola.Peek(V);
    //UE_LOG(LogClass, Log, TEXT("tam cola = %d"), V->Nivel);
    //al finalizar la cola, tendra los nodos de el nivel ene l que empieza esta cosa
    return NumHojas;
}

void ALayout1PMVRVisualization::Layout3(float NewRadio) {
    Calculos2();//no siemepre seran necesarios, reptirlos si ya se calcualron antes, salvo que cambien la estructura del arbol y se deban actualizar
    Calc();
    int NivelDenso= NivelMasDenso();
    ANodo * Root = Nodos[Nodos.Num() - 1];
    //para calcular el deltaPhi, necesito examinar hacia arriba y hacia abajo del nivel maximo, seleccionar el que tenga mas niveles y dividir PI/2 entre esa cantidad de niveles, para calclar la variacion
    //y luego calcular el phi inicial, para el primer nivel despues de la raiz, y emepzar desde alli el layout
    float DeltaPhi;
    float DeltaInicial;
    if (NivelDenso > Root->Altura / 2) {//estos calculos cambiaran cuano pempiece a limitar los angulos maximos para la esfera, tamibien falta agergar el umbral
        DeltaPhi = (PI / 2) / NivelDenso;
        DeltaInicial = DeltaPhi;//aplica desde el primer nivel
    }
    else {
        DeltaPhi = (PI / 2) / (Root->Altura - NivelDenso);
        DeltaInicial = PI / 2 - (NivelDenso - 1)*DeltaPhi;
    }
    
    TQueue<ANodo *> Cola;
    //la raiz es el ultimo nodo
    Root->Theta = 0;
    Root->Phi = 0;
    Root->WTam = 2*PI;
    Root->WInicio = 0;
    Root->Xcoordinate = Radio * FMath::Sin(Root->Phi) * FMath::Cos(Root->Theta);
    Root->Ycoordinate = Radio * FMath::Sin(Root->Phi) * FMath::Sin(Root->Theta);
    Root->Zcoordinate = Radio * FMath::Cos(Root->Phi);
    UE_LOG(LogClass, Log, TEXT("Layout 3 Nivel Denso = %d"), NivelDenso);
    //float DeltaPhi = PI / Root->Altura;
    float WTemp = Root->WInicio;
    //debo tener en cuenta al padre para hacer los calculos, ya que esto esta como arbol sin raiz
    Root->Parent->Phi = DeltaInicial;
    Root->Parent->WTam = Root->WTam * (float(Root->Parent->Hojas) / Root->Hojas);
    Root->Parent->WInicio = WTemp;
    Root->Parent->Theta = WTemp + Root->Parent->WTam / 2;
    Root->Parent->Xcoordinate = Radio * FMath::Sin(Root->Parent->Phi) * FMath::Cos(Root->Parent->Theta);
    Root->Parent->Ycoordinate = Radio * FMath::Sin(Root->Parent->Phi) * FMath::Sin(Root->Parent->Theta);
    Root->Parent->Zcoordinate = Radio * FMath::Cos(Root->Parent->Phi);
    WTemp += Root->Parent->WTam;
    Cola.Enqueue(Root->Parent);
    for (int i = 0; i < Root->Sons.Num(); i++) {
        Root->Sons[i]->Phi = DeltaInicial;
        Root->Sons[i]->WTam = Root->WTam * (float(Root->Sons[i]->Hojas) / Root->Hojas);
        Root->Sons[i]->WInicio = WTemp;
        Root->Sons[i]->Theta = WTemp + Root->Sons[i]->WTam / 2;
        Root->Sons[i]->Xcoordinate = Radio * FMath::Sin(Root->Sons[i]->Phi) * FMath::Cos(Root->Sons[i]->Theta);
        Root->Sons[i]->Ycoordinate = Radio * FMath::Sin(Root->Sons[i]->Phi) * FMath::Sin(Root->Sons[i]->Theta);
        Root->Sons[i]->Zcoordinate = Radio * FMath::Cos(Root->Sons[i]->Phi);
        WTemp += Root->Sons[i]->WTam;
        Cola.Enqueue(Root->Sons[i]);
    }

    while (!Cola.IsEmpty()) {
        ANodo * V;
        Cola.Dequeue(V);
        WTemp = V->WInicio;
        for (int i = 0; i < V->Sons.Num(); i++) {
            V->Sons[i]->Phi = V->Phi + DeltaPhi;
            V->Sons[i]->WTam = V->WTam * (float(V->Sons[i]->Hojas) / V->Hojas);
            V->Sons[i]->WInicio = WTemp;
            V->Sons[i]->Theta = WTemp + V->Sons[i]->WTam / 2;
            V->Sons[i]->Xcoordinate = Radio * FMath::Sin(V->Sons[i]->Phi) * FMath::Cos(V->Sons[i]->Theta);
            V->Sons[i]->Ycoordinate = Radio * FMath::Sin(V->Sons[i]->Phi) * FMath::Sin(V->Sons[i]->Theta);
            V->Sons[i]->Zcoordinate = Radio * FMath::Cos(V->Sons[i]->Phi);
            WTemp += V->Sons[i]->WTam;
            Cola.Enqueue(V->Sons[i]);
        }
    }
}

void ALayout1PMVRVisualization::UbicacionesLayout4(ANodo * Rama, int NivelDenso) {// en lugar de nre radio, deberia usar la variable radio
    NivelDenso = NivelMasDensoRama(Rama);//esta linea digamos se quejeucta afuera, o podria dejarlo a quito dentro, 
    //para calcular el deltaPhi, necesito examinar hacia arriba y hacia abajo del nivel maximo, seleccionar el que tenga mas niveles y dividir PI/2 entre esa cantidad de niveles, para calclar la variacion
    //y luego calcular el phi inicial, para el primer nivel despues de la raiz, y emepzar desde alli el layout
    /*ANodo * Root = Nodos[Nodos.Num() - 1];
    float DeltaPhi;
    float DeltaInicial;
    //hay que corregir el valor del nivel denso de la rama, en funcion de su posicion o nivel del nodo inicial de la rama, para que tenga sentido las divisiones sigueintes
    if (NivelDenso > Root->Altura / 2) {//estos calculos cambiaran cuano pempiece a limitar los angulos maximos para la esfera, tamibien falta agergar el umbral
        DeltaPhi = (PI / 2) / NivelDenso;
        DeltaInicial = DeltaPhi;//aplica desde el primer nivel
    }
    else {
        DeltaPhi = (PI / 2) / (Root->Altura - NivelDenso);
        DeltaInicial = PI / 2 - (NivelDenso - 1)*DeltaPhi;
    }*/
    ANodo * Root = Nodos[Nodos.Num() - 1];
    float DeltaPhi;
    float DeltaInicial;
    //hay que corregir el valor del nivel denso de la rama, en funcion de su posicion o nivel del nodo inicial de la rama, para que tenga sentido las divisiones sigueintes
    if (NivelDenso > Rama->Altura / 2) {//estos calculos cambiaran cuano pempiece a limitar los angulos maximos para la esfera, tamibien falta agergar el umbral
        DeltaPhi = (PI / 2) / NivelDenso;
        DeltaInicial = DeltaPhi;//aplica desde el primer nivel
        DeltaInicial = PI / 2 - (NivelDenso-1)*DeltaPhi;//esto esta bien tien el mismo resultdo que la linea anterior, pero esta si  funcionaria para los umbrales.
    }
    else {
        DeltaPhi = (PI / 2) / (Rama->Altura+1 - NivelDenso);
        DeltaInicial = PI / 2 - (NivelDenso - 1)*DeltaPhi;
    }
    if (DeltaPhi > PI / 30) {
        DeltaPhi = PI / 60;
        DeltaInicial = PI / 2 - (NivelDenso)*DeltaPhi;
    }
    Rama->Phi = DeltaInicial;
    Rama->Xcoordinate = Radio * FMath::Sin(Rama->Phi) * FMath::Cos(Rama->Theta);
    Rama->Ycoordinate = Radio * FMath::Sin(Rama->Phi) * FMath::Sin(Rama->Theta);
    Rama->Zcoordinate = Radio * FMath::Cos(Rama->Phi);
    TQueue<ANodo *> Cola;
    //la raiz es el ultimo nodo

    Cola.Enqueue(Rama);
    while (!Cola.IsEmpty()) {
        ANodo * V;
        Cola.Dequeue(V);
        float WTemp = V->WInicio;
        for (int i = 0; i < V->Sons.Num(); i++) {
            V->Sons[i]->Phi = V->Phi + DeltaPhi;
            V->Sons[i]->WTam = V->WTam * (float(V->Sons[i]->Hojas) / V->Hojas);
            V->Sons[i]->WInicio = WTemp;
            V->Sons[i]->Theta = WTemp + V->Sons[i]->WTam / 2;
            V->Sons[i]->Xcoordinate = Radio * FMath::Sin(V->Sons[i]->Phi) * FMath::Cos(V->Sons[i]->Theta);
            V->Sons[i]->Ycoordinate = Radio * FMath::Sin(V->Sons[i]->Phi) * FMath::Sin(V->Sons[i]->Theta);
            V->Sons[i]->Zcoordinate = Radio * FMath::Cos(V->Sons[i]->Phi);
            WTemp += V->Sons[i]->WTam;
            Cola.Enqueue(V->Sons[i]);
        }
    }
}


void ALayout1PMVRVisualization::Layout4() {//se entiende que sera usado despuesde un primer caclulo del radial layout
    //encontrar hasta que nivel quiero dividir las ramas, y las almaceno
    Calculos2();//no siemepre seran necesarios, reptirlos si ya se calcualron antes, salvo que cambien la estructura del arbol y se deban actualizar
    Calc();
    std::queue<ANodo *> Cola;
    ANodo * Root = Nodos[Nodos.Num() - 1];
    Cola.push(Root->Parent);
    Cola.push(Root->Sons[0]);
    Cola.push(Root->Sons[1]);
    for (int i = 0; i < Cola.size(); i++) {//quiza sea mejor usar un arreglo en lugar de una colas, es solo que lo anterior se reeempalzara por un bfs, que me debolvera algo, cola o array
        int NivelDensoRama = NivelMasDensoRama(Cola.front());
        UbicacionesLayout4(Cola.front(), NivelDensoRama);
        Cola.push(Cola.front());
        Cola.pop();
    }
    //luego de determinadas las posiciones para las ramas, se deben calcular las nuevas posiciones para los nodos, que nos hemos pasado.
    //es decir los padres de las ramas; por ahora solo estamos tomando desde el primer nivel, asi que no hya que repossicionar nada, pero si descendiera mas, deberia calcular en funionce de los hijos de forma recursiva

    //en cada rama seleccinada, busco su nivel mas denso.
    //aplicar el layout3 el algoritmo del layot 3 para cada rama, este algortmo solo modific los dela phi, generar n aversion conrta de ese algortimo, para que se ejecute en ramas, ya no teneinedo en cuenta el problema de la raiz
}

void ALayout1PMVRVisualization::Layout2(ANodo * Node, float NewRadio, int NivelExp, float PhiNode, float WInicioInicial, float WTamInicial) {
    TQueue<ANodo *> Cola;
    TQueue<ANodo *> Cola2;
    int HojasNivel = EncontrarNivel(Cola2, Node, NivelExp);
    //Node->Phi = PhiNode;
    //Node->WInicio = WInicioInicial;
    //Node->WTam = WTamInicial;
    //no pudo aplcarle a inicio de la rama, debo aplicar esto desde aqui, con el wtemp
    //para los lementod de la cola, o sus hijos de bo actualurza con los parametros ingresados
    float WTemp = WInicioInicial;
    //UE_LOG(LogClass, Log, TEXT("Layout2 Entrada %f, %f, %d"), WInicioInicial, WTamInicial, HojasNivel);
    while (!Cola2.IsEmpty()) {
        ANodo * V;
        Cola2.Dequeue(V);
        Cola.Enqueue(V);
        V->WTam = WTamInicial * (float(V->Hojas) / HojasNivel);//necesitare saber todas las hojas, de este nivel
        V->WInicio = WTemp;
        //UE_LOG(LogClass, Log, TEXT("Layout2 Id %d, Nivel Inicio %d, %f, %f"), V->Id, V->Nivel, V->WInicio, V->WTam);
        V->Theta = modFloat(WTemp + V->WTam / 2, 2*PI);
        V->Xcoordinate = Radio * FMath::Sin(V->Phi) * FMath::Cos(V->Theta);
        V->Ycoordinate = Radio * FMath::Sin(V->Phi) * FMath::Sin(V->Theta);
        V->Zcoordinate = Radio * FMath::Cos(V->Phi);
        WTemp = modFloat(WTemp + V->WTam, 2*PI);
    }

    while (!Cola.IsEmpty()) {
        ANodo * V;
        Cola.Dequeue(V);
        WTemp = V->WInicio;
        for (int i = 0; i < V->Sons.Num(); i++) {
            V->Sons[i]->WTam = V->WTam * (float(V->Sons[i]->Hojas) / V->Hojas);
            V->Sons[i]->WInicio = WTemp;
            //UE_LOG(LogClass, Log, TEXT("Layout2 Id %d, Nivel %d, %f, %f"), V->Sons[i]->Id, V->Sons[i]->Nivel, V->Sons[i]->WInicio, V->Sons[i]->WTam);
            V->Sons[i]->Theta = modFloat(WTemp + V->Sons[i]->WTam / 2, 2*PI);
            V->Sons[i]->Xcoordinate = Radio * FMath::Sin(V->Sons[i]->Phi) * FMath::Cos(V->Sons[i]->Theta);
            V->Sons[i]->Ycoordinate = Radio * FMath::Sin(V->Sons[i]->Phi) * FMath::Sin(V->Sons[i]->Theta);
            V->Sons[i]->Zcoordinate = Radio * FMath::Cos(V->Sons[i]->Phi);
            WTemp = modFloat(WTemp + V->Sons[i]->WTam, 2*PI);
            //UE_LOG(LogClass, Log, TEXT("THETA %f"), V->Sons[i]->Theta);
            Cola.Enqueue(V->Sons[i]);
        }
    }
}

void ALayout1PMVRVisualization::ExpandirLayout() {
    Calc();
    TArray<ANodo *> Ramas;
    ANodo * Root = Nodos[Nodos.Num() - 1];
    Ramas.Add(Root->Sons[0]);
    Ramas.Add(Root->Sons[1]);
    Ramas.Add(Root->Parent);
    TArray<float> WIniciales;
    TArray<float> WTams;
    TArray<int> NivelExpansion;
    for (int i = 0; i < Ramas.Num(); i++) {
        int idxIzq = mod((i - 1), Ramas.Num());
        int idxDer = mod((i + 1), Ramas.Num());
        //UE_LOG(LogClass, Log, TEXT("Rama Altura %d, indices %d, %d"), Ramas[i]->Altura, idxIzq, idxDer);
        int NivelExp = 0;
        if (Ramas[i]->Altura > Ramas[idxIzq]->Altura) {//rama de la izquierda
            //UE_LOG(LogClass, Log, TEXT("izquierda"));
            NivelExp = Ramas[idxIzq]->Altura + Ramas[i]->Nivel + 1;//marcamos a que nivel se hara la expansion, que luego se comparara con el de la siguiente rama
            //agregamos la mitad del espacion para poder distribuir ramas/
            WIniciales.Add(modFloat(Ramas[i]->WInicio - Ramas[idxIzq]->WTam / 2, 2*PI));//el nuevo inicial sera a la mitad del espacinde la rama, este menos deberia estar reducido,o sacar una especie de modulo de 2*PI
            WTams.Add(Ramas[i]->WTam + Ramas[idxIzq]->WTam / 2);//incrementamos el tamañó con lo correspondiente, 
        }
        else {//como no es mayor, los valores se mantinen, pero hay que agregar los valores en los arreglos 
            NivelExp = Ramas[i]->Altura + Ramas[i]->Nivel + 1;
            WIniciales.Add(Ramas[i]->WInicio);//aun que si pusiera 0 podria diferenciar, si modifique o no, para no recalcular en una rama, que no sea necesario, por ahora calcular en todas
            WTams.Add(Ramas[i]->WTam);
        }
        if (Ramas[i]->Altura > Ramas[idxDer]->Altura) {// rama de la derecha
            //UE_LOG(LogClass, Log, TEXT("derecha"));
            NivelExp = FMath::Max<float>(Ramas[idxDer]->Altura + Ramas[i]->Nivel + 1, NivelExp);
            WTams[i] += Ramas[idxDer]->WTam / 2;//incrementamos el tamaño en el arreglo de tams con lo correspondiente, 
        }
        NivelExpansion.Add(NivelExp);
        //calculos listos, ahora se procede a modificar cada rama, para esto se debe ir rama por rama, con los repectivos nuevos valores
        //profunidzar en cada rama, ahsta el nivel respectivo, a travez de un bfs, me quedo con los nodos de ese nivel en una cola, y luego procedo a recalcular las dimensiones
    }
    /*for (int i = 0; i < Ramas.Num(); i++) {
    }*/
    for (int i = 0; i < Ramas.Num(); i++) {
        //UE_LOG(LogClass, Log, TEXT("ramas %d, %f, %f"), NivelExpansion[i], Ramas[i]->WInicio, Ramas[i]->WTam);
        //UE_LOG(LogClass, Log, TEXT("ramasnuevo %d, %f, %f"), NivelExpansion[i], WIniciales[i], WTams[i]);
        //if(i != 2)
        Layout2(Ramas[i], Radio, NivelExpansion[i], Ramas[i]->Phi, WIniciales[i], WTams[i]);
    }
}

float ALayout1PMVRVisualization::EncontrarRadio1(float PhiNivelDenso, int CantidadNodosNivel) {//el radio de los nodos esta en la variable de la clase VRVisualization
    int NivelDenso;
    int CantidadNodos;
    NivelMasDenso(NivelDenso, CantidadNodos);
    float RadioNivel = 8 * CantidadNodosNivel*RadioNodos/(2*PI);
    //usar la cantidad de nodos no esta bien, produce mucho errores, ya que no hay la suficiente cantidad, o no estan acomodados en el lugar correcto, el conjunto feo es pureuba de ello
    //asumire que quiero una separacion casi nula en este nivel, de ser necesario agregare un sumando mas
    float RadioArbol = RadioNivel / FMath::Sin(PhiNivelDenso);
    
    return RadioArbol;
}

float ALayout1PMVRVisualization::EncontrarRadio2(float PhiUltimoNivel) {
    ANodo * Root = Nodos[Nodos.Num() - 1];
    float RadioNivel = Root->Hojas*2*RadioNodos / (2 * PI);
    //float RadioNivel = (Root->Hojas*2*RadioNodos + Root->Hojas*1.0f)/ (2 * PI);
    float RadioArbol = RadioNivel / FMath::Sin(PhiUltimoNivel);
    return RadioArbol;
}

void ALayout1PMVRVisualization::ReducionDistanciaHojas() {
    //reducir la arista solo si es mas grande, del espacio o tamaño que deseo, esto avita que expanda, als que esten muy cortas
    ANodo * Root = Nodos[Nodos.Num() - 1];
    float Separacion = 2*2 * PI / Root->Hojas;
    //recorrer el arbol, e ir comprobando los hijos si son hojas o no
    std::queue<ANodo *> Cola;
    Cola.push(Root->Parent);
    Cola.push(Root->Sons[0]);
    Cola.push(Root->Sons[1]);
    while(!Cola.empty()){
        ANodo * V = Cola.front();
        Cola.pop();
        for (int i = 0; i < V->Sons.Num(); i++) {
            ANodo * Hijito = V->Sons[i];
            if(Hijito->Valid){
                //UE_LOG(LogClass, Log, TEXT("Nodo a modificar: %d"), Hijito->Id);
                float DistanciaAngular = V->Theta - Hijito->Theta;
                if (abs(DistanciaAngular) > Separacion) {//mas grande de lo que deseo
                    if (DistanciaAngular <= 0) {//si esta a la derecha o izquierda
                        Hijito->Theta = modFloat(V->Theta + Separacion, 2 * PI);
                        Hijito->Xcoordinate = Radio * FMath::Sin(Hijito->Phi) * FMath::Cos(Hijito->Theta);
                        Hijito->Ycoordinate = Radio * FMath::Sin(Hijito->Phi) * FMath::Sin(Hijito->Theta);
                        //Hijito->Zcoordinate = Radio * FMath::Cos(Hijito->Phi);
                    }
                    else {
                        Hijito->Theta = modFloat(V->Theta - Separacion, 2 * PI);
                        Hijito->Xcoordinate = Radio * FMath::Sin(Hijito->Phi) * FMath::Cos(Hijito->Theta);
                        Hijito->Ycoordinate = Radio * FMath::Sin(Hijito->Phi) * FMath::Sin(Hijito->Theta);
                        //Hijito->Zcoordinate = Radio * FMath::Cos(Hijito->Phi);
                    }
                }
            }
            Cola.push(V->Sons[i]);
        }
    }
}

void ALayout1PMVRVisualization::AplicarTraslacion(FVector Traslacion) {

}

void ALayout1PMVRVisualization::AplicarRotacionRelativaANodo(ANodo* NodoReferencia, FVector PuntoReferencia) {

}

void ALayout1PMVRVisualization::AplicarTraslacionEsferica(float TraslacionPhi, float TraslacionTheta) {
    //UE_LOG(LogClass, Log, TEXT("DeltasEsfericos = %f, %f"), TraslacionPhi, TraslacionTheta);
    for (int i = 0; i < NodosSeleccionados.Num(); i++) {
        NodosSeleccionados[i]->Theta = modFloat(NodosSeleccionados[i]->Theta + TraslacionTheta, 2*PI);
        NodosSeleccionados[i]->Phi = FMath::Clamp(NodosSeleccionados[i]->Phi + TraslacionPhi, 0.0f, PI);
        NodosSeleccionados[i]->Xcoordinate = Radio * FMath::Sin(NodosSeleccionados[i]->Phi) * FMath::Cos(NodosSeleccionados[i]->Theta);
        NodosSeleccionados[i]->Ycoordinate = Radio * FMath::Sin(NodosSeleccionados[i]->Phi) * FMath::Sin(NodosSeleccionados[i]->Theta);
        NodosSeleccionados[i]->Zcoordinate = Radio * FMath::Cos(NodosSeleccionados[i]->Phi);
        FVector NuevaPosicion;
        NuevaPosicion.X = NodosSeleccionados[i]->Xcoordinate;
        NuevaPosicion.Y = NodosSeleccionados[i]->Ycoordinate;
        NuevaPosicion.Z = NodosSeleccionados[i]->Zcoordinate;
        NodosSeleccionados[i]->SetActorRelativeLocation(NuevaPosicion);
    }
}

FVector ALayout1PMVRVisualization::InterseccionLineaSuperficie() {//retorna en espacio local
    FVector Punto = RightController->GetComponentTransform().GetLocation();
    Punto = GetTransform().InverseTransformPosition(Punto);
    FVector Vector = RightController->GetForwardVector();
    Vector = GetTransform().InverseTransformVector(Vector);

    float A = (Vector.X*Vector.X + Vector.Y*Vector.Y + Vector.Z*Vector.Z);//a/R^2
    float B = 2*(Punto.X*Vector.X + Punto.Y*Vector.Y + Punto.Z*Vector.Z);//2*b/R^2
    float C = (Punto.X*Punto.X + Punto.Y*Punto.Y + Punto.Z*Punto.Z)-(Radio*Radio);//c/R^2;
    float Discriminante = B*B - 4*A*C;
    //UE_LOG(LogClass, Log, TEXT("Punto = %f, %f, %f"), Punto.X, Punto.Y, Punto.Z);
    //UE_LOG(LogClass, Log, TEXT("Vector = %f, %f, %f"), Vector.X, Vector.Y, Vector.Z);
    //UE_LOG(LogClass, Log, TEXT("(A,B,C) = %f, %f, %f"), A, B, C);
    //UE_LOG(LogClass, Log, TEXT("Determinante = %f"), Discriminante);
    if (Discriminante < 0) {
        return FVector::ZeroVector;
        //return Punto + DistanciaLaserMaxima*Vector;
        //DrawDebugLine(GetWorld(), Punto, Punto + DistanciaLaser*Vector, FColor::Red, false, -1.0f, 0, 1.0f);// los calculos estan perfectos
        //dibujar en rojo, el maximo alcance
    }
    else if (Discriminante > 0) {
        float T1 = (-B + FMath::Sqrt(Discriminante)) / (2 * A);
        float T2 = (-B - FMath::Sqrt(Discriminante)) / (2 * A);
        //UE_LOG(LogClass, Log, TEXT("(T1,T2) = %f, %f"), T1, T2);
        FVector P1 = Punto + T1*Vector;
        FVector P2 = Punto + T2*Vector;
        if (T1 >= 0 && T2 >= 0) {
            if ((P1 - Punto).Size() < (P2 - Punto).Size()) {
                //DrawDebugLine(GetWorld(), Punto, P1, FColor::Blue, false, -1.0f, 0, 1.0f);// los calculos estan perfectos
                return P1;
                //return GetTransform().InverseTransformPosition(P1);
            }
            else {
                return P2;
                //DrawDebugLine(GetWorld(), Punto, P2, FColor::Blue, false, -1.0f, 0, 1.0f);// los calculos estan perfectos
                //return GetTransform().InverseTransformPosition(P2);
            }
        }
        else if (T1 >= 0) {
            return P1;
            //return GetTransform().InverseTransformPosition(P1);
            //DrawDebugLine(GetWorld(), Punto, P1, FColor::Blue, false, -1.0f, 0, 1.0f);// los calculos estan perfectos
        }
        else if (T2 >= 0) {
            return P2;
            //return GetTransform().InverseTransformPosition(P2);
            //DrawDebugLine(GetWorld(), Punto, P2, FColor::Blue, false, -1.0f, 0, 1.0f);// los calculos estan perfectos
        }
    }
    else{
        float T = (-B + FMath::Sqrt(Discriminante)) / (2 * A);
        //DrawDebugLine(GetWorld(), Punto, Punto + T*Vector, FColor::Green, false, -1.0f, 0, 1.0f);// los calculos estan perfectos
        //return GetTransform().InverseTransformPosition(Punto + T*Vector);
        return Punto + T*Vector;
    } 
    //return Punto + DistanciaLaserMaxima*Vector;
    return FVector::ZeroVector;
    //si retorno el vector 0, eso quiere decir que no encontre interseccion, por lo tanto, cuadno reciba este dato, debo evaluar que hacer con el
}

void ALayout1PMVRVisualization::TraslacionConNodoGuia() {//retorna en espacio local
    ImpactPoint = InterseccionLineaSuperficie();
    //UE_LOG(LogClass, Log, TEXT("Impact = %f, %f, %f"), ImpactPoint.X, ImpactPoint.Y, ImpactPoint.Z);
    if (ImpactPoint != FVector::ZeroVector) {
        //FVector ImpactPointRelative = GetTransform().InverseTransformPosition(ImpactPoint);
        float ImpactPhi = FMath::Acos(ImpactPoint.Z / Radio);
        float ImpactTheta = FMath::Acos(ImpactPoint.X / (Radio*FMath::Sin(ImpactPhi)));
        //no he convertido los puntos a espacion local
        if (ImpactPoint.Y < 0) {
            ImpactTheta = 2 * PI - ImpactTheta;
        }
        //UE_LOG(LogClass, Log, TEXT("ImpactEsfericos = %f, %f"), ImpactPhi, ImpactTheta);
        //UE_LOG(LogClass, Log, TEXT("NodoEsfericos = %f, %f"), NodoGuia->Phi, NodoGuia->Theta);
        AplicarTraslacionEsferica(ImpactPhi - NodoGuia->Phi, ImpactTheta - NodoGuia->Theta);
        //AplicarTraslacion(ImpactPoint - NodoGuia->GetActorLocation());
        Usuario->CambiarLaser(1);
        Usuario->CambiarPuntoFinal(GetTransform().TransformPosition(ImpactPoint));
        //dibujar laser apropiado
    }
    else {
        Usuario->CambiarLaser(2);
        FVector Punto = RightController->GetComponentTransform().GetLocation();
        Punto = GetTransform().InverseTransformPosition(Punto);
        FVector Vector = RightController->GetForwardVector();
        Vector = GetTransform().InverseTransformVector(Vector);
        Usuario->CambiarPuntoFinal(Punto + DistanciaLaserMaxima*Vector);
        //dibujarLaserApropiado, aqui funciona bien
    }
}

void ALayout1PMVRVisualization::CreateSphereTemplate(int Precision) {
    UE_LOG(LogClass, Log, TEXT("Creando esfera"));
    float DeltaPhi = 1.10714872;
    float DeltaTheta = PI * 72 / 180;
    float DiffTheta = PI * 36 / 180;
    VerticesNodoTemplate.push_back(FVector(0.0f, 0.0f, 1.0f));//puntos para radio uno
    NormalsNodoTemplate.push_back(FVector(0.0f, 0.0f, 1.0f));//puntos para radio uno
    //Tangents.Add(FProcMeshTangent(1.0f, 0.0f, 0.0f));
    TangentsNodoTemplate.push_back(FProcMeshTangent(1.0f * FMath::Sin(PI/2)*FMath::Cos(PI/2), 1.0f * FMath::Sin(PI/2)*FMath::Sin(PI/2), 1.0f * FMath::Cos(PI/2)));
    VerticesPNodoTemplate.push_back(FVector(0.0f, 0.0f, 1.0f));
    UV0NodoTemplate.push_back(FVector2D(0.0f,1.0f));
	VertexColorsNodoTemplate.push_back(FLinearColor(0.75, 0.75, 0.75, 1.0));
    for (int i = 0; i < 5; i++) {
        VerticesPNodoTemplate.push_back(FVector(DeltaPhi, i*DeltaTheta, 1.0f));//esto es en esferico, no se para que lo uso desues, creo que para las divisiones para aumentr la suavidad
        FVector posicion;
        posicion.X = 1.0f * FMath::Sin(DeltaPhi) * FMath::Cos(i*DeltaTheta);
        posicion.Y = 1.0f * FMath::Sin(DeltaPhi) * FMath::Sin(i*DeltaTheta);
        posicion.Z = 1.0f * FMath::Cos(DeltaPhi);
        //hasta aqui posicion esta como unitario, puede servir para el verctor de normales
        VerticesNodoTemplate.push_back(posicion);
        NormalsNodoTemplate.push_back(posicion);
        TangentsNodoTemplate.push_back(FProcMeshTangent(1.0f * FMath::Sin(PI/2)*FMath::Cos(i*DeltaTheta + PI/2), 1.0f * FMath::Sin(PI/2)*FMath::Sin(i*DeltaTheta + PI/2), 1.0f * FMath::Cos(PI/2)));
        UV0NodoTemplate.push_back(FVector2D(i*DeltaTheta/(2*PI), 1 - DeltaPhi/PI));
        VertexColorsNodoTemplate.push_back(FLinearColor(0.75, 0.75, 0.75, 1.0));
    }
    for (int i = 0; i < 5; i++) {
        VerticesPNodoTemplate.push_back(FVector(PI - DeltaPhi, i*DeltaTheta + DiffTheta, 1.0f));
        FVector posicion;
        posicion.X = 1.0f * FMath::Sin(PI - DeltaPhi) * FMath::Cos(i*DeltaTheta + DiffTheta);
        posicion.Y = 1.0f * FMath::Sin(PI - DeltaPhi) * FMath::Sin(i*DeltaTheta + DiffTheta);
        posicion.Z = 1.0f * FMath::Cos(PI - DeltaPhi);
        //hasta aqui posicion esta como unitario, puede servir para el verctor de normales
        NormalsNodoTemplate.push_back(posicion);
        TangentsNodoTemplate.push_back(FProcMeshTangent(1.0f * FMath::Sin(PI/2)*FMath::Cos(i*DeltaTheta + PI/2), 1.0f * FMath::Sin(PI/2)*FMath::Sin(i*DeltaTheta + PI/2), 1.0f * FMath::Cos(PI/2)));
        VerticesNodoTemplate.push_back(posicion);
        UV0NodoTemplate.push_back(FVector2D((i*DeltaTheta + DiffTheta)/(2*PI), 1 - (PI - DeltaPhi)/PI));
        VertexColorsNodoTemplate.push_back(FLinearColor(0.75, 0.75, 0.75, 1.0));
    }
    VerticesNodoTemplate.push_back(FVector(0.0, 0.0f, -1.0f));//puntos para radio uno
    NormalsNodoTemplate.push_back(FVector(0.0, 0.0f, -1.0f));//puntos para radio uno
    //Tangents.Add(FProcMeshTangent(-1.0f, 0.0f, 0.0f));
    TangentsNodoTemplate.push_back(FProcMeshTangent(1.0f * FMath::Sin(PI/2)*FMath::Cos(PI/2), 1.0f * FMath::Sin(PI/2)*FMath::Sin(PI/2), 1.0f * FMath::Cos(PI/2)));
    VerticesPNodoTemplate.push_back(FVector(PI, 0.0f, 1.0f));
    UV0NodoTemplate.push_back(FVector2D(0.0f,0.0f));
    VertexColorsNodoTemplate.push_back(FLinearColor(0.75, 0.75, 0.75, 1.0));

    //Agregados vertices y normales, faltarian agregar las tangentes

    //Tirangulos superiores
    TriangulosNodoTemplate.push_back(Triangulo(VerticesNodoTemplate[0], VerticesNodoTemplate[1], VerticesNodoTemplate[2], 0, 1, 2, 0, 0));
    TriangulosNodoTemplate.push_back(Triangulo(VerticesNodoTemplate[0], VerticesNodoTemplate[2], VerticesNodoTemplate[3], 0, 2, 3, 0, 0));
    TriangulosNodoTemplate.push_back(Triangulo(VerticesNodoTemplate[0], VerticesNodoTemplate[3], VerticesNodoTemplate[4], 0, 3, 4, 0, 0));
    TriangulosNodoTemplate.push_back(Triangulo(VerticesNodoTemplate[0], VerticesNodoTemplate[4], VerticesNodoTemplate[5], 0, 4, 5, 0, 0));
    TriangulosNodoTemplate.push_back(Triangulo(VerticesNodoTemplate[0], VerticesNodoTemplate[5], VerticesNodoTemplate[1], 0, 5, 1, 0, 0));

    //triangulos medios
    TriangulosNodoTemplate.push_back(Triangulo(VerticesNodoTemplate[1], VerticesNodoTemplate[6], VerticesNodoTemplate[2], 1, 6, 2, 1, 1));
    TriangulosNodoTemplate.push_back(Triangulo(VerticesNodoTemplate[2], VerticesNodoTemplate[6], VerticesNodoTemplate[7], 2, 6, 7, 1, 0));
    TriangulosNodoTemplate.push_back(Triangulo(VerticesNodoTemplate[2], VerticesNodoTemplate[7], VerticesNodoTemplate[3], 2, 7, 3, 1, 1));
    TriangulosNodoTemplate.push_back(Triangulo(VerticesNodoTemplate[3], VerticesNodoTemplate[7], VerticesNodoTemplate[8], 3, 7, 8, 1, 0));
    TriangulosNodoTemplate.push_back(Triangulo(VerticesNodoTemplate[3], VerticesNodoTemplate[8], VerticesNodoTemplate[4], 3, 8, 4, 1, 1));
    TriangulosNodoTemplate.push_back(Triangulo(VerticesNodoTemplate[4], VerticesNodoTemplate[8], VerticesNodoTemplate[9], 4, 8, 9, 1, 0));
    TriangulosNodoTemplate.push_back(Triangulo(VerticesNodoTemplate[4], VerticesNodoTemplate[9], VerticesNodoTemplate[5], 4, 9, 5, 1, 1));
    TriangulosNodoTemplate.push_back(Triangulo(VerticesNodoTemplate[5], VerticesNodoTemplate[9], VerticesNodoTemplate[10], 5, 9, 10, 1, 0));
    TriangulosNodoTemplate.push_back(Triangulo(VerticesNodoTemplate[5], VerticesNodoTemplate[10], VerticesNodoTemplate[1], 5, 10, 1, 1, 1));
    TriangulosNodoTemplate.push_back(Triangulo(VerticesNodoTemplate[1], VerticesNodoTemplate[10], VerticesNodoTemplate[6], 1, 10, 6, 1, 0));

    //triangulos inferiores
    TriangulosNodoTemplate.push_back(Triangulo(VerticesNodoTemplate[6], VerticesNodoTemplate[11], VerticesNodoTemplate[7], 6, 11, 7, 2, 1));
    TriangulosNodoTemplate.push_back(Triangulo(VerticesNodoTemplate[7], VerticesNodoTemplate[11], VerticesNodoTemplate[8], 7, 11, 8, 2, 1));
    TriangulosNodoTemplate.push_back(Triangulo(VerticesNodoTemplate[8], VerticesNodoTemplate[11], VerticesNodoTemplate[9], 8, 11, 9, 2, 1));
    TriangulosNodoTemplate.push_back(Triangulo(VerticesNodoTemplate[9], VerticesNodoTemplate[11], VerticesNodoTemplate[10], 9, 11, 10, 2, 1));
    TriangulosNodoTemplate.push_back(Triangulo(VerticesNodoTemplate[10], VerticesNodoTemplate[11], VerticesNodoTemplate[6], 10, 11, 6, 2, 1));

    while (Precision) {
        DividirTriangulos();
        Precision--;
    }
}

FVector ALayout1PMVRVisualization::PuntoTFromAToB(FVector a, FVector b, float t) {
    FVector direccion = b - a;
    FVector punto = a + direccion*t;
    //normalizar a esfera de radio 1
    punto = punto.GetSafeNormal();
    return punto;
}

FVector ALayout1PMVRVisualization::PuntoTFromAToBEsferico(FVector a, FVector b, float t) {
    FVector direccion = b - a;
    FVector punto = a + direccion*t;
    //normalizar a esfera de radio 1
    punto = punto.GetSafeNormal();
    float phi = FMath::Acos(punto.Z);
    float theta = FMath::Asin(punto.Y / FMath::Sin(phi));
    FVector puntoesferico (phi, theta, 1.0f);
    return puntoesferico;
}

void ALayout1PMVRVisualization::DividirTriangulo(Triangulo t) {
    FVector D = PuntoTFromAToB(t.C, t.A, 0.5);
    VerticesNodoTemplate.push_back(D);//0.5 es la mitad
    int IdD = VerticesNodoTemplate.size() - 1;
    FVector DP = PuntoTFromAToBEsferico(t.C, t.A, 0.5);
    VerticesPNodoTemplate.push_back(DP);//0.5 es la mitad
    NormalsNodoTemplate.push_back(D);
    TangentsNodoTemplate.push_back(FProcMeshTangent(1.0f * FMath::Sin(PI/2)*FMath::Cos(DP.Y + PI/2), 1.0f * FMath::Sin(PI/2)*FMath::Sin(DP.Y + PI/2), 1.0f * FMath::Cos(PI/2)));
    UV0NodoTemplate.push_back(FVector2D(DP.Y/(2*PI), 1 - DP.X/PI));
    VertexColorsNodoTemplate.push_back(FLinearColor(0.75, 0.75, 0.75, 1.0));

    FVector E = PuntoTFromAToB(t.B, t.A, 0.5);
    VerticesNodoTemplate.push_back(E);//0.5 es la mitad
    int IdE = VerticesNodoTemplate.size() - 1;
    FVector EP = PuntoTFromAToBEsferico(t.B, t.A, 0.5);
    VerticesPNodoTemplate.push_back(EP);//0.5 es la mitad
    NormalsNodoTemplate.push_back(E);
    TangentsNodoTemplate.push_back(FProcMeshTangent(1.0f * FMath::Sin(PI/2)*FMath::Cos(EP.Y + PI/2), 1.0f * FMath::Sin(PI/2)*FMath::Sin(EP.Y + PI/2), 1.0f * FMath::Cos(PI/2)));
    UV0NodoTemplate.push_back(FVector2D(EP.Y/(2*PI), 1 - EP.X/PI));
    VertexColorsNodoTemplate.push_back(FLinearColor(0.75, 0.75, 0.75, 1.0));

    FVector F = PuntoTFromAToB(t.C, t.B, 0.5);
    VerticesNodoTemplate.push_back(F);//0.5 es la mitad
    int IdF = VerticesNodoTemplate.size() - 1;
    FVector FP = PuntoTFromAToBEsferico(t.C, t.B, 0.5);
    VerticesPNodoTemplate.push_back(FP);//0.5 es la mitad
    NormalsNodoTemplate.push_back(F);
    TangentsNodoTemplate.push_back(FProcMeshTangent(1.0f * FMath::Sin(PI/2)*FMath::Cos(FP.Y + PI/2), 1.0f * FMath::Sin(PI/2)*FMath::Sin(FP.Y + PI/2), 1.0f * FMath::Cos(PI/2)));
    UV0NodoTemplate.push_back(FVector2D(FP.Y/(2*PI), 1 - FP.X/PI));
    VertexColorsNodoTemplate.push_back(FLinearColor(0.75, 0.75, 0.75, 1.0));

    //if (VerticesP[t.IdA].X == VerticesP[t.IdC].X) {//tringulo hacia abajo
    if (t.Orientacion) {//tringulo hacia abajo
        TriangulosNodoTemplate.push_back(Triangulo(t.A, E, D, t.IdA, IdE, IdD, t.Nivel*2, 0));
        TriangulosNodoTemplate.push_back(Triangulo(D, E, F, IdD, IdE, IdF, t.Nivel*2, 0));
        TriangulosNodoTemplate.push_back(Triangulo(D, F, t.C, IdD, IdF, t.IdC, t.Nivel*2, 0));
        TriangulosNodoTemplate.push_back(Triangulo(E, t.B, F, IdE, t.IdB, IdF, t.Nivel*2 + 1, 1));
    }
    else {//triangulo hacia arriba, por lo tanto c y b esta a la misma algutar, a arriba
        TriangulosNodoTemplate.push_back(Triangulo(t.A, E, D, t.IdA, IdE, IdD, t.Nivel*2, 0));
        TriangulosNodoTemplate.push_back(Triangulo(E, t.B, F, IdE, t.IdB, IdF, t.Nivel*2 + 1, 1));
        TriangulosNodoTemplate.push_back(Triangulo(E, F, D, IdE, IdF, IdD, t.Nivel*2+1, 1));
        TriangulosNodoTemplate.push_back(Triangulo(D, F, t.C, IdD, IdF, t.IdC, t.Nivel*2 + 1, 1));
    }
}

void ALayout1PMVRVisualization::DividirTriangulos() {
    int n = TriangulosNodoTemplate.size();
    for (int i = 0; i < n; i++) {
        DividirTriangulo(TriangulosNodoTemplate[0]);
        TriangulosNodoTemplate.erase(TriangulosNodoTemplate.begin());
    }
}


void ALayout1PMVRVisualization::AddNodoToMesh(FVector Posicion, float RadioNodo, int NumNodo) {
    for (int i = 0; i < VerticesNodoTemplate.size(); i++) {
        VerticesNodos.Add(VerticesNodoTemplate[i] * RadioNodo + Posicion);
    }
    for (int i = 0; i < VerticesPNodoTemplate.size(); i++) {
        VerticesPNodos.Add(VerticesPNodoTemplate[i]);
    }
    for (int i = 0; i < TriangulosNodoTemplate.size(); i++) {
        TrianglesNodos.Add(TriangulosNodoTemplate[i].IdC + NumNodo * VerticesNodoTemplate.size());
        TrianglesNodos.Add(TriangulosNodoTemplate[i].IdB + NumNodo * VerticesNodoTemplate.size());
        TrianglesNodos.Add(TriangulosNodoTemplate[i].IdA + NumNodo * VerticesNodoTemplate.size());
    }
    for (int i = 0; i < NormalsNodoTemplate.size(); i++) {
        NormalsNodos.Add(NormalsNodoTemplate[i]);
    }
    for (int i = 0; i < UV0NodoTemplate.size(); i++) {
        UV0Nodos.Add(UV0NodoTemplate[i]);
    }
    for (int i = 0; i < TangentsNodoTemplate.size(); i++) {
        TangentsNodos.Add(TangentsNodoTemplate[i]);
    }
    for (int i = 0; i < VertexColorsNodoTemplate.size(); i++) {
        VertexColorsNodos.Add(VertexColorsNodoTemplate[i]);
    }
}

void ALayout1PMVRVisualization::AddNodoToMesh(FVector Posicion, float RadioNodo, FLinearColor Color, int NumNodo) {
    for (int i = 0; i < VerticesNodoTemplate.size(); i++) {
        VerticesNodos.Add(VerticesNodoTemplate[i] * RadioNodo + Posicion);
    }
    for (int i = 0; i < VerticesPNodoTemplate.size(); i++) {
        VerticesPNodos.Add(VerticesPNodoTemplate[i]);
    }
    for (int i = 0; i < TriangulosNodoTemplate.size(); i++) {
        TrianglesNodos.Add(TriangulosNodoTemplate[i].IdC + NumNodo * VerticesNodoTemplate.size());
        TrianglesNodos.Add(TriangulosNodoTemplate[i].IdB + NumNodo * VerticesNodoTemplate.size());
        TrianglesNodos.Add(TriangulosNodoTemplate[i].IdA + NumNodo * VerticesNodoTemplate.size());
    }
    for (int i = 0; i < NormalsNodoTemplate.size(); i++) {
        NormalsNodos.Add(NormalsNodoTemplate[i]);
    }
    for (int i = 0; i < UV0NodoTemplate.size(); i++) {
        UV0Nodos.Add(UV0NodoTemplate[i]);
    }
    for (int i = 0; i < TangentsNodoTemplate.size(); i++) {
        TangentsNodos.Add(TangentsNodoTemplate[i]);
    }
    for (int i = 0; i < VertexColorsNodoTemplate.size(); i++) {
        VertexColorsNodos.Add(Color);
    }
}

void ALayout1PMVRVisualization::UpdatePosicionNodoMesh(int IdNodo, FVector NewPosition) {
    for (int i = 0; i < VerticesNodoTemplate.size(); i++) {
        VerticesNodos[i + IdNodo * VerticesNodoTemplate.size()] = VerticesNodoTemplate[i] * Nodos[IdNodo]->Radio + NewPosition;
    }
}

void ALayout1PMVRVisualization::CreateNodosMesh() {
	NodosMesh->CreateMeshSection_LinearColor(0, VerticesNodos, TrianglesNodos, NormalsNodos, UV0Nodos, VertexColorsNodos, TangentsNodos, false);
 
	NodosMesh->ContainsPhysicsTriMeshData(false);

    if (NodosMeshMaterial) {
        NodosMesh->SetMaterial(0, NodosMeshMaterial);
    }
}

void ALayout1PMVRVisualization::UpdateNodosMesh() {
	NodosMesh->UpdateMeshSection_LinearColor(0, VerticesNodos, NormalsNodos, UV0Nodos, VertexColorsNodos, TangentsNodos);
}

void ALayout1PMVRVisualization::AddAristaToMesh(FVector Source, FVector Target, int RadioArista, int NumArista) {
    //precision debe ser mayor a 3, represante el numero de lados del cilindro
    //la precision deberia ser de forma general,asi lo puedo usar para determinar la cantidad de vertices añadidos
    FVector Direccion = (Target - Source).GetSafeNormal();
    //calculando vertices superioes

    float phi = PI/2;
    float theta = FMath::Asin(Direccion.Y / FMath::Sin(phi)) + PI/2;
    FVector VectorU;
    VectorU.X = 1.0f * FMath::Sin(phi) * FMath::Cos(theta);
    VectorU.Y = 1.0f * FMath::Sin(phi) * FMath::Sin(theta);
    VectorU.Z = 1.0f * FMath::Cos(phi);
    FVector VectorV = FVector::CrossProduct(Direccion, VectorU).GetSafeNormal();
    float DeltaTheta = 2 * PI / PrecisionAristas;

    for (int i = 0; i < PrecisionAristas; i++) {
        VerticesAristas.Add(Target + (VectorU*FMath::Cos(i*DeltaTheta) + VectorV*FMath::Sin(i*DeltaTheta))*RadioArista);
        NormalsAristas.Add((VectorU*FMath::Cos(i*DeltaTheta) + VectorV*FMath::Sin(i*DeltaTheta)).GetSafeNormal());
        TangentsAristas.Add(FProcMeshTangent(1.0f * FMath::Sin(PI/2)*FMath::Cos(i*DeltaTheta + PI/2), 1.0f * FMath::Sin(PI/2)*FMath::Sin(i*DeltaTheta + PI/2), 1.0f * FMath::Cos(PI/2)));
        UV0Aristas.Add(FVector2D(i*(1.0f/PrecisionAristas), 1));
        VertexColorsAristas.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
    }
    for (int i = 0; i < PrecisionAristas; i++) {
        VerticesAristas.Add(Source + (VectorU*FMath::Cos(i*DeltaTheta) + VectorV*FMath::Sin(i*DeltaTheta))*RadioArista);
        NormalsAristas.Add((VectorU*FMath::Cos(i*DeltaTheta) + VectorV*FMath::Sin(i*DeltaTheta)).GetSafeNormal());
        TangentsAristas.Add(FProcMeshTangent(1.0f * FMath::Sin(PI/2)*FMath::Cos(i*DeltaTheta + PI/2), 1.0f * FMath::Sin(PI/2)*FMath::Sin(i*DeltaTheta + PI/2), 1.0f * FMath::Cos(PI/2)));
        UV0Aristas.Add(FVector2D(i*(1.0f/PrecisionAristas), 0));
        //VertexColorsAristas.Add(FLinearColor(0.0, 0.75, 0.75, 1.0));
        VertexColorsAristas.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
    }

    for (int i = 0; i < PrecisionAristas; i++) {
        TrianglesAristas.Add((i+1)%PrecisionAristas + NumArista * PrecisionAristas*2);
        TrianglesAristas.Add(PrecisionAristas + (i + 1)%PrecisionAristas + NumArista * PrecisionAristas*2);
        TrianglesAristas.Add(i + NumArista * PrecisionAristas*2);

        TrianglesAristas.Add(PrecisionAristas + i + NumArista * PrecisionAristas*2);
        TrianglesAristas.Add(i + NumArista * PrecisionAristas*2);
        TrianglesAristas.Add(PrecisionAristas + (i+1)%PrecisionAristas + NumArista * PrecisionAristas*2);
    }
}

void ALayout1PMVRVisualization::CreateAristasMesh() {
	AristasMesh->CreateMeshSection_LinearColor(0, VerticesAristas, TrianglesAristas, NormalsAristas, UV0Aristas, VertexColorsAristas, TangentsAristas, false);
        // Enable collision data
	AristasMesh->ContainsPhysicsTriMeshData(false);

    if (AristasMeshMaterial) {
        AristasMesh->SetMaterial(0, AristasMeshMaterial);
    }
}

void ALayout1PMVRVisualization::UpdateAristasMesh() {
	AristasMesh->UpdateMeshSection_LinearColor(0, VerticesAristas, NormalsAristas, UV0Aristas, VertexColorsAristas, TangentsAristas);
}

void ALayout1PMVRVisualization::UpdatePosicionesAristaMesh(int IdArista, FVector Source, FVector Target) {
    FVector Direccion = (Target - Source).GetSafeNormal();
    //calculando vertices superioes

    float phi = PI/2;
    float theta = FMath::Asin(Direccion.Y / FMath::Sin(phi)) + PI/2;
    FVector VectorU;
    VectorU.X = 1.0f * FMath::Sin(phi) * FMath::Cos(theta);
    VectorU.Y = 1.0f * FMath::Sin(phi) * FMath::Sin(theta);
    VectorU.Z = 1.0f * FMath::Cos(phi);
    FVector VectorV = FVector::CrossProduct(Direccion, VectorU).GetSafeNormal();
    float DeltaTheta = 2 * PI / PrecisionAristas;

    for (int i = 0; i < PrecisionAristas; i++) {
        VerticesAristas[i + IdArista * PrecisionAristas*2] = Target + (VectorU*FMath::Cos(i*DeltaTheta) + VectorV*FMath::Sin(i*DeltaTheta))*RadioAristas;
        NormalsAristas[i + IdArista * PrecisionAristas*2] = (VectorU*FMath::Cos(i*DeltaTheta) + VectorV*FMath::Sin(i*DeltaTheta)).GetSafeNormal();
        TangentsAristas[i + IdArista * PrecisionAristas*2] = FProcMeshTangent(1.0f * FMath::Sin(PI/2)*FMath::Cos(i*DeltaTheta + PI/2), 1.0f * FMath::Sin(PI/2)*FMath::Sin(i*DeltaTheta + PI/2), 1.0f * FMath::Cos(PI/2));
    }
    for (int i = 0; i < PrecisionAristas; i++) {
        VerticesAristas[i + PrecisionAristas + IdArista * PrecisionAristas*2] = Source + (VectorU*FMath::Cos(i*DeltaTheta) + VectorV*FMath::Sin(i*DeltaTheta))*RadioAristas;
        NormalsAristas[i + PrecisionAristas + IdArista * PrecisionAristas*2] = (VectorU*FMath::Cos(i*DeltaTheta) + VectorV*FMath::Sin(i*DeltaTheta)).GetSafeNormal();
        TangentsAristas[i + PrecisionAristas + IdArista * PrecisionAristas*2] = FProcMeshTangent(1.0f * FMath::Sin(PI/2)*FMath::Cos(i*DeltaTheta + PI/2), 1.0f * FMath::Sin(PI/2)*FMath::Sin(i*DeltaTheta + PI/2), 1.0f * FMath::Cos(PI/2));
    }
}


//reemplazar fors por entradas directas




