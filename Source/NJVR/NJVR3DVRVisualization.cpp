// Fill out your copyright notice in the Description page of Project Settings.

#include "NJVR3DVRVisualization.h"
#include "NJVR.h"
#include "Nodo.h"
#include "Arista.h"
#include "VRPawn.h"
#include "MotionControllerComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include <stack>
#include <queue>

ANJVR3DVRVisualization::ANJVR3DVRVisualization() {
    RadioHoja = 6.0f;
    DeltaDistanciaArista = 2.0f;
    //DeltaDistanciaArista = 0.5f;
}

void ANJVR3DVRVisualization::BeginPlay() {
    Super::BeginPlay();
    //LayoutBase();
    //LayoutDistanciaReducida();
    //LayoutDistanciaAumentada();//el intermedio, funcona bien con los de radio grandes recuciendo el delta para los radios
    LayoutDistanciaAumentadaAnguloReducido();//acoplado bonito

    //LayoutDistanciaAumentadaHijoAnguloReducido();// mas peque�o que el anterior
    ActualizarLayout();
    SetActorScale3D(FVector(0.3f));
}

void ANJVR3DVRVisualization::CreateNodos() {
    FXmlNode * rootnode = XmlSource.GetRootNode();
    //FXmlNode * rootnode = XmlSourceP->GetRootNode();
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

void ANJVR3DVRVisualization::CreateAristas() {
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

void ANJVR3DVRVisualization::AplicarLayout() {
    LayoutDistanciaAumentadaAnguloReducido();//acoplado bonito
    ActualizarLayout();
}

void ANJVR3DVRVisualization::Calculos(ANodo * V) {//lo uso dentro de claculos 2, por alguna razon
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

void ANJVR3DVRVisualization::Calculos2() {//calcula hojas y altura, de otra forma
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

void ANJVR3DVRVisualization::Calc() {//para hallar niveles
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

void ANJVR3DVRVisualization::CalcularRadio(ANodo * V) {
    //rp es el Radio Frame
    if (V->Valid) {
        V->RadioFrame = 6.0f;//un poco mas del doble del diametro de los nodos
        //V->RadioFrame = 1.0f;//un poco mas del doble del diametro de los nodos
        //UE_LOG(LogClass, Log, TEXT("Valid Nodo = %d, RadioFrame %f"), V->Id, V->RadioFrame);
    }
    else{
        CalcularRadio(V->Sons[0]);
        CalcularRadio(V->Sons[1]);
        //V->RadioFrame = FMath::Max(V->Sons[0]->RadioFrame, V->Sons[1]->RadioFrame) + 2;//funciona bien apra el conjunto peque�o
        //V->RadioFrame = FMath::Max(V->Sons[0]->RadioFrame, V->Sons[1]->RadioFrame) + 0.5;//par los otros conjuntos
        V->RadioFrame = FMath::Max(V->Sons[0]->RadioFrame, V->Sons[1]->RadioFrame) + DeltaDistanciaArista;//par los otros conjuntos
        //que deberia depender de la altura o numero de ivevels, y la arista mas grande que deseo obtener
        //UE_LOG(LogClass, Log, TEXT("InValid Nodo = %d, RadioFrame %f"), V->Id, V->RadioFrame);
    }
}

void ANJVR3DVRVisualization::CalcularRadioMin(ANodo * V) {
    //rp es el Radio Frame
    if (V->Valid) {
        V->RadioFrame = 6.0f;//un poco mas del doble del diametro de los nodos
        //V->RadioFrame = 1.0f;//un poco mas del doble del diametro de los nodos
        //UE_LOG(LogClass, Log, TEXT("Valid Nodo = %d, RadioFrame %f"), V->Id, V->RadioFrame);
    }
    else{
        CalcularRadioMin(V->Sons[0]);
        CalcularRadioMin(V->Sons[1]);
        //V->RadioFrame = FMath::Min(V->Sons[0]->RadioFrame, V->Sons[1]->RadioFrame) + 2;//funciona bien apra el conjunto peque�o
        //V->RadioFrame = FMath::Min(V->Sons[0]->RadioFrame, V->Sons[1]->RadioFrame) + 4;//funciona bien apra el conjunto peuqe�o
        //V->RadioFrame = FMath::Min(V->Sons[0]->RadioFrame, V->Sons[1]->RadioFrame) + 0.5;
        V->RadioFrame = FMath::Min(V->Sons[0]->RadioFrame, V->Sons[1]->RadioFrame) + DeltaDistanciaArista;
        //UE_LOG(LogClass, Log, TEXT("InValid Nodo = %d, RadioFrame %f"), V->Id, V->RadioFrame);
    }
}


void ANJVR3DVRVisualization::CalcularRadioHemiesfera(ANodo * V) {
    //rp es el Radio Frame
    if (V->Valid) {
        V->RadioFrame = RadioHoja;
        //UE_LOG(LogClass, Log, TEXT("Valid Nodo = %d, RadioFrame %f"), V->Id, V->RadioFrame);
    }
    else{
        CalcularRadioHemiesfera(V->Sons[0]);
        CalcularRadioHemiesfera(V->Sons[1]);
        float HAp = PI*V->Sons[0]->RadioFrame*V->Sons[0]->RadioFrame + PI*V->Sons[1]->RadioFrame*V->Sons[1]->RadioFrame;
        V->RadioFrame = FMath::Sqrt(HAp/(2*PI));
        //UE_LOG(LogClass, Log, TEXT("InValid Nodo = %d, RadioFrame %f"), V->Id, V->RadioFrame);
    }
}

void ANJVR3DVRVisualization::LayoutBase() {
    TQueue<ANodo *> Cola;
    Calculos2();
    Calc();//no estaba antes
    ANodo * Root = Nodos[Nodos.Num() - 1];

    //calculamos los radios
    float DeltaRadio = RadioHoja / Root->Altura + 1;
    CalcularRadioHemiesfera(Root->Sons[0]);
    CalcularRadioHemiesfera(Root->Sons[1]);
    CalcularRadioHemiesfera(Root->Parent);
    float HAp = PI*Root->Sons[0]->RadioFrame*Root->Sons[0]->RadioFrame + PI*Root->Sons[1]->RadioFrame*Root->Sons[1]->RadioFrame;
    HAp += PI*Root->Parent->RadioFrame*Root->Parent->RadioFrame;
    Root->RadioFrame = FMath::Sqrt(HAp/(2*PI));
    //fin clculo radios

    Root->Theta = 0;
    Root->Phi = 0;
    Root->Xcoordinate = 0.0f;//esta es la posicion general dentro de la visualizacion, 
    Root->Ycoordinate = 0.0f;
    Root->Zcoordinate = 0.0f;
    Root->X = 0.0f;//esta es la posicion relativa respecto al padre
    Root->Y = 0.0f;
    Root->Z = 0.0f;
    UE_LOG(LogClass, Log, TEXT("Root id = %d, (%f,%f,%f)"), Root->Id, Root->Xcoordinate, Root->Ycoordinate, Root->Zcoordinate);

    FMatrix RotacionY;
    FMatrix RotacionZ;
    FMatrix TraslacionV;
    //por ahora los tres primeros se dividiran de forma equitativa, pero despues ya no, sera en base a sus proporciones, 
    //sa debe realizar el calculo de los frames

    Root->Parent->Phi = 0;
    Root->Parent->Theta = 0;
    Root->Parent->X = Root->RadioFrame * FMath::Sin(Root->Parent->Phi) * FMath::Cos(Root->Parent->Theta);
    Root->Parent->Y = Root->RadioFrame * FMath::Sin(Root->Parent->Phi) * FMath::Sin(Root->Parent->Theta);
    Root->Parent->Z = Root->RadioFrame * FMath::Cos(Root->Parent->Phi);
    RotacionY = MatrizRotacionY(Root->Parent->Phi);//3*PI/2 siempre sera este valor
    RotacionZ = MatrizRotacionZ(2 * PI - PI / 2);
    TraslacionV = MatrizTraslacion(Root->Parent->X, Root->Parent->Y, Root->Parent->Z);
    Root->Parent->Frame = MultiplicacionMatriz(MultiplicacionMatriz(TraslacionV, RotacionZ), RotacionY);
    Root->Parent->Xcoordinate = Root->Parent->Frame.M[0][3];
    Root->Parent->Ycoordinate = Root->Parent->Frame.M[1][3];
    Root->Parent->Zcoordinate = Root->Parent->Frame.M[2][3];
    Cola.Enqueue(Root->Parent);
    for (int i = 0; i < Root->Sons.Num(); i++) {
        Root->Sons[i]->Phi = 2*PI/3;
        Root->Sons[i]->Theta = (i & 1) * PI;
        Root->Sons[i]->X = Root->RadioFrame * FMath::Sin(Root->Sons[i]->Phi) * FMath::Cos(Root->Sons[i]->Theta);
        Root->Sons[i]->Y = Root->RadioFrame * FMath::Sin(Root->Sons[i]->Phi) * FMath::Sin(Root->Sons[i]->Theta);
        Root->Sons[i]->Z = Root->RadioFrame * FMath::Cos(Root->Sons[i]->Phi);
        if (i & 1) {
            RotacionY = MatrizRotacionY(Root->Sons[i]->Phi);
        }
        else {
            RotacionY = MatrizRotacionY(2 * PI - Root->Sons[i]->Phi);
        }
        //RotacionY = MatrizRotacionY(2 * PI - Root->Sons[i]->Phi);
        RotacionZ = MatrizRotacionZ(2 * PI - PI / 2);
        TraslacionV = MatrizTraslacion(Root->Sons[i]->X, Root->Sons[i]->Y, Root->Sons[i]->Z);
        Root->Sons[i]->Frame = MultiplicacionMatriz(MultiplicacionMatriz(TraslacionV, RotacionZ), RotacionY);
        Root->Sons[i]->Xcoordinate = Root->Sons[i]->Frame.M[0][3];
        Root->Sons[i]->Ycoordinate = Root->Sons[i]->Frame.M[1][3];
        Root->Sons[i]->Zcoordinate = Root->Sons[i]->Frame.M[2][3];
        Cola.Enqueue(Root->Sons[i]);
    }

    while (!Cola.IsEmpty()) {
        ANodo * V;
        Cola.Dequeue(V);
        UE_LOG(LogClass, Log, TEXT("Nodo id = %d, RadioFrame: %f"), V->Id, V->RadioFrame);
        float PhiTotal = 0.0f;
        for (int i = 0; i < V->Sons.Num(); i++) {
            //V->Sons[i]->Phi = FMath::Atan(V->Sons[i]->RadioFrame / V->RadioFrame);
            V->Sons[i]->Phi = PI / 4;
            PhiTotal += V->Sons[i]->Phi;
        }
        for (int i = 0; i < V->Sons.Num(); i++) {
            V->Sons[i]->Phi = PhiTotal - V->Sons[i]->Phi;//bastaria con asignar defrente Pi?4
            V->Sons[i]->Theta = (i & 1) * PI;// +(V->Nivel & 1) * (PI / 2);//si es el primer hijo, le toca Theta 0, si es el segundo le toca Theta PI, y a ello dependeindo del nivel se le agrega La variacion de theta
            V->Sons[i]->X= V->RadioFrame * FMath::Sin(V->Sons[i]->Phi) * FMath::Cos(V->Sons[i]->Theta);
            V->Sons[i]->Y= V->RadioFrame * FMath::Sin(V->Sons[i]->Phi) * FMath::Sin(V->Sons[i]->Theta);
            V->Sons[i]->Z= V->RadioFrame * FMath::Cos(V->Sons[i]->Phi);
            if (i & 1) {
                RotacionY = MatrizRotacionY(V->Sons[i]->Phi);
            }
            else {
                RotacionY = MatrizRotacionY(2 * PI - V->Sons[i]->Phi);
            }
            //RotacionY = MatrizRotacionY(2 * PI - V->Sons[i]->Phi);
            //RotacionX = MatrizRotacionX(2 * PI - V->Sons[i]->Phi);
            RotacionZ = MatrizRotacionZ(2 * PI - PI / 2);
            /*if (V->Nivel & 1) {
                RotacionZ = MatrizRotacionZ(PI / 2);
            }
            else {
                RotacionZ = MatrizRotacionZ(2 * PI - PI / 2);
            }*/
            TraslacionV = MatrizTraslacion(V->Sons[i]->X, V->Sons[i]->Y, V->Sons[i]->Z);
            V->Sons[i]->Frame = MultiplicacionMatriz(V->Frame, MultiplicacionMatriz(MultiplicacionMatriz(TraslacionV, RotacionZ), RotacionY));
            //V->Sons[i]->Frame = MultiplicacionMatriz(MultiplicacionMatriz(TraslacionV, RotacionX), RotacionZ);
            V->Sons[i]->Xcoordinate = V->Sons[i]->Frame.M[0][3];
            V->Sons[i]->Ycoordinate = V->Sons[i]->Frame.M[1][3];
            V->Sons[i]->Zcoordinate = V->Sons[i]->Frame.M[2][3];
            Cola.Enqueue(V->Sons[i]);
        }
    }
}
//probar otros enoque para la asignacion del radio, quiza que vata de abajo hacia arriba, asi todas las hojas, tienen la arista de la misma distancia con su padre.
//corregir el erro que aparece por ejemplo con el conjunto 5, leafshapte que no posee una orientacioncorrecta.
void ANJVR3DVRVisualization::LayoutDistanciaReducida() {
    TQueue<ANodo *> Cola;
    Calculos2();
    Calc();//no estaba antes
    ANodo * Root = Nodos[Nodos.Num() - 1];

    //calculamos los radios
    //float DeltaRadio = RadioHoja / (Root->Altura + 1);
    float DeltaRadio = RadioHoja / Root->Altura + 1;
    CalcularRadioHemiesfera(Root->Sons[0]);
    CalcularRadioHemiesfera(Root->Sons[1]);
    CalcularRadioHemiesfera(Root->Parent);
    float HAp = PI*Root->Sons[0]->RadioFrame*Root->Sons[0]->RadioFrame + PI*Root->Sons[1]->RadioFrame*Root->Sons[1]->RadioFrame;
    HAp += PI*Root->Parent->RadioFrame*Root->Parent->RadioFrame;
    Root->RadioFrame = FMath::Sqrt(HAp/(2*PI));

    Root->RadioFrame = RadioHoja - DeltaRadio*Root->Nivel;
    //fin clculo radios

    Root->Theta = 0;
    Root->Phi = 0;
    Root->Xcoordinate = 0.0f;//esta es la posicion general dentro de la visualizacion, 
    Root->Ycoordinate = 0.0f;
    Root->Zcoordinate = 0.0f;
    Root->X = 0.0f;//esta es la posicion relativa respecto al padre
    Root->Y = 0.0f;
    Root->Z = 0.0f;
    UE_LOG(LogClass, Log, TEXT("Root id = %d, (%f,%f,%f)"), Root->Id, Root->Xcoordinate, Root->Ycoordinate, Root->Zcoordinate);

    FMatrix RotacionY;
    FMatrix RotacionZ;
    FMatrix TraslacionV;
    //por ahora los tres primeros se dividiran de forma equitativa, pero despues ya no, sera en base a sus proporciones, 
    //sa debe realizar el calculo de los frames

    Root->Parent->RadioFrame = RadioHoja - DeltaRadio*Root->Parent->Nivel;

    Root->Parent->Phi = 0;
    Root->Parent->Theta = 0;
    Root->Parent->X = Root->RadioFrame * FMath::Sin(Root->Parent->Phi) * FMath::Cos(Root->Parent->Theta);
    Root->Parent->Y = Root->RadioFrame * FMath::Sin(Root->Parent->Phi) * FMath::Sin(Root->Parent->Theta);
    Root->Parent->Z = Root->RadioFrame * FMath::Cos(Root->Parent->Phi);
    RotacionY = MatrizRotacionY(Root->Parent->Phi);//3*PI/2 siempre sera este valor
    RotacionZ = MatrizRotacionZ(2 * PI - PI / 2);
    TraslacionV = MatrizTraslacion(Root->Parent->X, Root->Parent->Y, Root->Parent->Z);
    Root->Parent->Frame = MultiplicacionMatriz(MultiplicacionMatriz(TraslacionV, RotacionZ), RotacionY);
    Root->Parent->Xcoordinate = Root->Parent->Frame.M[0][3];
    Root->Parent->Ycoordinate = Root->Parent->Frame.M[1][3];
    Root->Parent->Zcoordinate = Root->Parent->Frame.M[2][3];
    Cola.Enqueue(Root->Parent);
    for (int i = 0; i < Root->Sons.Num(); i++) {

        Root->Sons[i]->RadioFrame = RadioHoja - DeltaRadio*Root->Sons[i]->Nivel;

        Root->Sons[i]->Phi = 2*PI/3;
        Root->Sons[i]->Theta = (i & 1) * PI;
        Root->Sons[i]->X = Root->RadioFrame * FMath::Sin(Root->Sons[i]->Phi) * FMath::Cos(Root->Sons[i]->Theta);
        Root->Sons[i]->Y = Root->RadioFrame * FMath::Sin(Root->Sons[i]->Phi) * FMath::Sin(Root->Sons[i]->Theta);
        Root->Sons[i]->Z = Root->RadioFrame * FMath::Cos(Root->Sons[i]->Phi);
        //RotacionY = MatrizRotacionY(2 * PI - Root->Sons[i]->Phi);
        if (i & 1) {
            RotacionY = MatrizRotacionY(Root->Sons[i]->Phi);
        }
        else {
            RotacionY = MatrizRotacionY(2 * PI - Root->Sons[i]->Phi);
        }
        RotacionZ = MatrizRotacionZ(2 * PI - PI / 2);
        TraslacionV = MatrizTraslacion(Root->Sons[i]->X, Root->Sons[i]->Y, Root->Sons[i]->Z);
        Root->Sons[i]->Frame = MultiplicacionMatriz(MultiplicacionMatriz(TraslacionV, RotacionZ), RotacionY);
        Root->Sons[i]->Xcoordinate = Root->Sons[i]->Frame.M[0][3];
        Root->Sons[i]->Ycoordinate = Root->Sons[i]->Frame.M[1][3];
        Root->Sons[i]->Zcoordinate = Root->Sons[i]->Frame.M[2][3];
        Cola.Enqueue(Root->Sons[i]);
    }

    while (!Cola.IsEmpty()) {
        ANodo * V;
        Cola.Dequeue(V);
        UE_LOG(LogClass, Log, TEXT("Nodo id = %d, RadioFrame: %f"), V->Id, V->RadioFrame);
        float PhiTotal = 0.0f;
        for (int i = 0; i < V->Sons.Num(); i++) {
            //V->Sons[i]->Phi = FMath::Atan(V->Sons[i]->RadioFrame / V->RadioFrame);
            V->Sons[i]->Phi = PI / 4;
            PhiTotal += V->Sons[i]->Phi;
        }
        for (int i = 0; i < V->Sons.Num(); i++) {

            V->Sons[i]->RadioFrame = RadioHoja - DeltaRadio*V->Sons[i]->Nivel;

            V->Sons[i]->Phi = PhiTotal - V->Sons[i]->Phi;//bastaria con asignar defrente Pi?4
            V->Sons[i]->Theta = (i & 1) * PI;// +(V->Nivel & 1) * (PI / 2);//si es el primer hijo, le toca Theta 0, si es el segundo le toca Theta PI, y a ello dependeindo del nivel se le agrega La variacion de theta
            V->Sons[i]->X= V->RadioFrame * FMath::Sin(V->Sons[i]->Phi) * FMath::Cos(V->Sons[i]->Theta);
            V->Sons[i]->Y= V->RadioFrame * FMath::Sin(V->Sons[i]->Phi) * FMath::Sin(V->Sons[i]->Theta);
            V->Sons[i]->Z= V->RadioFrame * FMath::Cos(V->Sons[i]->Phi);
            if (i & 1) {
                RotacionY = MatrizRotacionY(V->Sons[i]->Phi);
            }
            else {
                RotacionY = MatrizRotacionY(2 * PI - V->Sons[i]->Phi);
            }
            //RotacionY = MatrizRotacionY(2 * PI - V->Sons[i]->Phi);
            //RotacionX = MatrizRotacionX(2 * PI - V->Sons[i]->Phi);
            RotacionZ = MatrizRotacionZ(2 * PI - PI / 2);
            /*if (V->Nivel & 1) {
                RotacionZ = MatrizRotacionZ(PI / 2);
            }
            else {
                RotacionZ = MatrizRotacionZ(2 * PI - PI / 2);
            }*/
            TraslacionV = MatrizTraslacion(V->Sons[i]->X, V->Sons[i]->Y, V->Sons[i]->Z);
            V->Sons[i]->Frame = MultiplicacionMatriz(V->Frame, MultiplicacionMatriz(MultiplicacionMatriz(TraslacionV, RotacionZ), RotacionY));
            //V->Sons[i]->Frame = MultiplicacionMatriz(MultiplicacionMatriz(TraslacionV, RotacionX), RotacionZ);
            V->Sons[i]->Xcoordinate = V->Sons[i]->Frame.M[0][3];
            V->Sons[i]->Ycoordinate = V->Sons[i]->Frame.M[1][3];
            V->Sons[i]->Zcoordinate = V->Sons[i]->Frame.M[2][3];
            Cola.Enqueue(V->Sons[i]);
        }
    }
}

void ANJVR3DVRVisualization::LayoutDistanciaAumentada() {
    TQueue<ANodo *> Cola;
    Calculos2();
    Calc();//no estaba antes
    ANodo * Root = Nodos[Nodos.Num() - 1];

    //calculamos los radios
    //float DeltaRadio = RadioHoja / (Root->Altura + 1);
    CalcularRadio(Root->Sons[0]);
    CalcularRadio(Root->Sons[1]);
    CalcularRadio(Root->Parent);
    Root->RadioFrame = FMath::Max3(Root->Sons[0]->RadioFrame, Root->Sons[1]->RadioFrame, Root->Parent->RadioFrame) + 2.0f;
    //Root->RadioFrame = FMath::Max3(Root->Sons[0]->RadioFrame, Root->Sons[1]->RadioFrame, Root->Parent->RadioFrame) + DeltaDistanciaArista;
    //fin clculo radios

    Root->Theta = 0;
    Root->Phi = 0;
    Root->Xcoordinate = 0.0f;//esta es la posicion general dentro de la visualizacion, 
    Root->Ycoordinate = 0.0f;
    Root->Zcoordinate = 0.0f;
    Root->X = 0.0f;//esta es la posicion relativa respecto al padre
    Root->Y = 0.0f;
    Root->Z = 0.0f;
    UE_LOG(LogClass, Log, TEXT("Root id = %d, (%f,%f,%f)"), Root->Id, Root->Xcoordinate, Root->Ycoordinate, Root->Zcoordinate);

    FMatrix RotacionY;
    FMatrix RotacionZ;
    FMatrix TraslacionV;
    //por ahora los tres primeros se dividiran de forma equitativa, pero despues ya no, sera en base a sus proporciones, 
    //sa debe realizar el calculo de los frames
    Root->Parent->Phi = 0;
    Root->Parent->Theta = 0;
    Root->Parent->X = Root->RadioFrame * FMath::Sin(Root->Parent->Phi) * FMath::Cos(Root->Parent->Theta);
    Root->Parent->Y = Root->RadioFrame * FMath::Sin(Root->Parent->Phi) * FMath::Sin(Root->Parent->Theta);
    Root->Parent->Z = Root->RadioFrame * FMath::Cos(Root->Parent->Phi);
    RotacionY = MatrizRotacionY(Root->Parent->Phi);//3*PI/2 siempre sera este valor
    RotacionZ = MatrizRotacionZ(2 * PI - PI / 2);
    TraslacionV = MatrizTraslacion(Root->Parent->X, Root->Parent->Y, Root->Parent->Z);
    Root->Parent->Frame = MultiplicacionMatriz(MultiplicacionMatriz(TraslacionV, RotacionZ), RotacionY);
    Root->Parent->Xcoordinate = Root->Parent->Frame.M[0][3];
    Root->Parent->Ycoordinate = Root->Parent->Frame.M[1][3];
    Root->Parent->Zcoordinate = Root->Parent->Frame.M[2][3];
    Cola.Enqueue(Root->Parent);
    for (int i = 0; i < Root->Sons.Num(); i++) {
        Root->Sons[i]->Phi = 2*PI/3;
        Root->Sons[i]->Theta = (i & 1) * PI;
        Root->Sons[i]->X = Root->RadioFrame * FMath::Sin(Root->Sons[i]->Phi) * FMath::Cos(Root->Sons[i]->Theta);
        Root->Sons[i]->Y = Root->RadioFrame * FMath::Sin(Root->Sons[i]->Phi) * FMath::Sin(Root->Sons[i]->Theta);
        Root->Sons[i]->Z = Root->RadioFrame * FMath::Cos(Root->Sons[i]->Phi);
        //RotacionY = MatrizRotacionY(2 * PI - Root->Sons[i]->Phi);
        if (i & 1) {
            RotacionY = MatrizRotacionY(Root->Sons[i]->Phi);
        }
        else {
            RotacionY = MatrizRotacionY(2 * PI - Root->Sons[i]->Phi);
        }
        RotacionZ = MatrizRotacionZ(2 * PI - PI / 2);
        TraslacionV = MatrizTraslacion(Root->Sons[i]->X, Root->Sons[i]->Y, Root->Sons[i]->Z);
        Root->Sons[i]->Frame = MultiplicacionMatriz(MultiplicacionMatriz(TraslacionV, RotacionZ), RotacionY);
        Root->Sons[i]->Xcoordinate = Root->Sons[i]->Frame.M[0][3];
        Root->Sons[i]->Ycoordinate = Root->Sons[i]->Frame.M[1][3];
        Root->Sons[i]->Zcoordinate = Root->Sons[i]->Frame.M[2][3];
        Cola.Enqueue(Root->Sons[i]);
    }

    while (!Cola.IsEmpty()) {
        ANodo * V;
        Cola.Dequeue(V);
        UE_LOG(LogClass, Log, TEXT("Nodo id = %d, RadioFrame: %f"), V->Id, V->RadioFrame);
        float PhiTotal = 0.0f;
        for (int i = 0; i < V->Sons.Num(); i++) {
            //V->Sons[i]->Phi = FMath::Atan(V->Sons[i]->RadioFrame / V->RadioFrame);
            V->Sons[i]->Phi = PI / 4;
            PhiTotal += V->Sons[i]->Phi;
        }
        for (int i = 0; i < V->Sons.Num(); i++) {
            V->Sons[i]->Phi = PhiTotal - V->Sons[i]->Phi;//bastaria con asignar defrente Pi?4
            V->Sons[i]->Theta = (i & 1) * PI;// +(V->Nivel & 1) * (PI / 2);//si es el primer hijo, le toca Theta 0, si es el segundo le toca Theta PI, y a ello dependeindo del nivel se le agrega La variacion de theta
            V->Sons[i]->X= V->RadioFrame * FMath::Sin(V->Sons[i]->Phi) * FMath::Cos(V->Sons[i]->Theta);
            V->Sons[i]->Y= V->RadioFrame * FMath::Sin(V->Sons[i]->Phi) * FMath::Sin(V->Sons[i]->Theta);
            V->Sons[i]->Z= V->RadioFrame * FMath::Cos(V->Sons[i]->Phi);
            if (i & 1) {
                RotacionY = MatrizRotacionY(V->Sons[i]->Phi);
            }
            else {
                RotacionY = MatrizRotacionY(2 * PI - V->Sons[i]->Phi);
            }
            //RotacionY = MatrizRotacionY(2 * PI - V->Sons[i]->Phi);
            //RotacionX = MatrizRotacionX(2 * PI - V->Sons[i]->Phi);
            RotacionZ = MatrizRotacionZ(2 * PI - PI / 2);
            /*if (V->Nivel & 1) {
                RotacionZ = MatrizRotacionZ(PI / 2);
            }
            else {
                RotacionZ = MatrizRotacionZ(2 * PI - PI / 2);
            }*/
            TraslacionV = MatrizTraslacion(V->Sons[i]->X, V->Sons[i]->Y, V->Sons[i]->Z);
            V->Sons[i]->Frame = MultiplicacionMatriz(V->Frame, MultiplicacionMatriz(MultiplicacionMatriz(TraslacionV, RotacionZ), RotacionY));
            //V->Sons[i]->Frame = MultiplicacionMatriz(MultiplicacionMatriz(TraslacionV, RotacionX), RotacionZ);
            V->Sons[i]->Xcoordinate = V->Sons[i]->Frame.M[0][3];
            V->Sons[i]->Ycoordinate = V->Sons[i]->Frame.M[1][3];
            V->Sons[i]->Zcoordinate = V->Sons[i]->Frame.M[2][3];
            Cola.Enqueue(V->Sons[i]);
        }
    }
}


void ANJVR3DVRVisualization::LayoutDistanciaAumentadaAnguloReducido() {
    TQueue<ANodo *> Cola;
    Calculos2();
    Calc();//no estaba antes
    ANodo * Root = Nodos[Nodos.Num() - 1];

    //calculamos los radios
    float DeltaPhi = PI/4 / (Root->Altura + 1);
    CalcularRadio(Root->Sons[0]);
    CalcularRadio(Root->Sons[1]);
    CalcularRadio(Root->Parent);
    Root->RadioFrame = FMath::Max3(Root->Sons[0]->RadioFrame, Root->Sons[1]->RadioFrame, Root->Parent->RadioFrame) + 2.0f;
    //Root->RadioFrame = FMath::Max3(Root->Sons[0]->RadioFrame, Root->Sons[1]->RadioFrame, Root->Parent->RadioFrame) + DeltaDistanciaArista;
    //fin clculo radios

    Root->Theta = 0;
    Root->Phi = 0;
    Root->Xcoordinate = 0.0f;//esta es la posicion general dentro de la visualizacion, 
    Root->Ycoordinate = 0.0f;
    Root->Zcoordinate = 0.0f;
    Root->X = 0.0f;//esta es la posicion relativa respecto al padre
    Root->Y = 0.0f;
    Root->Z = 0.0f;
    UE_LOG(LogClass, Log, TEXT("Root id = %d, (%f,%f,%f)"), Root->Id, Root->Xcoordinate, Root->Ycoordinate, Root->Zcoordinate);

    FMatrix RotacionY;
    FMatrix RotacionZ;
    FMatrix TraslacionV;
    //por ahora los tres primeros se dividiran de forma equitativa, pero despues ya no, sera en base a sus proporciones, 
    //sa debe realizar el calculo de los frames
    Root->Parent->Phi = 0;
    Root->Parent->Theta = 0;
    Root->Parent->X = Root->RadioFrame * FMath::Sin(Root->Parent->Phi) * FMath::Cos(Root->Parent->Theta);
    Root->Parent->Y = Root->RadioFrame * FMath::Sin(Root->Parent->Phi) * FMath::Sin(Root->Parent->Theta);
    Root->Parent->Z = Root->RadioFrame * FMath::Cos(Root->Parent->Phi);
    RotacionY = MatrizRotacionY(Root->Parent->Phi);//3*PI/2 siempre sera este valor
    RotacionZ = MatrizRotacionZ(2 * PI - PI / 2);
    TraslacionV = MatrizTraslacion(Root->Parent->X, Root->Parent->Y, Root->Parent->Z);
    Root->Parent->Frame = MultiplicacionMatriz(MultiplicacionMatriz(TraslacionV, RotacionZ), RotacionY);
    Root->Parent->Xcoordinate = Root->Parent->Frame.M[0][3];
    Root->Parent->Ycoordinate = Root->Parent->Frame.M[1][3];
    Root->Parent->Zcoordinate = Root->Parent->Frame.M[2][3];
    Cola.Enqueue(Root->Parent);
    for (int i = 0; i < Root->Sons.Num(); i++) {
        Root->Sons[i]->Phi = 2*PI/3;
        Root->Sons[i]->Theta = (i & 1) * PI;
        Root->Sons[i]->X = Root->RadioFrame * FMath::Sin(Root->Sons[i]->Phi) * FMath::Cos(Root->Sons[i]->Theta);
        Root->Sons[i]->Y = Root->RadioFrame * FMath::Sin(Root->Sons[i]->Phi) * FMath::Sin(Root->Sons[i]->Theta);
        Root->Sons[i]->Z = Root->RadioFrame * FMath::Cos(Root->Sons[i]->Phi);
        //RotacionY = MatrizRotacionY(2 * PI - Root->Sons[i]->Phi);
        if (i & 1) {
            RotacionY = MatrizRotacionY(Root->Sons[i]->Phi);
        }
        else {
            RotacionY = MatrizRotacionY(2 * PI - Root->Sons[i]->Phi);
        }
        RotacionZ = MatrizRotacionZ(2 * PI - PI / 2);
        TraslacionV = MatrizTraslacion(Root->Sons[i]->X, Root->Sons[i]->Y, Root->Sons[i]->Z);
        Root->Sons[i]->Frame = MultiplicacionMatriz(MultiplicacionMatriz(TraslacionV, RotacionZ), RotacionY);
        Root->Sons[i]->Xcoordinate = Root->Sons[i]->Frame.M[0][3];
        Root->Sons[i]->Ycoordinate = Root->Sons[i]->Frame.M[1][3];
        Root->Sons[i]->Zcoordinate = Root->Sons[i]->Frame.M[2][3];
        Cola.Enqueue(Root->Sons[i]);
    }

    while (!Cola.IsEmpty()) {
        ANodo * V;
        Cola.Dequeue(V);
        UE_LOG(LogClass, Log, TEXT("Nodo id = %d, RadioFrame: %f"), V->Id, V->RadioFrame);
        float PhiTotal = 0.0f;
        for (int i = 0; i < V->Sons.Num(); i++) {
            //V->Sons[i]->Phi = FMath::Atan(V->Sons[i]->RadioFrame / V->RadioFrame);
            V->Sons[i]->Phi = PI / 4 - DeltaPhi*V->Nivel;
            PhiTotal += V->Sons[i]->Phi;
        }
        for (int i = 0; i < V->Sons.Num(); i++) {
            V->Sons[i]->Phi = PI/4 - DeltaPhi*V->Nivel;//bastaria con asignar defrente Pi?4
            V->Sons[i]->Theta = (i & 1) * PI;// +(V->Nivel & 1) * (PI / 2);//si es el primer hijo, le toca Theta 0, si es el segundo le toca Theta PI, y a ello dependeindo del nivel se le agrega La variacion de theta
            V->Sons[i]->X= V->RadioFrame * FMath::Sin(V->Sons[i]->Phi) * FMath::Cos(V->Sons[i]->Theta);
            V->Sons[i]->Y= V->RadioFrame * FMath::Sin(V->Sons[i]->Phi) * FMath::Sin(V->Sons[i]->Theta);
            V->Sons[i]->Z= V->RadioFrame * FMath::Cos(V->Sons[i]->Phi);
            if (i & 1) {
                RotacionY = MatrizRotacionY(V->Sons[i]->Phi);
            }
            else {
                RotacionY = MatrizRotacionY(2 * PI - V->Sons[i]->Phi);
            }
            //RotacionY = MatrizRotacionY(2 * PI - V->Sons[i]->Phi);
            //RotacionX = MatrizRotacionX(2 * PI - V->Sons[i]->Phi);
            RotacionZ = MatrizRotacionZ(2 * PI - PI / 2);
            /*if (V->Nivel & 1) {
                RotacionZ = MatrizRotacionZ(PI / 2);
            }
            else {
                RotacionZ = MatrizRotacionZ(2 * PI - PI / 2);
            }*/
            TraslacionV = MatrizTraslacion(V->Sons[i]->X, V->Sons[i]->Y, V->Sons[i]->Z);
            V->Sons[i]->Frame = MultiplicacionMatriz(V->Frame, MultiplicacionMatriz(MultiplicacionMatriz(TraslacionV, RotacionZ), RotacionY));
            //V->Sons[i]->Frame = MultiplicacionMatriz(MultiplicacionMatriz(TraslacionV, RotacionX), RotacionZ);
            V->Sons[i]->Xcoordinate = V->Sons[i]->Frame.M[0][3];
            V->Sons[i]->Ycoordinate = V->Sons[i]->Frame.M[1][3];
            V->Sons[i]->Zcoordinate = V->Sons[i]->Frame.M[2][3];
            Cola.Enqueue(V->Sons[i]);
        }
    }
}

void ANJVR3DVRVisualization::LayoutDistanciaAumentadaHijoAnguloReducido() {
    TQueue<ANodo *> Cola;
    Calculos2();
    Calc();//no estaba antes
    ANodo * Root = Nodos[Nodos.Num() - 1];

    //calculamos los radios
    float DeltaPhi = PI/4 / (Root->Altura + 1);
    /*CalcularRadio(Root->Sons[0]);
    CalcularRadio(Root->Sons[1]);
    CalcularRadio(Root->Parent);*/
    CalcularRadioMin(Root->Sons[0]);
    CalcularRadioMin(Root->Sons[1]);
    CalcularRadioMin(Root->Parent);
    Root->RadioFrame = FMath::Max3(Root->Sons[0]->RadioFrame, Root->Sons[1]->RadioFrame, Root->Parent->RadioFrame) + 2.0f;
    //Root->RadioFrame = FMath::Max3(Root->Sons[0]->RadioFrame, Root->Sons[1]->RadioFrame, Root->Parent->RadioFrame) + DeltaDistanciaArista;
    //fin clculo radios

    Root->Theta = 0;
    Root->Phi = 0;
    Root->Xcoordinate = 0.0f;//esta es la posicion general dentro de la visualizacion, 
    Root->Ycoordinate = 0.0f;
    Root->Zcoordinate = 0.0f;
    Root->X = 0.0f;//esta es la posicion relativa respecto al padre
    Root->Y = 0.0f;
    Root->Z = 0.0f;
    UE_LOG(LogClass, Log, TEXT("Root id = %d, (%f,%f,%f)"), Root->Id, Root->Xcoordinate, Root->Ycoordinate, Root->Zcoordinate);

    FMatrix RotacionY;
    FMatrix RotacionZ;
    FMatrix TraslacionV;
    //por ahora los tres primeros se dividiran de forma equitativa, pero despues ya no, sera en base a sus proporciones, 
    //sa debe realizar el calculo de los frames
    Root->Parent->Phi = 0;
    Root->Parent->Theta = 0;
    Root->Parent->X = Root->Parent->RadioFrame * FMath::Sin(Root->Parent->Phi) * FMath::Cos(Root->Parent->Theta);
    Root->Parent->Y = Root->Parent->RadioFrame * FMath::Sin(Root->Parent->Phi) * FMath::Sin(Root->Parent->Theta);
    Root->Parent->Z = Root->Parent->RadioFrame * FMath::Cos(Root->Parent->Phi);
    RotacionY = MatrizRotacionY(Root->Parent->Phi);//3*PI/2 siempre sera este valor
    RotacionZ = MatrizRotacionZ(2 * PI - PI / 2);
    TraslacionV = MatrizTraslacion(Root->Parent->X, Root->Parent->Y, Root->Parent->Z);
    Root->Parent->Frame = MultiplicacionMatriz(MultiplicacionMatriz(TraslacionV, RotacionZ), RotacionY);
    Root->Parent->Xcoordinate = Root->Parent->Frame.M[0][3];
    Root->Parent->Ycoordinate = Root->Parent->Frame.M[1][3];
    Root->Parent->Zcoordinate = Root->Parent->Frame.M[2][3];
    Cola.Enqueue(Root->Parent);
    for (int i = 0; i < Root->Sons.Num(); i++) {
        Root->Sons[i]->Phi = 2*PI/3;
        Root->Sons[i]->Theta = (i & 1) * PI;
        Root->Sons[i]->X = Root->Sons[i]->RadioFrame * FMath::Sin(Root->Sons[i]->Phi) * FMath::Cos(Root->Sons[i]->Theta);
        Root->Sons[i]->Y = Root->Sons[i]->RadioFrame * FMath::Sin(Root->Sons[i]->Phi) * FMath::Sin(Root->Sons[i]->Theta);
        Root->Sons[i]->Z = Root->Sons[i]->RadioFrame * FMath::Cos(Root->Sons[i]->Phi);
        //RotacionY = MatrizRotacionY(2 * PI - Root->Sons[i]->Phi);
        if (i & 1) {
            RotacionY = MatrizRotacionY(Root->Sons[i]->Phi);
        }
        else {
            RotacionY = MatrizRotacionY(2 * PI - Root->Sons[i]->Phi);
        }
        RotacionZ = MatrizRotacionZ(2 * PI - PI / 2);
        TraslacionV = MatrizTraslacion(Root->Sons[i]->X, Root->Sons[i]->Y, Root->Sons[i]->Z);
        Root->Sons[i]->Frame = MultiplicacionMatriz(MultiplicacionMatriz(TraslacionV, RotacionZ), RotacionY);
        Root->Sons[i]->Xcoordinate = Root->Sons[i]->Frame.M[0][3];
        Root->Sons[i]->Ycoordinate = Root->Sons[i]->Frame.M[1][3];
        Root->Sons[i]->Zcoordinate = Root->Sons[i]->Frame.M[2][3];
        Cola.Enqueue(Root->Sons[i]);
    }

    while (!Cola.IsEmpty()) {
        ANodo * V;
        Cola.Dequeue(V);
        UE_LOG(LogClass, Log, TEXT("Nodo id = %d, RadioFrame: %f"), V->Id, V->RadioFrame);
        float PhiTotal = 0.0f;
        for (int i = 0; i < V->Sons.Num(); i++) {
            //V->Sons[i]->Phi = FMath::Atan(V->Sons[i]->RadioFrame / V->RadioFrame);
            V->Sons[i]->Phi = PI / 4 - DeltaPhi*V->Nivel;
            PhiTotal += V->Sons[i]->Phi;
        }
        for (int i = 0; i < V->Sons.Num(); i++) {
            V->Sons[i]->Phi = PI/4 - DeltaPhi*V->Nivel;//bastaria con asignar defrente Pi?4
            V->Sons[i]->Theta = (i & 1) * PI;// +(V->Nivel & 1) * (PI / 2);//si es el primer hijo, le toca Theta 0, si es el segundo le toca Theta PI, y a ello dependeindo del nivel se le agrega La variacion de theta
            V->Sons[i]->X= V->Sons[i]->RadioFrame * FMath::Sin(V->Sons[i]->Phi) * FMath::Cos(V->Sons[i]->Theta);
            V->Sons[i]->Y= V->Sons[i]->RadioFrame * FMath::Sin(V->Sons[i]->Phi) * FMath::Sin(V->Sons[i]->Theta);
            V->Sons[i]->Z= V->Sons[i]->RadioFrame * FMath::Cos(V->Sons[i]->Phi);
            if (i & 1) {
                RotacionY = MatrizRotacionY(V->Sons[i]->Phi);
            }
            else {
                RotacionY = MatrizRotacionY(2 * PI - V->Sons[i]->Phi);
            }
            //RotacionY = MatrizRotacionY(2 * PI - V->Sons[i]->Phi);
            //RotacionX = MatrizRotacionX(2 * PI - V->Sons[i]->Phi);
            RotacionZ = MatrizRotacionZ(2 * PI - PI / 2);
            /*if (V->Nivel & 1) {
                RotacionZ = MatrizRotacionZ(PI / 2);
            }
            else {
                RotacionZ = MatrizRotacionZ(2 * PI - PI / 2);
            }*/
            TraslacionV = MatrizTraslacion(V->Sons[i]->X, V->Sons[i]->Y, V->Sons[i]->Z);
            V->Sons[i]->Frame = MultiplicacionMatriz(V->Frame, MultiplicacionMatriz(MultiplicacionMatriz(TraslacionV, RotacionZ), RotacionY));
            //V->Sons[i]->Frame = MultiplicacionMatriz(MultiplicacionMatriz(TraslacionV, RotacionX), RotacionZ);
            V->Sons[i]->Xcoordinate = V->Sons[i]->Frame.M[0][3];
            V->Sons[i]->Ycoordinate = V->Sons[i]->Frame.M[1][3];
            V->Sons[i]->Zcoordinate = V->Sons[i]->Frame.M[2][3];
            Cola.Enqueue(V->Sons[i]);
        }
    }
}

void ANJVR3DVRVisualization::ActualizarLayout() {//este actulizar deberia ser general
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

void ANJVR3DVRVisualization::AplicarTraslacion(FVector Traslacion) {
    for (int i = 0; i < NodosSeleccionados.Num(); i++) {
        //NodosSeleccionados[i]->AddActorLocalOffset(Traslacion);//no esoty seguro si esto funcone
        NodosSeleccionados[i]->SetActorLocation(NodosSeleccionados[i]->GetActorLocation() + Traslacion);
    }
    //se debe actualizar las matrices o frames de cada nodo y a los que se afecten
}

void ANJVR3DVRVisualization::TraslacionConNodoGuia() {
    FVector PuntoInicial = RightController->GetComponentLocation();//lo mismo que en teorioa, GetComponentTransfor().GetLocation();
    FVector Vec = RightController->GetForwardVector();
    FVector PuntoFinal = PuntoInicial + Vec*DistanciaLaser;
    if(Usuario->LaserActual() != 6){
        Usuario->CambiarLaser(6);
    }
    Usuario->CambiarPuntoFinal(PuntoFinal);
    AplicarTraslacion(PuntoFinal - NodoGuia->GetActorLocation());
}

void ANJVR3DVRVisualization::TrasladarRamaPressed() {//para el traslado en 3d
    if (bHitNodo) {
        SeleccionarRama(HitNodo);
        NodoGuia = HitNodo;
        bNodoGuia = true;
        DistanciaLaser = (ImpactPoint - RightController->GetComponentLocation()).Size();
    }
}


FMatrix ANJVR3DVRVisualization::MatrizTraslacion(float x, float y, float z) {
    FMatrix Traslacion;
    Traslacion.M[0][0] = 1;
    Traslacion.M[0][1] = 0;
    Traslacion.M[0][2] = 0;
    Traslacion.M[0][3] = x;
    Traslacion.M[1][0] = 0;
    Traslacion.M[1][1] = 1;
    Traslacion.M[1][2] = 0;
    Traslacion.M[1][3] = y;
    Traslacion.M[2][0] = 0;
    Traslacion.M[2][1] = 0;
    Traslacion.M[2][2] = 1;
    Traslacion.M[2][3] = z;
    Traslacion.M[3][0] = 0;
    Traslacion.M[3][1] = 0;
    Traslacion.M[3][2] = 0;
    Traslacion.M[3][3] = 1;
    return Traslacion;
}

FMatrix ANJVR3DVRVisualization::MatrizRotacionX(float angle) {
    FMatrix RotX;
    RotX.M[0][0] = cos(angle);
    RotX.M[0][1] = 0;
    RotX.M[0][2] = sin(angle);
    RotX.M[0][3] = 0;
    RotX.M[1][0] = 0;
    RotX.M[1][1] = 1;
    RotX.M[1][2] = 0;
    RotX.M[1][3] = 0;
    RotX.M[2][0] = -sin(angle);
    RotX.M[2][1] = 0;
    RotX.M[2][2] = cos(angle);
    RotX.M[2][3] = 0;
    RotX.M[3][0] = 0;
    RotX.M[3][1] = 0;
    RotX.M[3][2] = 0;
    RotX.M[3][3] = 1;
    return RotX;
}

FMatrix ANJVR3DVRVisualization::MatrizRotacionY(float angle) {
    FMatrix RotY;
    RotY.M[0][0] = 1;
    RotY.M[0][1] = 0;
    RotY.M[0][2] = 0;
    RotY.M[0][3] = 0;
    RotY.M[1][0] = 0;
    RotY.M[1][1] = cos(angle);
    RotY.M[1][2] = -sin(angle);
    RotY.M[1][3] = 0;
    RotY.M[2][0] = 0;
    RotY.M[2][1] = sin(angle);
    RotY.M[2][2] = cos(angle);
    RotY.M[2][3] = 0;
    RotY.M[3][0] = 0;
    RotY.M[3][1] = 0;
    RotY.M[3][2] = 0;
    RotY.M[3][3] = 1;
    return RotY;
}

FMatrix ANJVR3DVRVisualization::MatrizRotacionZ(float angle) {
    FMatrix RotZ;//en este caso la matriz z es la identidad por que theta de V es 0 y eso al realziar calculos es la matriz identdad;
    RotZ.M[0][0] = cos(angle);
    RotZ.M[0][1] = -sin(angle);
    RotZ.M[0][2] = 0;
    RotZ.M[0][3] = 0;
    RotZ.M[1][0] = sin(angle);
    RotZ.M[1][1] = cos(angle);
    RotZ.M[1][2] = 0;
    RotZ.M[1][3] = 0;
    RotZ.M[2][0] = 0;
    RotZ.M[2][1] = 0;
    RotZ.M[2][2] = 1;
    RotZ.M[2][3] = 0;
    RotZ.M[3][0] = 0;
    RotZ.M[3][1] = 0;
    RotZ.M[3][2] = 0;
    RotZ.M[3][3] = 1;
    return RotZ;
}

FMatrix ANJVR3DVRVisualization::MultiplicacionMatriz(FMatrix a, FMatrix b) {
    FMatrix c;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            c.M[i][j] = 0;
            for (int k = 0; k < 4; k++) {
                c.M[i][j] += a.M[i][k] * b.M[k][j];
            }
        }
    }
    return c;
}

void ANJVR3DVRVisualization::ImprimirMatriz(FMatrix m) {
    for (int i = 0; i < 4; i++) {
        UE_LOG(LogClass, Log, TEXT("[%.4f,%.4f,%.4f,%.4f]"), m.M[i][0], m.M[i][1], m.M[i][2], m.M[i][3]);
    }
}



