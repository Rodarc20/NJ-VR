// Fill out your copyright notice in the Description page of Project Settings.

#include "NJVR.h"
#include "Layout2VRVisualization.h"
#include "Nodo.h"
#include "Arista.h"
#include "VRPawn.h"
#include "MotionControllerComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include <stack>
#include <queue>

ALayout2VRVisualization::ALayout2VRVisualization(){

    Radio = 300.0f;
    PhiMax = PI;
    PhiMin = 0;
    PhiMax = FMath::DegreesToRadians(150);
    PhiMin = FMath::DegreesToRadians(40);
}

void ALayout2VRVisualization::BeginPlay() {
    Super::BeginPlay();
    Layout(Radio);
    ActualizarLayout();
}


void ALayout2VRVisualization::CreateNodos() {
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
            if (bColorear)
                Nodos[i]->Color = Colores[Nodos[i]->ColorNum];
            else
                Nodos[i]->Color = ColorReal;
            Nodos[i]->Actualizar();
        }
    }

}


void ALayout2VRVisualization::CreateAristas() {
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

void ALayout2VRVisualization::Calculos(ANodo * V) {//quien sera la raiz. tener cuidado con esto para layouts pequeños o esferitas
}

void ALayout2VRVisualization::Calculos2(int & hojas, int & nivelMax) {//este calculos 2 es diferente al calculos2 de layout 1, por eso no puedo generalzar aun,
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

void ALayout2VRVisualization::Layout(float NewRadio) {//en este algoritmo puedo asignar el nivel
    //Ladrillos
    TQueue<ANodo *> Cola;
    std::stack<ANodo *> pila;
    //la raiz es el ultimo nodo
    ANodo * Root = Nodos[Nodos.Num() - 1];
    int hojas;
    int nivelMax;
    Calculos2(hojas, nivelMax);
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
            V->Casilla += V->Nivel & 1;
            //V->Phi = V->Nivel * DeltaPhi;
            V->Phi = (V->Nivel-1) * DeltaPhi + PhiMin;
            V->Theta = V->Casilla * DeltaTheta + DeltaTheta / 2 * !(V->Nivel & 1);
        }
        else{
            //V->Phi = V->Nivel * DeltaPhi;
            V->Phi = (V->Nivel-1) * DeltaPhi + PhiMin;
            //V->Phi = nivelMax * DeltaPhi;//para alinear las hojas al ultimo nivel
            V->Theta = V->Casilla * DeltaTheta + DeltaTheta / 2 * !(V->Nivel & 1);
            //V->Theta = V->Casilla * DeltaTheta + DeltaTheta / 2 * !(nivelMax & 1);
        }
        V->Xcoordinate = NewRadio * FMath::Sin(V->Phi) * FMath::Cos(V->Theta);
        V->Ycoordinate = NewRadio * FMath::Sin(V->Phi) * FMath::Sin(V->Theta);
        V->Zcoordinate = NewRadio * FMath::Cos(V->Phi);
    }
}

