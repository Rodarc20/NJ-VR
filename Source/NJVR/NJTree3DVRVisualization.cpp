// Fill out your copyright notice in the Description page of Project Settings.

#include "NJTree3DVRVisualization.h"
#include "NJVR.h"
#include "Nodo.h"
#include "Arista.h"
#include "VRPawn.h"
#include "MotionControllerComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include <stack>
#include <queue>


ANJTree3DVRVisualization::ANJTree3DVRVisualization(){

    RadioNodos = 2.0f;
    RadioNodosVirtuales = 1.0f;
    RadioAristas = 0.75f;
    DistanciaArista = 20.0f;
    //PhiValido = PI / 4;
    PhiValido = 0;
    PhiInvalido = PI / 2;//virutlaies a nivel del piso
    //PhiInvalido = PI / 4;//virutlaies a nivel del piso
    //PhiInvalido = PI * 3 / 8;//virutlaies a nivel del piso
    //PhiInvalido = 3 * PI / 4;//reultado gracioso
}

void ANJTree3DVRVisualization::BeginPlay() {
    Super::BeginPlay();
    //Layout();
    Layout2();
    //LayoutEsferico();
    ActualizarLayout();
}

void ANJTree3DVRVisualization::CreateNodos() {
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
                NodoInstanciado->Color = ColorVirtual;//tambien debo cambiarle el tama�o
                NodoInstanciado->ColorNum = FCString::Atoi(*colorcdata);
                NodoInstanciado->Radio = RadioNodosVirtuales;
            }
            //actualizar nodo, para cambiar el color o el tama�o si es necesario
            NodoInstanciado->Escala = Escala;
            NodoInstanciado->Actualizar();
            //NodoInstanciado->AttachRootComponentToActor(this);
            NodoInstanciado->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);//segun el compilador de unral debo usar esto
            Nodos.Add(NodoInstanciado);
            //NodoInstanciado->GetRootComponent()->SetupAttachment(RootComponent);// para hacerlo hioj de la visualizaci�n, aunque parece que esto no es suficient
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


