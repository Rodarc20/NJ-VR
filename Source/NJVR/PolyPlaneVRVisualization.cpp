// Fill out your copyright notice in the Description page of Project Settings.

#include "NJVR.h"
#include "PolyPlaneVRVisualization.h"
#include "Nodo.h"
#include "Arista.h"
#include "VRPawn.h"
#include "MotionControllerComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include <stack>
#include <queue>

APolyPlaneVRVisualization::APolyPlaneVRVisualization() {
    Radio = 20.0f;
}
void APolyPlaneVRVisualization::BeginPlay() {
    Super::BeginPlay();
    Layout();
    ActualizarLayout();
}

void APolyPlaneVRVisualization::CreateNodos() {
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
        Colores.Add(UKismetMathLibrary::HSVToRGB(h, 1.0f, IntensidadColor, 1.0f));
    }
    for (int i = 0; i < Nodos.Num(); i++) {
        Nodos[i]->Parent = Nodos[Nodos[i]->ParentId];
        Nodos[i]->Usuario = Usuario;
        if (Nodos[i]->Valid) {
            Nodos[i]->Color = Colores[Nodos[i]->ColorNum];
            Nodos[i]->Actualizar();
        }
    }

}


void APolyPlaneVRVisualization::CreateAristas() {
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

void APolyPlaneVRVisualization::Calculos(ANodo * V) {//lo uso dentro de claculos 2, por alguna razon
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

void APolyPlaneVRVisualization::Calculos2() {//calcula hojas y altura, de otra forma
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

void APolyPlaneVRVisualization::Calc() {//para hallar niveles
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

void APolyPlaneVRVisualization::Layout() {//en este algoritmo puedo asignar el nivel
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
    Cola.Enqueue(Root->Parent);
    Cola.Enqueue(Root->Sons[0]);
    Cola.Enqueue(Root->Sons[1]);
    
    ANodo * V;
    while (!Cola.IsEmpty() && Cola.Peek(V) && V->Nivel != 3) {
        Cola.Dequeue(V);
        for (int i = 0; i < V->Sons.Num(); i++) {
            Cola.Enqueue(V->Sons[i]);
        }
    }

    for (int i = 0; i < 4 && !Cola.IsEmpty(); i++) {
        Cola.Dequeue(V);
        LayoutSubPlanoVertical(V, 0, PI/2, PI/2*i);
        Cola.Dequeue(V);
        LayoutSubPlanoVertical(V, PI/2, PI/2, PI/2*i);
    }
    for(int i = 0; i < 4 && !Cola.IsEmpty(); i++){
        Cola.Dequeue(V);
        LayoutSubPlanoHorizontal(V, PI / 2 * i, PI / 2, PI/2);
    }
    
    Root->Theta = 0;
    Root->Phi = 0;
    Root->Xcoordinate = 0;
    Root->Ycoordinate = 0;
    Root->Zcoordinate = 0;

    Root->Parent->Phi = 0;
    Root->Parent->Theta = 0;
    Root->Parent->Xcoordinate = Radio * FMath::Sin(Root->Parent->Phi) * FMath::Cos(Root->Parent->Theta);
    Root->Parent->Ycoordinate = Radio * FMath::Sin(Root->Parent->Phi) * FMath::Sin(Root->Parent->Theta);
    Root->Parent->Zcoordinate = Radio * FMath::Cos(Root->Parent->Phi);
    for (int i = 0; i < Root->Sons.Num(); i++) {
        Root->Sons[i]->Phi = 2*PI/3;
        Root->Sons[i]->Theta = 7*PI/8 + PI*(i & 1);
        Root->Sons[i]->Xcoordinate = Radio * FMath::Sin(Root->Sons[i]->Phi) * FMath::Cos(Root->Sons[i]->Theta);
        Root->Sons[i]->Ycoordinate = Radio * FMath::Sin(Root->Sons[i]->Phi) * FMath::Sin(Root->Sons[i]->Theta);
        Root->Sons[i]->Zcoordinate = Radio * FMath::Cos(Root->Sons[i]->Phi);
    }
    //deberia hacer esto primero para que lo que quede en la cola, sea lo que pase por los subplanos
    Root->Parent->Sons[0]->Phi = PI/4;
    Root->Parent->Sons[0]->Theta = 3*PI/4;
    Root->Parent->Sons[0]->Xcoordinate = Radio * 2 * FMath::Sin(Root->Parent->Sons[0]->Phi) * FMath::Cos(Root->Parent->Sons[0]->Theta);
    Root->Parent->Sons[0]->Ycoordinate = Radio * 2  * FMath::Sin(Root->Parent->Sons[0]->Phi) * FMath::Sin(Root->Parent->Sons[0]->Theta);
    Root->Parent->Sons[0]->Zcoordinate = Radio * 2  * FMath::Cos(Root->Parent->Sons[0]->Phi);

    Root->Parent->Sons[1]->Phi = PI/4;
    Root->Parent->Sons[1]->Theta = 7*PI/4;
    Root->Parent->Sons[1]->Xcoordinate = Radio * 2  * FMath::Sin(Root->Parent->Sons[1]->Phi) * FMath::Cos(Root->Parent->Sons[1]->Theta);
    Root->Parent->Sons[1]->Ycoordinate = Radio * 2  * FMath::Sin(Root->Parent->Sons[1]->Phi) * FMath::Sin(Root->Parent->Sons[1]->Theta);
    Root->Parent->Sons[1]->Zcoordinate = Radio * 2  * FMath::Cos(Root->Parent->Sons[1]->Phi);

    Root->Sons[0]->Sons[0]->Phi = PI/2;
    Root->Sons[0]->Sons[0]->Theta = PI;
    Root->Sons[0]->Sons[0]->Xcoordinate = Radio * 2  * FMath::Sin(Root->Sons[0]->Sons[0]->Phi) * FMath::Cos(Root->Sons[0]->Sons[0]->Theta);
    Root->Sons[0]->Sons[0]->Ycoordinate = Radio * 2  * FMath::Sin(Root->Sons[0]->Sons[0]->Phi) * FMath::Sin(Root->Sons[0]->Sons[0]->Theta);
    Root->Sons[0]->Sons[0]->Zcoordinate = Radio * 2  * FMath::Cos(Root->Sons[0]->Sons[0]->Phi);

    Root->Sons[0]->Sons[1]->Phi = 3*PI/4;
    Root->Sons[0]->Sons[1]->Theta = 3*PI/4;
    Root->Sons[0]->Sons[1]->Xcoordinate = Radio * 2  * FMath::Sin(Root->Sons[0]->Sons[1]->Phi) * FMath::Cos(Root->Sons[0]->Sons[1]->Theta);
    Root->Sons[0]->Sons[1]->Ycoordinate = Radio * 2  * FMath::Sin(Root->Sons[0]->Sons[1]->Phi) * FMath::Sin(Root->Sons[0]->Sons[1]->Theta);
    Root->Sons[0]->Sons[1]->Zcoordinate = Radio * 2  * FMath::Cos(Root->Sons[0]->Sons[1]->Phi);

    Root->Sons[1]->Sons[0]->Phi = PI/2;
    Root->Sons[1]->Sons[0]->Theta = 0;
    Root->Sons[1]->Sons[0]->Xcoordinate = Radio * 2  * FMath::Sin(Root->Sons[1]->Sons[0]->Phi) * FMath::Cos(Root->Sons[1]->Sons[0]->Theta);
    Root->Sons[1]->Sons[0]->Ycoordinate = Radio * 2  * FMath::Sin(Root->Sons[1]->Sons[0]->Phi) * FMath::Sin(Root->Sons[1]->Sons[0]->Theta);
    Root->Sons[1]->Sons[0]->Zcoordinate = Radio * 2  * FMath::Cos(Root->Sons[1]->Sons[0]->Phi);

    Root->Sons[1]->Sons[1]->Phi = 3*PI/4;
    Root->Sons[1]->Sons[1]->Theta = 3*PI/4;
    Root->Sons[1]->Sons[1]->Xcoordinate = Radio * 2  * FMath::Sin(Root->Sons[1]->Sons[1]->Phi) * FMath::Cos(Root->Sons[1]->Sons[1]->Theta);
    Root->Sons[1]->Sons[1]->Ycoordinate = Radio * 2  * FMath::Sin(Root->Sons[1]->Sons[1]->Phi) * FMath::Sin(Root->Sons[1]->Sons[1]->Theta);
    Root->Sons[1]->Sons[1]->Zcoordinate = Radio * 2  * FMath::Cos(Root->Sons[1]->Sons[1]->Phi);
}


void APolyPlaneVRVisualization::LayoutSubPlanoVertical(ANodo * Node, float PhiInicioInicial, float PhiTamInicial, float Theta) {
    //El phi vien a ser el wedge
    TQueue<ANodo *> Cola;
    //la raiz es el ultimo nodo
    Calculos2();
    Calc();//no estaba antes

    Node->Theta = Theta;
    Node->Phi = PhiInicioInicial + PhiTamInicial/2;
    Node->WTam = PhiTamInicial;
    Node->WInicio = PhiInicioInicial;
    Node->Xcoordinate = Radio * Node->Nivel * FMath::Sin(Node->Phi) * FMath::Cos(Node->Theta);
    Node->Ycoordinate = Radio * Node->Nivel * FMath::Sin(Node->Phi) * FMath::Sin(Node->Theta);
    Node->Zcoordinate = Radio * Node->Nivel * FMath::Cos(Node->Phi);
    UE_LOG(LogClass, Log, TEXT("Root id = %d, (%f,%f,%f)"), Node->Id, Node->Xcoordinate, Node->Ycoordinate, Node->Zcoordinate);
    //float DeltaPhi = PI / Root->Altura;
    Cola.Enqueue(Node);
    while (!Cola.IsEmpty()) {
        ANodo * V;
        Cola.Dequeue(V);
        float WTemp = V->WInicio;
        for (int i = 0; i < V->Sons.Num(); i++) {
            V->Sons[i]->Theta = V->Theta;
            V->Sons[i]->WTam = V->WTam * (float(V->Sons[i]->Hojas) / V->Hojas);
            V->Sons[i]->WInicio = WTemp;
            V->Sons[i]->Phi = WTemp + V->Sons[i]->WTam / 2;
            V->Sons[i]->Xcoordinate = Radio * (V->Sons[i]->Nivel /*- Node->Nivel + 1*/) * FMath::Sin(V->Sons[i]->Phi) * FMath::Cos(V->Sons[i]->Theta);
            V->Sons[i]->Ycoordinate = Radio * (V->Sons[i]->Nivel /*- Node->Nivel + 1*/) * FMath::Sin(V->Sons[i]->Phi) * FMath::Sin(V->Sons[i]->Theta);
            V->Sons[i]->Zcoordinate = Radio * (V->Sons[i]->Nivel /*- Node->Nivel + 1*/) * FMath::Cos(V->Sons[i]->Phi);
            WTemp += V->Sons[i]->WTam;
            Cola.Enqueue(V->Sons[i]);
        }
    }
}

void APolyPlaneVRVisualization::LayoutSubPlanoHorizontal(ANodo * Node, float ThetaInicioInicial, float ThetaTamInicial, float Phi) {
    TQueue<ANodo *> Cola;
    //la raiz es el ultimo nodo
    ANodo * Root = Nodos[Nodos.Num() - 1];
    Calculos2();
    Calc();//no estaba antes

    Node->Theta = ThetaInicioInicial + ThetaTamInicial/2;
    Node->Phi = Phi;
    Node->WTam = ThetaTamInicial;
    Node->WInicio = ThetaInicioInicial;
    Node->Xcoordinate = Radio * Node->Nivel * FMath::Sin(Node->Phi) * FMath::Cos(Node->Theta);
    Node->Ycoordinate = Radio * Node->Nivel * FMath::Sin(Node->Phi) * FMath::Sin(Node->Theta);
    Node->Zcoordinate = Radio * Node->Nivel * FMath::Cos(Node->Phi);
    UE_LOG(LogClass, Log, TEXT("Root id = %d, (%f,%f,%f)"), Root->Id, Root->Xcoordinate, Root->Ycoordinate, Root->Zcoordinate);
    //float DeltaPhi = PI / Root->Altura;
    Cola.Enqueue(Node);
    while (!Cola.IsEmpty()) {
        ANodo * V;
        Cola.Dequeue(V);
        float WTemp = V->WInicio;
        for (int i = 0; i < V->Sons.Num(); i++) {
            V->Sons[i]->Phi = V->Phi;
            V->Sons[i]->WTam = V->WTam * (float(V->Sons[i]->Hojas) / V->Hojas);
            V->Sons[i]->WInicio = WTemp;
            V->Sons[i]->Theta = WTemp + V->Sons[i]->WTam / 2;
            V->Sons[i]->Xcoordinate = Radio * (V->Sons[i]->Nivel /*- Node->Nivel + 1*/) * FMath::Sin(V->Sons[i]->Phi) * FMath::Cos(V->Sons[i]->Theta);
            V->Sons[i]->Ycoordinate = Radio * (V->Sons[i]->Nivel /*- Node->Nivel + 1*/) * FMath::Sin(V->Sons[i]->Phi) * FMath::Sin(V->Sons[i]->Theta);
            V->Sons[i]->Zcoordinate = Radio * (V->Sons[i]->Nivel /*- Node->Nivel + 1*/) * FMath::Cos(V->Sons[i]->Phi);
            WTemp += V->Sons[i]->WTam;
            Cola.Enqueue(V->Sons[i]);
        }
    }
}

void APolyPlaneVRVisualization::ActualizarLayout() {//este actulizar deberia ser general
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