void ALayout2VRVisualization::ActualizarLayout() {
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

//convertir de esfericas a cartesianas deberia estar en una funcion en la que recibes solamente el nodo
void ALayout2VRVisualization::AplicarTraslacion(FVector Traslacion) {
    for (int i = 0; i < NodosSeleccionados.Num(); i++) {
        //NodosSeleccionados[i]->AddActorLocalOffset(Traslacion);//no esoty seguro si esto funcone
        NodosSeleccionados[i]->SetActorLocation(NodosSeleccionados[i]->GetActorLocation() + Traslacion);
    }
    //deberia solo actulizzar los seleccioados para que sea mas eficiente

}

void ALayout2VRVisualization::AplicarRotacionRelativaANodo(ANodo* NodoReferencia, FVector PuntoReferencia) {

}

void ALayout2VRVisualization::AplicarTraslacionEsferica(float TraslacionPhi, float TraslacionTheta) {
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

FVector ALayout2VRVisualization::InterseccionLineaSuperficie() {//retorna en espacio local
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

void ALayout2VRVisualization::TraslacionConNodoGuia() {//retorna en espacio local
    ImpactPoint = InterseccionLineaSuperficie();
    //UE_LOG(LogClass, Log, TEXT("Impact = %f, %f, %f"), ImpactPoint.X, ImpactPoint.Y, ImpactPoint.Z);
    if (ImpactPoint != FVector::ZeroVector) {//un punto imposible para el layout, que seria cualquier que no cumpa la ecuacion podria por eso ser el vector 0
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

void ALayout2VRVisualization::InvertirRama(ANodo * NodoReferencia) {
    std::queue<ANodo *> Cola;
    float ThetaRelativo = NodoReferencia->Theta;
    Cola.push(NodoReferencia);
    while (!Cola.empty()) {
        ANodo * V = Cola.front();
        Cola.pop();
        for (int i = 0; i < V->Sons.Num(); i++) {
            V->Sons[i]->Theta = modFloat(ThetaRelativo + (ThetaRelativo - V->Sons[i]->Theta), 2 * PI);
            V->Sons[i]->Xcoordinate = Radio * FMath::Sin(V->Sons[i]->Phi) * FMath::Cos(V->Sons[i]->Theta);
            V->Sons[i]->Ycoordinate = Radio * FMath::Sin(V->Sons[i]->Phi) * FMath::Sin(V->Sons[i]->Theta);
            V->Sons[i]->Zcoordinate = Radio * FMath::Cos(V->Sons[i]->Phi);
            Cola.push(V->Sons[i]);
        }
    }
    ActualizarLayout();
}

/*void ALayout2VRVisualization::BuscandoNodoConLaser() {//retorna en espacio local
    ANodo * NodoEncontrado;
    FVector PuntoImpacto = BuscarNodo(NodoEncontrado);
    if (NodoEncontrado && !Interaction->IsOverHitTestVisibleWidget()) {//comprobamos la interaccion para que no se detecte lo que este detras del menu
        if (HitNodo && HitNodo != NodoEncontrado) {//quiza se pueda hacer con el boleano, debo ocultar si es que es diferente al de ahora,
            if (MostrarLabel) {
                HitNodo->OcultarNombre();
                NodoEncontrado->MostrarNombre();
            }
            //HitNodo = NodoEncontrado; bHitNodo = true; ImpactPoint = PuntoImpacto;
        }
        else {
            if (MostrarLabel) {
                NodoEncontrado->MostrarNombre();
            }
            //HitNodo = NodoEncontrado; bHitNodo = true; ImpactPoint = PuntoImpacto;
        }
        HitNodo = NodoEncontrado;//podria dejar ests 3 lineas, y borrar las de adentro
        bHitNodo = true;
        ImpactPoint = PuntoImpacto;
    }
    else {//si no estoy golpeando algun nodo
        if (HitNodo) {
            if (MostrarLabel) {
                HitNodo->OcultarNombre();
            }
            HitNodo = nullptr;
            bHitNodo = false;
            ImpactPoint = PuntoImpacto;
        }
        //el caso contrario, seria encontrar como lo deje con el if anterior, asi que no se hace nada
    }
    //todo esto podria ser una sola funcion
    //hasta aqui he verificado si encontre algun nodo, pero no si encotnre un, menu, y tampoo he ejecutado los cambios visuales
    //hagamos algo visual, antes de incluir los menus
    if (bHitNodo) {//quiza la verificacion que hago sobre si hubo cambio o no de hit nodo, ayude a evitar ciertos calculos, tal vez, por ejemplo si el laser siempre esa al maximo, no tiene mucho sentido seimpre setear con el mismo valor
        Usuario->CambiarPuntoFinal(ImpactPoint);
    }
    else {
        Usuario->CambiarPuntoFinal(RightController->GetComponentLocation() + RightController->GetForwardVector()*DistanciaLaserMaxima);//debieria tener un punto por defecto, pero mejor lo dejamos asi
        //esta funcion deberia administrar le punto recbido, y verficar si acutalmente el puntero de interaccion esta sobre el menu, y tomar el adecuado para cada situacion
    }
    //creo que la parte de interacion con el menu, deberia estar manajedo por el pawn, asi dentro de la funcion cambiar punto final, evaluo o verifico que no este primero algun menu
    //la pregunta es como hare con los clicks digamos para el contenido, si estoy buscando algun nodo, quiza igual deberia evitar que de algun click, si tengo algun overlap en ferente, evaluar la mejor forma de hacer todo esto
    //o usar esto en lugar de un trace solo que debo hacer esto antes de que haga cambios visuales, obtener el punto y evaluar,  antes de setear lo de hit nodo y dema
}*/

/*void ALayout2VRVisualization::TraslacionVisualizacion() {//esta es una funcion gloabl, ponerlo en la clase padre, analizar estas cosas
    FVector PuntoInicial = RightController->GetComponentLocation();//lo mismo que en teorioa, GetComponentTransfor().GetLocation();
    FVector Vec = RightController->GetForwardVector();
    FVector PuntoFinal = PuntoInicial + Vec*DistanciaLaser;
    if(Usuario->LaserActual() != 6){
        Usuario->CambiarLaser(6);
    }
    Usuario->CambiarPuntoFinal(PuntoFinal);
    SetActorLocation(PuntoFinal + OffsetToPointLaser);
}*/

