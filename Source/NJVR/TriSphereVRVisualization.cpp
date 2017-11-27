// Fill out your copyright notice in the Description page of Project Settings.

#include "NJVR.h"
#include "TriSphereVRVisualization.h"
#include "Nodo.h"
#include "Arista.h"
#include "VRPawn.h"
#include "MotionControllerComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include <stack>
#include <queue>

bool MenorTriangulo(Triangulo a, Triangulo b) {
    return a.Nivel < b.Nivel || (a.Nivel == b.Nivel && a.Theta < b.Theta);
}

ATriSphereVRVisualization::ATriSphereVRVisualization() {
    Radio = 20.0f;
}
void ATriSphereVRVisualization::BeginPlay() {
    Super::BeginPlay();
    //LayoutTD();
    GenerarIcosaedro(1);
    DividirTriangulos();
    DividirTriangulos();
    //DividirTriangulos();

    OrdenarTriangulos();
    LayoutBU();
    ActualizarLayout();
}

void ATriSphereVRVisualization::CreateNodos() {
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


void ATriSphereVRVisualization::CreateAristas() {
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

void ATriSphereVRVisualization::Calculos(ANodo * V) {//lo uso dentro de claculos 2, por alguna razon
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

void ATriSphereVRVisualization::Calculos2() {//calcula hojas y altura, de otra forma
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

void ATriSphereVRVisualization::Calc() {//para hallar niveles
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

void ATriSphereVRVisualization::Calculos3(int & hojas, int & nivelMax) {//este calculos 2 es diferente al calculos2 de layout 1, por eso no puedo generalzar aun,
    std::stack<ANodo *> pila;
    //la raiz es el ultimo nodo
    ANodo * Root = Nodos[Nodos.Num() - 1];
    Root->Nivel = 0;
    //pila.push(Root);//no deberia dsencolarlo
    hojas = 0;
    nivelMax = 0;
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
        else {
            V->Casilla = hojas;
            hojas++;
            nivelMax = FMath::Max<float>(nivelMax, V->Nivel);
        }
    }
}

void ATriSphereVRVisualization::Calculos4(int & hojas, int & nivelMax) {//este calculos 2 es diferente al calculos2 de layout 1, por eso no puedo generalzar aun,
    std::stack<ANodo *> pila;
    //la raiz es el ultimo nodo
    ANodo * Root = Nodos[Nodos.Num() - 1];
    Root->Nivel = 0;
    //pila.push(Root);//no deberia dsencolarlo
    hojas = 0;
    nivelMax = 0;
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
        else {
            V->Casilla = hojas;
            hojas++;
            nivelMax = FMath::Max<float>(nivelMax, V->Nivel);
        }
    }
}

void ATriSphereVRVisualization::LayoutCasillas(float NewRadio) {//en este algoritmo puedo asignar el nivel
    //Ladrillos
    TQueue<ANodo *> Cola;
    std::stack<ANodo *> pila;
    //la raiz es el ultimo nodo
    ANodo * Root = Nodos[Nodos.Num() - 1];
    int hojas;
    int nivelMax;
    Calculos3(hojas, nivelMax);
    //esos datos creo que se pueden sacar de la raiz del arbol
    Root->Theta = 0;
    Root->Phi = 0;
    Root->Xcoordinate = NewRadio * FMath::Sin(Root->Phi) * FMath::Cos(Root->Theta);
    Root->Ycoordinate = NewRadio * FMath::Sin(Root->Phi) * FMath::Sin(Root->Theta);
    Root->Zcoordinate = NewRadio * FMath::Cos(Root->Phi);
    //UE_LOG(LogClass, Log, TEXT("Root id = %d, (%f,%f,%f)"), Root->Id, Root->Xcoordinate, Root->Ycoordinate, Root->Zcoordinate);
    //float DeltaPhi = PI / nivelMax;
    float DeltaPhi = (PhiMax - PhiMin) / nivelMax;
    //float DeltaPhi = PI / nivelMax;
    float DeltaTheta = 2 * PI / hojas;
    UE_LOG(LogClass, Log, TEXT("DeltaPhi = %f"), DeltaPhi);
    Cola.Enqueue(Root->Sons[0]);
    pila.push(Root->Sons[0]);
    Cola.Enqueue(Root->Sons[1]);
    pila.push(Root->Sons[1]);
    Cola.Enqueue(Root->Parent);
    pila.push(Root->Parent);

    while (!Cola.IsEmpty()) {
        ANodo * V;
        Cola.Dequeue(V);
        for (int i = 0; i < V->Sons.Num(); i++) {
            Cola.Enqueue(V->Sons[i]);
            pila.push(V->Sons[i]);
        }
    }

    while (!pila.empty()) {
        ANodo * V = pila.top();
        pila.pop();
        if (V->Sons.Num()) {
            V->Casilla = (V->Sons[0]->Casilla + V->Sons[1]->Casilla) / 2;
            //V->Casilla += V->Nivel & 1;
            //V->Phi = V->Nivel * DeltaPhi;
            //V->Phi = (V->Nivel-1) * DeltaPhi + PhiMin;
            //V->Theta = V->Casilla * DeltaTheta + DeltaTheta / 2 * !(V->Nivel & 1);
        }
        //else{
            //V->Phi = V->Nivel * DeltaPhi;
            //V->Phi = (V->Nivel-1) * DeltaPhi + PhiMin;
            //V->Phi = nivelMax * DeltaPhi;//para alinear las hojas al ultimo nivel
            //V->Theta = V->Casilla * DeltaTheta + DeltaTheta / 2 * !(V->Nivel & 1);
            //V->Theta = V->Casilla * DeltaTheta + DeltaTheta / 2 * !(nivelMax & 1);
        //}
        FVector Posicion = Triangulos[V->Casilla].Centro * Radio * V->Nivel;
        V->Xcoordinate = Posicion.X;
        V->Ycoordinate = Posicion.Y;
        V->Zcoordinate = Posicion.Z;
    }
}

void ATriSphereVRVisualization::LayoutTD() {
}

void ATriSphereVRVisualization::LayoutBU() {
    //Ladrillos
    TQueue<ANodo *> Cola;
    std::stack<ANodo *> pila;
    //la raiz es el ultimo nodo
    ANodo * Root = Nodos[Nodos.Num() - 1];
    int hojas;
    int nivelMax;
    Calculos3(hojas, nivelMax);
    //esos datos creo que se pueden sacar de la raiz del arbol
    Root->Theta = 0;
    Root->Phi = 0;
    Root->Xcoordinate = 0 * FMath::Sin(Root->Phi) * FMath::Cos(Root->Theta);
    Root->Ycoordinate = 0 * FMath::Sin(Root->Phi) * FMath::Sin(Root->Theta);
    Root->Zcoordinate = 0 * FMath::Cos(Root->Phi);
    Cola.Enqueue(Root->Sons[0]);
    pila.push(Root->Sons[0]);
    Cola.Enqueue(Root->Sons[1]);
    pila.push(Root->Sons[1]);
    Cola.Enqueue(Root->Parent);
    pila.push(Root->Parent);

    while (!Cola.IsEmpty()) {
        ANodo * V;
        Cola.Dequeue(V);
        for (int i = 0; i < V->Sons.Num(); i++) {
            Cola.Enqueue(V->Sons[i]);
            pila.push(V->Sons[i]);
        }
    }

    while (!pila.empty()) {
        ANodo * V = pila.top();
        pila.pop();
        if (!V->Valid) {
            //V->Casilla = (V->Sons[0]->Casilla + V->Sons[1]->Casilla) / 2;
            V->Casilla = V->Sons[0]->Casilla;
        }
        //FVector Posicion = Triangulos[V->Casilla].Centro * Radio * 5;
        FVector Posicion = Triangulos[V->Casilla].Centro * Radio * V->Nivel;
        V->Xcoordinate = Posicion.X;
        V->Ycoordinate = Posicion.Y;
        V->Zcoordinate = Posicion.Z;
    }
}

void ATriSphereVRVisualization::ActualizarLayout() {
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

void ATriSphereVRVisualization::GenerarIcosaedro(int Precision) {
    //float DeltaPhi = PI * 26.56505 / 180;
    float DeltaPhi = 1.10714872;
    float DeltaTheta = PI * 72 / 180;
    float DiffTheta = PI * 36 / 180;
    Vertices.push_back(FVector(0.0f, 0.0f, 1.0f));//puntos para radio uno
    VerticesP.push_back(FVector(0.0f, 0.0f, 1.0f));
    for (int i = 0; i < 5; i++) {
        VerticesP.push_back(FVector(DeltaPhi, i*DeltaTheta, 1.0f));
        FVector posicion;
        posicion.X = 1.0f * FMath::Sin(DeltaPhi) * FMath::Cos(i*DeltaTheta);
        posicion.Y = 1.0f * FMath::Sin(DeltaPhi) * FMath::Sin(i*DeltaTheta);
        posicion.Z = 1.0f * FMath::Cos(DeltaPhi);
        Vertices.push_back(posicion);
    }
    for (int i = 0; i < 5; i++) {
        VerticesP.push_back(FVector(PI - DeltaPhi, i*DeltaTheta + DiffTheta, 1.0f));
        FVector posicion;
        posicion.X = 1.0f * FMath::Sin(PI - DeltaPhi) * FMath::Cos(i*DeltaTheta + DiffTheta);
        posicion.Y = 1.0f * FMath::Sin(PI - DeltaPhi) * FMath::Sin(i*DeltaTheta + DiffTheta);
        posicion.Z = 1.0f * FMath::Cos(PI - DeltaPhi);
        Vertices.push_back(posicion);
    }
    Vertices.push_back(FVector(0.0, 0.0f, -1.0f));//puntos para radio uno
    VerticesP.push_back(FVector(PI, 0.0f, 1.0f));

    //Tirangulos superiores
    Triangulos.push_back(Triangulo(Vertices[0], Vertices[1], Vertices[2], 0, 0));
    Triangulos.push_back(Triangulo(Vertices[0], Vertices[2], Vertices[3], 0, 0));
    Triangulos.push_back(Triangulo(Vertices[0], Vertices[3], Vertices[4], 0, 0));
    Triangulos.push_back(Triangulo(Vertices[0], Vertices[4], Vertices[5], 0, 0));
    Triangulos.push_back(Triangulo(Vertices[0], Vertices[5], Vertices[1], 0, 0));

    //triangulos medios
    Triangulos.push_back(Triangulo(Vertices[1], Vertices[6], Vertices[2], 1, 1));
    Triangulos.push_back(Triangulo(Vertices[2], Vertices[6], Vertices[7], 1, 0));
    Triangulos.push_back(Triangulo(Vertices[2], Vertices[7], Vertices[3], 1, 1));
    Triangulos.push_back(Triangulo(Vertices[3], Vertices[7], Vertices[8], 1, 0));
    Triangulos.push_back(Triangulo(Vertices[3], Vertices[8], Vertices[4], 1, 1));
    Triangulos.push_back(Triangulo(Vertices[4], Vertices[8], Vertices[9], 1, 0));
    Triangulos.push_back(Triangulo(Vertices[4], Vertices[9], Vertices[5], 1, 1));
    Triangulos.push_back(Triangulo(Vertices[5], Vertices[9], Vertices[10], 1, 0));
    Triangulos.push_back(Triangulo(Vertices[5], Vertices[10], Vertices[1], 1, 1));
    Triangulos.push_back(Triangulo(Vertices[1], Vertices[10], Vertices[6], 1, 0));

    //triangulos inferiores
    Triangulos.push_back(Triangulo(Vertices[6], Vertices[11], Vertices[7], 2, 1));
    Triangulos.push_back(Triangulo(Vertices[7], Vertices[11], Vertices[8], 2, 1));
    Triangulos.push_back(Triangulo(Vertices[8], Vertices[11], Vertices[9], 2, 1));
    Triangulos.push_back(Triangulo(Vertices[9], Vertices[11], Vertices[10], 2, 1));
    Triangulos.push_back(Triangulo(Vertices[10], Vertices[11], Vertices[6], 2, 1));
    //LlenarVectorsConIdTriangulos();
    for (int i = 0; i < Triangulos.size(); i++) {
        CalcularCentroTriangulo(Triangulos[i]);
    }
}

void ATriSphereVRVisualization::LlenarVectorsConIdTriangulos() {
    for (int i = 0; i < Triangulos.size(); i++) {
        Triangulos[i].A = Vertices[Triangulos[i].IdA];
        Triangulos[i].B = Vertices[Triangulos[i].IdB];
        Triangulos[i].C = Vertices[Triangulos[i].IdC];
    }
}

void ATriSphereVRVisualization::CalcularCentroTriangulo(Triangulo & t) {
    FVector p = PuntoTFromAToB(t.B, t.C, 0.5);
    t.Centro = PuntoTFromAToB(t.A, p, 2.0/3.0);
    //t.Phi = FMath::Acos(t.Centro.Z);
    //t.Theta = FMath::Asin(t.Centro.Y / FMath::Sin(t.Phi));
    //t.Theta = FMath::Acos(t.Centro.X / FMath::Sin(t.Phi));
    //t.Phi = FMath::Asin(t.Centro.Z);
    //t.Theta = FMath::Asin(t.Centro.X / FMath::Sin(t.Phi));
    //t.Theta = FMath::Acos(t.Centro.Y / FMath::Sin(t.Phi));
    t.Theta = FMath::Atan(t.Centro.Y/t.Centro.X);
    if (t.Theta < 0) {
        t.Theta += PI;
    }
    if (t.Centro.Y < 0) {
        t.Theta += PI;
    }
    t.Phi = FMath::Atan(FMath::Sqrt(t.Centro.X*t.Centro.X + t.Centro.Y*t.Centro.Y)/t.Centro.Z);
}


void ATriSphereVRVisualization::CalcularCentro(int IdTriangulo) {
    //en el arrgro de vertices los vectores representan phi theta y radio
    if (VerticesP[Triangulos[IdTriangulo].IdA].X == VerticesP[Triangulos[IdTriangulo].IdC].X) {//tringulo hacia abajo
        Triangulos[IdTriangulo].Theta = VerticesP[Triangulos[IdTriangulo].IdB].Y;
        //el centro del triangulo esta a 2 tercios de la algura deste uno de los vertices
        Triangulos[IdTriangulo].Phi = (VerticesP[Triangulos[IdTriangulo].IdB].X - VerticesP[Triangulos[IdTriangulo].IdA].X)/3 + VerticesP[Triangulos[IdTriangulo].IdA].X;
        //fata calcular el fvector
    }
    else {//triangulo hacia arriba, por lo tanto c y b esta a la misma algutar, a arriba
        Triangulos[IdTriangulo].Theta = VerticesP[Triangulos[IdTriangulo].IdA].Y;
        Triangulos[IdTriangulo].Phi = (VerticesP[Triangulos[IdTriangulo].IdB].X - VerticesP[Triangulos[IdTriangulo].IdA].X)*2/3 + VerticesP[Triangulos[IdTriangulo].IdA].X;
        //fata calcular el fvector
    }
    Triangulos[IdTriangulo].Centro.X = 1.0f * FMath::Sin(Triangulos[IdTriangulo].Phi) * FMath::Cos(Triangulos[IdTriangulo].Theta);
    Triangulos[IdTriangulo].Centro.Y = 1.0f * FMath::Sin(Triangulos[IdTriangulo].Phi) * FMath::Sin(Triangulos[IdTriangulo].Theta);
    Triangulos[IdTriangulo].Centro.Z = 1.0f * FMath::Cos(Triangulos[IdTriangulo].Phi);
}

FVector ATriSphereVRVisualization::PuntoTFromAToB(FVector a, FVector b, float t) {
    FVector direccion = b - a;
    FVector punto = a + direccion*t;
    //normalizar a esfera de radio 1
    punto = punto.GetSafeNormal();
    return punto;
}

FVector ATriSphereVRVisualization::PuntoTFromAToBEsferico(FVector a, FVector b, float t) {
    FVector direccion = b - a;
    FVector punto = a + direccion*t;
    //normalizar a esfera de radio 1
    punto = punto.GetSafeNormal();
    float phi = FMath::Acos(punto.Z);
    float theta = FMath::Asin(punto.Y / FMath::Sin(phi));
    FVector puntoesferico (phi, theta, 1.0f);
    return puntoesferico;
}

void ATriSphereVRVisualization::DividirTriangulo(Triangulo t) {
    FVector D = PuntoTFromAToB(t.C, t.A, 0.5);
    Vertices.push_back(D);//0.5 es la mitad
    VerticesP.push_back(PuntoTFromAToBEsferico(t.C, t.A, 0.5));//0.5 es la mitad

    FVector E = PuntoTFromAToB(t.B, t.A, 0.5);
    Vertices.push_back(E);//0.5 es la mitad
    VerticesP.push_back(PuntoTFromAToBEsferico(t.B, t.A, 0.5));//0.5 es la mitad

    FVector F = PuntoTFromAToB(t.C, t.B, 0.5);
    Vertices.push_back(F);//0.5 es la mitad
    VerticesP.push_back(PuntoTFromAToBEsferico(t.C, t.B, 0.5));//0.5 es la mitad
    //if (VerticesP[t.IdA].X == VerticesP[t.IdC].X) {//tringulo hacia abajo
    if (t.Orientacion) {//tringulo hacia abajo
        Triangulos.push_back(Triangulo(t.A, E, D, t.Nivel*2, 0));
        Triangulos.push_back(Triangulo(D, E, F, t.Nivel*2, 0));
        Triangulos.push_back(Triangulo(D, F, t.C, t.Nivel*2, 0));
        Triangulos.push_back(Triangulo(E, t.B, F, t.Nivel*2 + 1, 1));
    }
    else {//triangulo hacia arriba, por lo tanto c y b esta a la misma algutar, a arriba
        Triangulos.push_back(Triangulo(t.A, E, D, t.Nivel*2, 0));
        Triangulos.push_back(Triangulo(E, t.B, F, t.Nivel*2 + 1, 1));
        Triangulos.push_back(Triangulo(E, F, D, t.Nivel*2+1, 1));
        Triangulos.push_back(Triangulo(D, F, t.C, t.Nivel*2 + 1, 1));
    }
}

void ATriSphereVRVisualization::DividirTriangulos() {
    int n = Triangulos.size();
    for (int i = 0; i < n; i++) {
        DividirTriangulo(Triangulos[0]);
        Triangulos.erase(Triangulos.begin());
    }
    for (int i = 0; i < Triangulos.size(); i++) {
        CalcularCentroTriangulo(Triangulos[i]);
    }
}
void ATriSphereVRVisualization::DibujarVertices() {
    for (int i = 0; i < Vertices.size(); i++) {
        //DrawDebugLine(GetWorld(), FVector::ZeroVector, FVector::ZeroVector, FColor::Black, false, -1.0f, 0, Radio*Escala);
        if(i < 12)
            DrawDebugSphere(GetWorld(), Vertices[i]*40, 2.5f, 8, FColor::Black, false, -1.0f, 0, 0.5f);
        else
            DrawDebugSphere(GetWorld(), Vertices[i]*40, 2.5f, 8, FColor::Blue, false, -1.0f, 0, 0.5f);
    }
}

void ATriSphereVRVisualization::DibujarTriangulos() {
    for (int i = 0; i < Triangulos.size(); i++) {
        //DrawDebugLine(GetWorld(), FVector::ZeroVector, FVector::ZeroVector, FColor::Black, false, -1.0f, 0, Radio*Escala);
        DrawDebugSphere(GetWorld(), Triangulos[i].Centro*100, 2.5f, 4, FColor::Black, false, -1.0f, 0, 0.5f);
        //UE_LOG(LogClass, Log, TEXT("Triangulo = %d, (%f,%f,%f)"), i, Triangulos[i].Centro.X, Triangulos[i].Centro.Y, Triangulos[i].Centro.Z);
    }

}

void ATriSphereVRVisualization::OrdenarTriangulos() {
    std::sort(Triangulos.begin(), Triangulos.end(), MenorTriangulo);
    for (int i = 0; i < Triangulos.size(); i++) {
        UE_LOG(LogClass, Log, TEXT("Triangulo = %d, C(%f,%f,%f), Phi = %f, Theta = %f, Nivel = %d"), i, Triangulos[i].Centro.X, Triangulos[i].Centro.Y, Triangulos[i].Centro.Z, Triangulos[i].Phi, Triangulos[i].Theta, Triangulos[i].Nivel);
    }
}