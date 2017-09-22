// Fill out your copyright notice in the Description page of Project Settings.

#include "NJVR.h"
#include "Layout1VRVisualization.h"
#include "Nodo.h"
#include "Arista.h"
#include <stack>
#include <queue>

ALayout1VRVisualization::ALayout1VRVisualization(){

    Radio = 300.0f;
    PhiMax = PI;
    PhiMin = 0;
    PhiMax = FMath::DegreesToRadians(150);
    PhiMin = FMath::DegreesToRadians(40);
}

void ALayout1VRVisualization::BeginPlay() {
    Super::BeginPlay();
    Layout(Radio);
    ActualizarLayout();
}

void ALayout1VRVisualization::CreateNodos() {
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
            SpawnParams.Instigator = Instigator;

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
        Colores.Add(UKismetMathLibrary::HSVToRGB(h, 1.0f, 0.1f, 1.0f));
    }
    for (int i = 0; i < Nodos.Num(); i++) {
        Nodos[i]->Parent = Nodos[Nodos[i]->ParentId];
        if (Nodos[i]->Valid) {
            Nodos[i]->Color = Colores[Nodos[i]->ColorNum];
            Nodos[i]->Actualizar();
        }
    }

}


void ALayout1VRVisualization::CreateAristas() {
    int count = 0;
    UWorld * const World = GetWorld();
    if (World) {//este if deberia estar afuera
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = Instigator;
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
                AristaInstanciado->Actualizar();
                AristaInstanciado->ActualizarCollision();
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
        AristaInstanciado->Actualizar();
        AristaInstanciado->ActualizarCollision();
        //AristaInstanciado->AttachRootComponentToActor(this);

        Aristas.Add(AristaInstanciado);
        count++;
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

void ALayout1VRVisualization::Calculos(ANodo * V) {
    //calcular hojas, altura, nivel
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

void ALayout1VRVisualization::Calculos2() {
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

void ALayout1VRVisualization::Layout(float NewRadio) {//en este algoritmo puedo asignar el nivel
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
    Radio = EncontrarRadio2(DeltaPhi * Root->Altura); 
    //
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

void ALayout1VRVisualization::Layout5(float NewRadio) {//en este algoritmo puedo asignar el nivel
    TQueue<ANodo *> Cola;
    //la raiz es el ultimo nodo
    ANodo * Root = Nodos[Nodos.Num() - 1];
    Calculos2();
    Calc();//no estaba antes
    //agregado para el nuevo radio
    //int NivelDenso, CantidadNodosNivelDenso;
    //NivelMasDenso(NivelDenso, CantidadNodosNivelDenso);
    //NewRadio = EncontrarRadio1(DeltaPhi * NivelDenso, CantidadNodosNivelDenso); 
    float DistanciaArista = 20.0f;
    //
    Root->Theta = 0;
    Root->Phi = 0;
    Root->WTam = 2*PI;
    Root->WInicio = 0;
    Root->Xcoordinate = 0;
    Root->Ycoordinate = 0;
    Root->Zcoordinate = 0;
    UE_LOG(LogClass, Log, TEXT("Root id = %d, (%f,%f,%f)"), Root->Id, Root->Xcoordinate, Root->Ycoordinate, Root->Zcoordinate);
    //float DeltaPhi = PI / Root->Altura;
    float WTemp = Root->WInicio;
    //debo tener en cuenta al padre para hacer los calculos, ya que esto esta como arbol sin raiz

    //Root->Parent->Phi = Root->Phi + DeltaPhi;//estaba dividido /2
    Root->Parent->WTam = 2*PI * (float(Root->Parent->Hojas) / Root->Hojas);
    Root->Parent->WInicio = WTemp;
    Root->Parent->Theta = Root->Parent->WInicio + Root->Parent->WTam / 2;
    Root->Parent->Xcoordinate = 0;
    Root->Parent->Ycoordinate = Root->Ycoordinate + DistanciaArista * FMath::Cos(Root->Parent->Theta);
    Root->Parent->Zcoordinate = Root->Zcoordinate + DistanciaArista * FMath::Sin(Root->Parent->Theta);
    WTemp += Root->Parent->WTam;
    Cola.Enqueue(Root->Parent);
    for (int i = 0; i < Root->Sons.Num(); i++) {
        //Root->Sons[i]->Phi = Root->Phi + DeltaPhi;//estaba dividido por 2
        Root->Sons[i]->WTam = 2*PI * (float(Root->Sons[i]->Hojas) / Root->Hojas);
        Root->Sons[i]->WInicio = WTemp;
        Root->Sons[i]->Theta =Root->Sons[i]->WInicio + Root->Sons[i]->WTam / 2;
        Root->Sons[i]->Xcoordinate = 0;
        Root->Sons[i]->Ycoordinate = Root->Ycoordinate + DistanciaArista * FMath::Cos(Root->Sons[i]->Theta);
        Root->Sons[i]->Zcoordinate = Root->Zcoordinate + DistanciaArista * FMath::Sin(Root->Sons[i]->Theta);
        WTemp += Root->Sons[i]->WTam;
        Cola.Enqueue(Root->Sons[i]);
    }

    while (!Cola.IsEmpty()) {
        ANodo * V;
        Cola.Dequeue(V);
        WTemp = V->WInicio;
        for (int i = 0; i < V->Sons.Num(); i++) {
            V->Sons[i]->WTam = 2*PI * (float(V->Sons[i]->Hojas) / Root->Hojas);
            V->Sons[i]->WInicio = WTemp;
            V->Sons[i]->Theta = V->Sons[i]->WInicio + V->Sons[i]->WTam / 2;
            V->Sons[i]->Xcoordinate = 0;
            V->Sons[i]->Ycoordinate = V->Ycoordinate + DistanciaArista * FMath::Cos(V->Sons[i]->Theta);
            V->Sons[i]->Zcoordinate = V->Zcoordinate + DistanciaArista * FMath::Sin(V->Sons[i]->Theta);
            WTemp += V->Sons[i]->WTam;
            Cola.Enqueue(V->Sons[i]);
        }
    }
}

void ALayout1VRVisualization::ActualizarLayout() {
    for (int i = 0; i < Nodos.Num(); i++) {
        FVector NuevaPosicion;
        NuevaPosicion.X = Nodos[i]->Xcoordinate;
        NuevaPosicion.Y = Nodos[i]->Ycoordinate;
        NuevaPosicion.Z = Nodos[i]->Zcoordinate;
        UE_LOG(LogClass, Log, TEXT("Nodo id = %d, (%f,%f,%f)"), Nodos[i]->Id, NuevaPosicion.X, NuevaPosicion.Y, NuevaPosicion.Z);
        //Nodos[i]->SetActorLocation(NuevaPosicion);
        Nodos[i]->SetActorRelativeLocation(NuevaPosicion);
    }
    for (int i = 0; i < Aristas.Num(); i++) {
        Aristas[i]->Actualizar();
    }

}
void ALayout1VRVisualization::Calc() {//para hallar niveles
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

int ALayout1VRVisualization::EncontrarNivel(TQueue<ANodo *> & cola, ANodo * Rama, int Nivel) {//este requiere que los nodos tenga el nivel pueso, o que este layout calcule el nivel
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

void ALayout1VRVisualization::Layout3(float NewRadio) {
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

void ALayout1VRVisualization::UbicacionesLayout4(ANodo * Rama, int NivelDenso) {// en lugar de nre radio, deberia usar la variable radio
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


void ALayout1VRVisualization::Layout4() {//se entiende que sera usado despuesde un primer caclulo del radial layout
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

void ALayout1VRVisualization::Layout2(ANodo * Node, float NewRadio, int NivelExp, float PhiNode, float WInicioInicial, float WTamInicial) {
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

void ALayout1VRVisualization::ExpandirLayout() {
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

float ALayout1VRVisualization::EncontrarRadio1(float PhiNivelDenso, int CantidadNodosNivel) {//el radio de los nodos esta en la variable de la clase VRVisualization
    int NivelDenso;
    int CantidadNodos;
    NivelMasDenso(NivelDenso, CantidadNodos);
    float RadioNivel = 8 * CantidadNodosNivel*RadioNodos/(2*PI);
    //usar la cantidad de nodos no esta bien, produce mucho errores, ya que no hay la suficiente cantidad, o no estan acomodados en el lugar correcto, el conjunto feo es pureuba de ello
    //asumire que quiero una separacion casi nula en este nivel, de ser necesario agregare un sumando mas
    float RadioArbol = RadioNivel / FMath::Sin(PhiNivelDenso);
    
    return RadioArbol;
}

float ALayout1VRVisualization::EncontrarRadio2(float PhiUltimoNivel) {
    ANodo * Root = Nodos[Nodos.Num() - 1];
    float RadioNivel = Root->Hojas*RadioNodos / (2 * PI);
    float RadioArbol = RadioNivel / FMath::Sin(PhiUltimoNivel);
    return RadioArbol;
}

void ALayout1VRVisualization::ReducionDistanciaHojas() {
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

void ALayout1VRVisualization::AplicarTraslacion(FVector Traslacion) {

}

void ALayout1VRVisualization::AplicarRotacionRelativaANodo(ANodo* NodoReferencia, FVector PuntoReferencia) {

}

//reemplazar fors por entradas directas