void ANJTree3DVRVisualization::CreateAristas() {
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
        //los calculos de tama�ao direccion y posici�n debe estar dentro de la arita solo deberia pasarle la informaci�n referente a los nodos, la rista sola debe autocalcular lo demas
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

void ANJTree3DVRVisualization::Layout() {
    //parece que solo necesito la cantidad de hojas, pero siempre es mejor tener todos los datos
    TQueue<ANodo *> Cola;
    //la raiz es el ultimo nodo
    ANodo * Root = Nodos[Nodos.Num() - 1];
    Calculos2();
    Calc();//no estaba antes
    //agregado para el nuevo radio
    //int NivelDenso, CantidadNodosNivelDenso;
    //NivelMasDenso(NivelDenso, CantidadNodosNivelDenso);
    //NewRadio = EncontrarRadio1(DeltaPhi * NivelDenso, CantidadNodosNivelDenso); 
    //
    Root->Theta = 0;
    Root->Phi = PI/2;//el phi y theta son relativos respecto al padre del nodo
    //en realidad PHI0 sginifica nivel del sulo, pero neceito poner un phi para poder hacer calculos igual para todos, o tendria que hacer muchas diferencias
    Root->WTam = 2*PI;
    Root->WInicio = 0;
    Root->Xcoordinate = 0;
    Root->Ycoordinate = 0;
    Root->Zcoordinate = 0;
    UE_LOG(LogClass, Log, TEXT("Root id = %d, (%f,%f,%f)"), Root->Id, Root->Xcoordinate, Root->Ycoordinate, Root->Zcoordinate);
    //float DeltaPhi = PI / Root->Altura;
    float WTemp = Root->WInicio;
    //debo tener en cuenta al padre para hacer los calculos, ya que esto esta como arbol sin raiz

    if (Root->Parent->Valid) {
        Root->Parent->Phi = PhiValido;
    }
    else {
        Root->Parent->Phi = PhiInvalido;
    }
    Root->Parent->WTam = 2*PI * (float(Root->Parent->Hojas) / Root->Hojas);
    Root->Parent->WInicio = WTemp;
    Root->Parent->Theta = Root->Parent->WInicio + Root->Parent->WTam / 2;

    Root->Parent->Ycoordinate = Root->Ycoordinate + DistanciaArista * FMath::Sin(Root->Parent->Phi) * FMath::Cos(Root->Parent->Theta);
    Root->Parent->Xcoordinate = Root->Xcoordinate + DistanciaArista * FMath::Sin(Root->Parent->Phi) * FMath::Sin(Root->Parent->Theta);
    Root->Parent->Zcoordinate = Root->Zcoordinate + DistanciaArista * FMath::Cos(Root->Parent->Phi);
    WTemp += Root->Parent->WTam;
    Cola.Enqueue(Root->Parent);
    for (int i = 0; i < Root->Sons.Num(); i++) {
        //Root->Sons[i]->Phi = Root->Phi + DeltaPhi;//estaba dividido por 2
        if (Root->Sons[i]->Valid) {
            Root->Sons[i]->Phi = PhiValido;
        }
        else {
            Root->Sons[i]->Phi = PhiInvalido;
        }
        Root->Sons[i]->WTam = 2*PI * (float(Root->Sons[i]->Hojas) / Root->Hojas);
        Root->Sons[i]->WInicio = WTemp;
        Root->Sons[i]->Theta =Root->Sons[i]->WInicio + Root->Sons[i]->WTam / 2;

        Root->Sons[i]->Ycoordinate = Root->Ycoordinate + DistanciaArista * FMath::Sin(Root->Sons[i]->Phi) * FMath::Cos(Root->Sons[i]->Theta);
        Root->Sons[i]->Xcoordinate = Root->Xcoordinate + DistanciaArista * FMath::Sin(Root->Sons[i]->Phi) * FMath::Sin(Root->Sons[i]->Theta);
        Root->Sons[i]->Zcoordinate = Root->Zcoordinate + DistanciaArista * FMath::Cos(Root->Sons[i]->Phi);
        WTemp += Root->Sons[i]->WTam;
        Cola.Enqueue(Root->Sons[i]);
    }

    while (!Cola.IsEmpty()) {
        ANodo * V;
        Cola.Dequeue(V);
        WTemp = V->WInicio;
        for (int i = 0; i < V->Sons.Num(); i++) {
            if (V->Sons[i]->Valid) {
                V->Sons[i]->Phi = PhiValido;
            }
            else {
                V->Sons[i]->Phi = PhiInvalido;
            }
            V->Sons[i]->WTam = 2*PI * (float(V->Sons[i]->Hojas) / Root->Hojas);
            V->Sons[i]->WInicio = WTemp;
            V->Sons[i]->Theta = V->Sons[i]->WInicio + V->Sons[i]->WTam / 2;

            V->Sons[i]->Ycoordinate = V->Ycoordinate + DistanciaArista * FMath::Sin(V->Sons[i]->Phi) * FMath::Cos(V->Sons[i]->Theta);
            V->Sons[i]->Xcoordinate = V->Xcoordinate + DistanciaArista * FMath::Sin(V->Sons[i]->Phi) * FMath::Sin(V->Sons[i]->Theta);
            V->Sons[i]->Zcoordinate = V->Zcoordinate + DistanciaArista * FMath::Cos(V->Sons[i]->Phi);

            WTemp += V->Sons[i]->WTam;
            Cola.Enqueue(V->Sons[i]);
        }
    }
}

void ANJTree3DVRVisualization::Layout2() {
    TQueue<ANodo *> Cola;
    ANodo * Root = Nodos[Nodos.Num() - 1];
    float Phi1 = PI/4;
    //float Phi1 = 3*PI/4;
    float Phi2 = PI/2;
    //float Phi3 = 3*PI/4;
    float Phi3 = PI/4;
    Calculos2();
    Calc();//no estaba antes
    Root->Theta = 0;
    Root->Phi = PI/2;//el phi y theta son relativos respecto al padre del nodo
    //en realidad PHI0 sginifica nivel del sulo, pero neceito poner un phi para poder hacer calculos igual para todos, o tendria que hacer muchas diferencias
    Root->WTam = 2*PI;
    Root->WInicio = 0;
    Root->Xcoordinate = 0;
    Root->Ycoordinate = 0;
    Root->Zcoordinate = 0;
    UE_LOG(LogClass, Log, TEXT("Root id = %d, (%f,%f,%f)"), Root->Id, Root->Xcoordinate, Root->Ycoordinate, Root->Zcoordinate);
    //float DeltaPhi = PI / Root->Altura;
    float WTemp = Root->WInicio;
    //debo tener en cuenta al padre para hacer los calculos, ya que esto esta como arbol sin raiz

    if (Root->Parent->Valid) {
        Root->Parent->Phi = Phi3;
    }
    else {
        if(!Root->Parent->Sons[0]->Valid && !Root->Parent->Sons[1]->Valid)
            Root->Parent->Phi = Phi1;
        else
            Root->Parent->Phi = Phi2;
    }
    Root->Parent->WTam = 2*PI * (float(Root->Parent->Hojas) / Root->Hojas);
    Root->Parent->WInicio = WTemp;
    Root->Parent->Theta = Root->Parent->WInicio + Root->Parent->WTam / 2;

    Root->Parent->Ycoordinate = Root->Ycoordinate + DistanciaArista * FMath::Sin(Root->Parent->Phi) * FMath::Cos(Root->Parent->Theta);
    Root->Parent->Xcoordinate = Root->Xcoordinate + DistanciaArista * FMath::Sin(Root->Parent->Phi) * FMath::Sin(Root->Parent->Theta);
    Root->Parent->Zcoordinate = Root->Zcoordinate + DistanciaArista * FMath::Cos(Root->Parent->Phi);
    WTemp += Root->Parent->WTam;
    Cola.Enqueue(Root->Parent);
    for (int i = 0; i < Root->Sons.Num(); i++) {
        //Root->Sons[i]->Phi = Root->Phi + DeltaPhi;//estaba dividido por 2
        if (Root->Sons[i]->Valid) {
            Root->Sons[i]->Phi = Phi3;
        }
        else {
            if(!Root->Sons[i]->Sons[0]->Valid && !Root->Sons[i]->Sons[1]->Valid)
                Root->Sons[i]->Phi = Phi1;
            else
                Root->Sons[i]->Phi = Phi2;
        }
        Root->Sons[i]->WTam = 2*PI * (float(Root->Sons[i]->Hojas) / Root->Hojas);
        Root->Sons[i]->WInicio = WTemp;
        Root->Sons[i]->Theta =Root->Sons[i]->WInicio + Root->Sons[i]->WTam / 2;

        Root->Sons[i]->Ycoordinate = Root->Ycoordinate + DistanciaArista * FMath::Sin(Root->Sons[i]->Phi) * FMath::Cos(Root->Sons[i]->Theta);
        Root->Sons[i]->Xcoordinate = Root->Xcoordinate + DistanciaArista * FMath::Sin(Root->Sons[i]->Phi) * FMath::Sin(Root->Sons[i]->Theta);
        Root->Sons[i]->Zcoordinate = Root->Zcoordinate + DistanciaArista * FMath::Cos(Root->Sons[i]->Phi);
        WTemp += Root->Sons[i]->WTam;
        Cola.Enqueue(Root->Sons[i]);
    }

    while (!Cola.IsEmpty()) {
        ANodo * V;
        Cola.Dequeue(V);
        WTemp = V->WInicio;
        for (int i = 0; i < V->Sons.Num(); i++) {
            if (V->Sons[i]->Valid) {
                V->Sons[i]->Phi = Phi3;
            }
            else {
                if(!V->Sons[i]->Sons[0]->Valid && !V->Sons[i]->Sons[1]->Valid)
                    V->Sons[i]->Phi = Phi1;
                else
                    V->Sons[i]->Phi = Phi2;
            }
            V->Sons[i]->WTam = 2*PI * (float(V->Sons[i]->Hojas) / Root->Hojas);
            V->Sons[i]->WInicio = WTemp;
            V->Sons[i]->Theta = V->Sons[i]->WInicio + V->Sons[i]->WTam / 2;

            V->Sons[i]->Ycoordinate = V->Ycoordinate + DistanciaArista * FMath::Sin(V->Sons[i]->Phi) * FMath::Cos(V->Sons[i]->Theta);
            V->Sons[i]->Xcoordinate = V->Xcoordinate + DistanciaArista * FMath::Sin(V->Sons[i]->Phi) * FMath::Sin(V->Sons[i]->Theta);
            V->Sons[i]->Zcoordinate = V->Zcoordinate + DistanciaArista * FMath::Cos(V->Sons[i]->Phi);

            WTemp += V->Sons[i]->WTam;
            Cola.Enqueue(V->Sons[i]);
        }
    }
}

void ANJTree3DVRVisualization::LayoutEsferico() {
    //parece que solo necesito la cantidad de hojas, pero siempre es mejor tener todos los datos
    TQueue<ANodo *> Cola;
    //la raiz es el ultimo nodo
    ANodo * Root = Nodos[Nodos.Num() - 1];
    Calculos2();
    Calc();//no estaba antes
    //agregado para el nuevo radio
    //int NivelDenso, CantidadNodosNivelDenso;
    //NivelMasDenso(NivelDenso, CantidadNodosNivelDenso);
    //NewRadio = EncontrarRadio1(DeltaPhi * NivelDenso, CantidadNodosNivelDenso); 
    //
    Root->Theta = 0;
    Root->Phi = PI/2;//el phi y theta son relativos respecto al padre del nodo
    //en realidad PHI0 sginifica nivel del sulo, pero neceito poner un phi para poder hacer calculos igual para todos, o tendria que hacer muchas diferencias
    Root->WTam = 2*PI;
    Root->WInicio = 0;
    Root->Xcoordinate = 0;
    Root->Ycoordinate = 0;
    Root->Zcoordinate = 0;
    UE_LOG(LogClass, Log, TEXT("Root id = %d, (%f,%f,%f)"), Root->Id, Root->Xcoordinate, Root->Ycoordinate, Root->Zcoordinate);
    //float DeltaPhi = PI / Root->Altura/2;//para usar la mitad de una esfera masomenos
    float DeltaPhi = PI / Root->Altura;
    float WTemp = Root->WInicio;
    //debo tener en cuenta al padre para hacer los calculos, ya que esto esta como arbol sin raiz

    Root->Parent->Phi = Root->Phi + DeltaPhi;
    Root->Parent->WTam = 2*PI * (float(Root->Parent->Hojas) / Root->Hojas);
    Root->Parent->WInicio = WTemp;
    Root->Parent->Theta = Root->Parent->WInicio + Root->Parent->WTam / 2;

    Root->Parent->Ycoordinate = Root->Ycoordinate + DistanciaArista * FMath::Sin(Root->Parent->Phi) * FMath::Cos(Root->Parent->Theta);
    Root->Parent->Xcoordinate = Root->Xcoordinate + DistanciaArista * FMath::Sin(Root->Parent->Phi) * FMath::Sin(Root->Parent->Theta);
    Root->Parent->Zcoordinate = Root->Zcoordinate + DistanciaArista * FMath::Cos(Root->Parent->Phi);
    WTemp += Root->Parent->WTam;
    Cola.Enqueue(Root->Parent);
    for (int i = 0; i < Root->Sons.Num(); i++) {
        //Root->Sons[i]->Phi = Root->Phi + DeltaPhi;//estaba dividido por 2
        Root->Sons[i]->Phi = Root->Phi + DeltaPhi;
        Root->Sons[i]->WTam = 2*PI * (float(Root->Sons[i]->Hojas) / Root->Hojas);
        Root->Sons[i]->WInicio = WTemp;
        Root->Sons[i]->Theta =Root->Sons[i]->WInicio + Root->Sons[i]->WTam / 2;

        Root->Sons[i]->Ycoordinate = Root->Ycoordinate + DistanciaArista * FMath::Sin(Root->Sons[i]->Phi) * FMath::Cos(Root->Sons[i]->Theta);
        Root->Sons[i]->Xcoordinate = Root->Xcoordinate + DistanciaArista * FMath::Sin(Root->Sons[i]->Phi) * FMath::Sin(Root->Sons[i]->Theta);
        Root->Sons[i]->Zcoordinate = Root->Zcoordinate + DistanciaArista * FMath::Cos(Root->Sons[i]->Phi);
        WTemp += Root->Sons[i]->WTam;
        Cola.Enqueue(Root->Sons[i]);
    }

    while (!Cola.IsEmpty()) {
        ANodo * V;
        Cola.Dequeue(V);
        WTemp = V->WInicio;
        for (int i = 0; i < V->Sons.Num(); i++) {
            V->Sons[i]->Phi = V->Phi + DeltaPhi;
            V->Sons[i]->WTam = 2*PI * (float(V->Sons[i]->Hojas) / Root->Hojas);
            V->Sons[i]->WInicio = WTemp;
            V->Sons[i]->Theta = V->Sons[i]->WInicio + V->Sons[i]->WTam / 2;
            //si el phi es mayor a PI/2 entonces debo tomar el theta como negativo, pero solo para el calculo, de las posiciones, nada mas
            if (V->Sons[i]->Phi > PI) {//para la cruvatura hacia adentro
                V->Sons[i]->Ycoordinate = V->Ycoordinate + DistanciaArista * FMath::Sin(V->Sons[i]->Phi) * FMath::Cos(-1 * V->Sons[i]->Theta);
                V->Sons[i]->Xcoordinate = V->Xcoordinate + DistanciaArista * FMath::Sin(V->Sons[i]->Phi) * FMath::Sin(-1 * V->Sons[i]->Theta);
            }
            else {
                V->Sons[i]->Ycoordinate = V->Ycoordinate + DistanciaArista * FMath::Sin(V->Sons[i]->Phi) * FMath::Cos(V->Sons[i]->Theta);
                V->Sons[i]->Xcoordinate = V->Xcoordinate + DistanciaArista * FMath::Sin(V->Sons[i]->Phi) * FMath::Sin(V->Sons[i]->Theta);
            }
            V->Sons[i]->Zcoordinate = V->Zcoordinate + DistanciaArista * FMath::Cos(V->Sons[i]->Phi);

            WTemp += V->Sons[i]->WTam;
            Cola.Enqueue(V->Sons[i]);
        }
    }
}

void ANJTree3DVRVisualization::ActualizarLayout() {
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

//los calculos por ahora estan igual que los del layout1, intentar generalizar ya que estos calculos deben esar aplicados de forma general para todos
//en todos lso noodes deberian tener la cantidad de hojas altura y nivle, asi no lo use el layout, pero quiza sea informacion util para otra cosa

void ANJTree3DVRVisualization::Calculos(ANodo * V) {//lo uso dentro de claculos 2, por alguna razon
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

void ANJTree3DVRVisualization::Calculos2() {//calcula hojas y altura, de otra forma
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

void ANJTree3DVRVisualization::Calc() {//para hallar niveles
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




