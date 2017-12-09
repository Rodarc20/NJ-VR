// Fill out your copyright notice in the Description page of Project Settings.

#include "NJVR.h"
#include "H3VRVisualization.h"
#include "Nodo.h"
#include "Arista.h"
#include "VRPawn.h"
#include "MotionControllerComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include <stack>
#include <queue>
#include <math.h>

AH3VRVisualization::AH3VRVisualization() {
    RadioHoja = 0.5f;
    K = 2.0f;
    HemisphereAreaScale = 7.2f;
    LeafArea = 0.005f;
    EscalaSalida = 200.0f;
//valores por defecto para las escalas 1.25, 0.75, 0.25
    RadioNodos = 1.25;
    RadioNodosVirtuales = 0.75;
    RadioAristas = 0.25;

}

void AH3VRVisualization::BeginPlay() {
    Super::BeginPlay();
    //LayoutBase();
    LayoutBase2();
    //LayoutBaseH3();

    //ActualizarLayout();
    ActualizarLayoutH3();
}

void AH3VRVisualization::CreateNodos() {
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

void AH3VRVisualization::CreateAristas() {
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

void AH3VRVisualization::Calculos(ANodo * V) {//lo uso dentro de claculos 2, por alguna razon
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

void AH3VRVisualization::Calculos2() {//calcula hojas y altura, de otra forma
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

void AH3VRVisualization::Calc() {//para hallar niveles
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

void AH3VRVisualization::CalcularRadio(ANodo * V) {
    //rp es el Radio Frame
    if (V->Valid) {
        V->RadioFrame = 6.0f;//un poco mas del doble del diametro de los nodos
        //V->RadioFrame = 1.0f;//un poco mas del doble del diametro de los nodos
        //UE_LOG(LogClass, Log, TEXT("Valid Nodo = %d, RadioFrame %f"), V->Id, V->RadioFrame);
    }
    else{
        CalcularRadio(V->Sons[0]);
        CalcularRadio(V->Sons[1]);
        V->RadioFrame = FMath::Max(V->Sons[0]->RadioFrame, V->Sons[1]->RadioFrame) + 2;//funciona bien apra el conjunto pequeño
        //V->RadioFrame = FMath::Max(V->Sons[0]->RadioFrame, V->Sons[1]->RadioFrame) + 0.5;//par los otros conjuntos
        //que deberia depender de la altura o numero de ivevels, y la arista mas grande que deseo obtener
        //UE_LOG(LogClass, Log, TEXT("InValid Nodo = %d, RadioFrame %f"), V->Id, V->RadioFrame);
    }
}

void AH3VRVisualization::CalcularRadioHemiesfera(ANodo * V) {
    //rp es el Radio Frame
    if (V->Valid) {
        //V->RadioFrame = RadioHoja;
        //V->RadioFrame = K * std::asinh(std::sqrt(0.005f / (2*PI * K * K)));
        V->RadioFrame = K * std::asinh(std::sqrt(LeafArea / (2*PI * K * K)));
        //UE_LOG(LogClass, Log, TEXT("Valid Nodo = %d, RadioFrame %f"), V->Id, V->RadioFrame);
    }
    else{
        CalcularRadioHemiesfera(V->Sons[0]);
        CalcularRadioHemiesfera(V->Sons[1]);
        //float HAp = PI*V->Sons[0]->RadioFrame*V->Sons[0]->RadioFrame + PI*V->Sons[1]->RadioFrame*V->Sons[1]->RadioFrame;
        //V->RadioFrame = FMath::Sqrt(HAp/(2*PI));
        //float HAp = 2*PI*(std::cosh(V->Sons[0]->RadioFrame)-1) + 2*PI*(std::cosh(V->Sons[1]->RadioFrame)-1);
        //V->RadioFrame = std::asinh(FMath::Sqrt(HAp/(2*PI)));
        float HAp = 2*PI*(std::cosh(V->Sons[0]->RadioFrame/K)-1) + 2*PI*(std::cosh(V->Sons[1]->RadioFrame/K)-1);
        //HAp *= 100.0f;
        //HAp *= 7.2f;
        HAp *= HemisphereAreaScale;
        V->RadioFrame = K*std::asinh(FMath::Sqrt(HAp/(2*PI*K*K)));
        //UE_LOG(LogClass, Log, TEXT("InValid Nodo = %d, RadioFrame %f"), V->Id, V->RadioFrame);
    }
}

void AH3VRVisualization::LayoutBase() {//esta produciendo un layout bueno
    TQueue<ANodo *> Cola;
    Calculos2();
    Calc();//no estaba antes
    ANodo * Root = Nodos[Nodos.Num() - 1];

    //calculamos los radios
    float DeltaRadio = RadioHoja / Root->Altura + 1;
    CalcularRadioHemiesfera(Root->Sons[0]);
    CalcularRadioHemiesfera(Root->Sons[1]);
    CalcularRadioHemiesfera(Root->Parent);
    //float HAp = PI*Root->Sons[0]->RadioFrame*Root->Sons[0]->RadioFrame + PI*Root->Sons[1]->RadioFrame*Root->Sons[1]->RadioFrame;
    //HAp += PI*Root->Parent->RadioFrame*Root->Parent->RadioFrame;
    //float HAp = 2*PI*(std::cosh(Root->Sons[0]->RadioFrame)-1) + 2*PI*(std::cosh(Root->Sons[1]->RadioFrame)-1);
    //HAp += 2*PI*(std::cosh(Root->Parent->RadioFrame)-1);
    //Root->RadioFrame = std::asinh(FMath::Sqrt(HAp/(2*PI)));
    //fin clculo radios
    float HAp = 2*PI*(std::cosh(Root->Sons[0]->RadioFrame/K)-1) + 2*PI*(std::cosh(Root->Sons[1]->RadioFrame/K)-1);
    HAp += 2*PI*(std::cosh(Root->Parent->RadioFrame/K)-1);
    HAp *= HemisphereAreaScale;
    //HAp *= 7.2f;
    Root->RadioFrame = K*std::asinh(FMath::Sqrt(HAp/(2*PI*K*K)));

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
    //Root->Parent->X = Root->RadioFrame * FMath::Sin(Root->Parent->Phi) * FMath::Cos(Root->Parent->Theta);
    //Root->Parent->Y = Root->RadioFrame * FMath::Sin(Root->Parent->Phi) * FMath::Sin(Root->Parent->Theta);
    //Root->Parent->Z = Root->RadioFrame * FMath::Cos(Root->Parent->Phi);
    Root->Parent->X = ConvertirADistanciaEuclideana(Root->RadioFrame + Root->Parent->RadioFrame) * FMath::Sin(Root->Parent->Phi) * FMath::Cos(Root->Parent->Theta);
    Root->Parent->Y = ConvertirADistanciaEuclideana(Root->RadioFrame + Root->Parent->RadioFrame) * FMath::Sin(Root->Parent->Phi) * FMath::Sin(Root->Parent->Theta);
    Root->Parent->Z = ConvertirADistanciaEuclideana(Root->RadioFrame + Root->Parent->RadioFrame) * FMath::Cos(Root->Parent->Phi);
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
        Root->Sons[i]->X = ConvertirADistanciaEuclideana(Root->RadioFrame + Root->Sons[i]->RadioFrame) * FMath::Sin(Root->Sons[i]->Phi) * FMath::Cos(Root->Sons[i]->Theta);
        Root->Sons[i]->Y = ConvertirADistanciaEuclideana(Root->RadioFrame + Root->Sons[i]->RadioFrame) * FMath::Sin(Root->Sons[i]->Phi) * FMath::Sin(Root->Sons[i]->Theta);
        Root->Sons[i]->Z = ConvertirADistanciaEuclideana(Root->RadioFrame + Root->Sons[i]->RadioFrame) * FMath::Cos(Root->Sons[i]->Phi);
        //Root->Sons[i]->X = Root->RadioFrame * FMath::Sin(Root->Sons[i]->Phi) * FMath::Cos(Root->Sons[i]->Theta);
        //Root->Sons[i]->Y = Root->RadioFrame * FMath::Sin(Root->Sons[i]->Phi) * FMath::Sin(Root->Sons[i]->Theta);
        //Root->Sons[i]->Z = Root->RadioFrame * FMath::Cos(Root->Sons[i]->Phi);
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
            //V->Sons[i]->Phi = CalcularDeltaPhi(V->Sons[i]->RadioFrame, V->RadioFrame);
            PhiTotal += V->Sons[i]->Phi;
        }
        for (int i = 0; i < V->Sons.Num(); i++) {
            //V->Sons[i]->Phi = PhiTotal - V->Sons[i]->Phi;//bastaria con asignar defrente Pi?4
            V->Sons[i]->Theta = (i & 1) * PI;// +(V->Nivel & 1) * (PI / 2);//si es el primer hijo, le toca Theta 0, si es el segundo le toca Theta PI, y a ello dependeindo del nivel se le agrega La variacion de theta
            //V->Sons[i]->X= V->RadioFrame * FMath::Sin(V->Sons[i]->Phi) * FMath::Cos(V->Sons[i]->Theta);
            //V->Sons[i]->Y= V->RadioFrame * FMath::Sin(V->Sons[i]->Phi) * FMath::Sin(V->Sons[i]->Theta);
            //V->Sons[i]->Z= V->RadioFrame * FMath::Cos(V->Sons[i]->Phi);
            V->Sons[i]->X = ConvertirADistanciaEuclideana(V->Sons[i]->RadioFrame + V->RadioFrame) * FMath::Sin(V->Sons[i]->Phi) * FMath::Cos(V->Sons[i]->Theta);
            V->Sons[i]->Y = ConvertirADistanciaEuclideana(V->Sons[i]->RadioFrame + V->RadioFrame) * FMath::Sin(V->Sons[i]->Phi) * FMath::Sin(V->Sons[i]->Theta);
            V->Sons[i]->Z = ConvertirADistanciaEuclideana(V->Sons[i]->RadioFrame + V->RadioFrame) * FMath::Cos(V->Sons[i]->Phi);
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


void AH3VRVisualization::LayoutBase2() {//esta produciendo un layout bueno
    //usar escala salida con valor 100
    TQueue<ANodo *> Cola;
    Calculos2();
    Calc();//no estaba antes
    ANodo * Root = Nodos[Nodos.Num() - 1];

    //calculamos los radios
    float DeltaRadio = RadioHoja / Root->Altura + 1;
    CalcularRadioHemiesfera(Root->Sons[0]);
    CalcularRadioHemiesfera(Root->Sons[1]);
    CalcularRadioHemiesfera(Root->Parent);
    //float HAp = PI*Root->Sons[0]->RadioFrame*Root->Sons[0]->RadioFrame + PI*Root->Sons[1]->RadioFrame*Root->Sons[1]->RadioFrame;
    //HAp += PI*Root->Parent->RadioFrame*Root->Parent->RadioFrame;
    //float HAp = 2*PI*(std::cosh(Root->Sons[0]->RadioFrame)-1) + 2*PI*(std::cosh(Root->Sons[1]->RadioFrame)-1);
    //HAp += 2*PI*(std::cosh(Root->Parent->RadioFrame)-1);
    //Root->RadioFrame = std::asinh(FMath::Sqrt(HAp/(2*PI)));
    //fin clculo radios
    float HAp = 2*PI*(std::cosh(Root->Sons[0]->RadioFrame/K)-1) + 2*PI*(std::cosh(Root->Sons[1]->RadioFrame/K)-1);
    HAp += 2*PI*(std::cosh(Root->Parent->RadioFrame/K)-1);
    HAp *= HemisphereAreaScale;
    //HAp *= 7.2f;
    Root->RadioFrame = K*std::asinh(FMath::Sqrt(HAp/(2*PI*K*K)));

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
    //Root->Parent->X = Root->RadioFrame * FMath::Sin(Root->Parent->Phi) * FMath::Cos(Root->Parent->Theta);
    //Root->Parent->Y = Root->RadioFrame * FMath::Sin(Root->Parent->Phi) * FMath::Sin(Root->Parent->Theta);
    //Root->Parent->Z = Root->RadioFrame * FMath::Cos(Root->Parent->Phi);
    //Root->Parent->X = ConvertirADistanciaEuclideana(Root->RadioFrame) * FMath::Sin(Root->Parent->Phi) * FMath::Cos(Root->Parent->Theta);
    //Root->Parent->Y = ConvertirADistanciaEuclideana(Root->RadioFrame) * FMath::Sin(Root->Parent->Phi) * FMath::Sin(Root->Parent->Theta);
    //Root->Parent->Z = ConvertirADistanciaEuclideana(Root->RadioFrame) * FMath::Cos(Root->Parent->Phi);
    Root->Parent->X = ConvertirADistanciaEuclideana(Root->RadioFrame + Root->Parent->RadioFrame) * FMath::Sin(Root->Parent->Phi) * FMath::Cos(Root->Parent->Theta);
    Root->Parent->Y = ConvertirADistanciaEuclideana(Root->RadioFrame + Root->Parent->RadioFrame) * FMath::Sin(Root->Parent->Phi) * FMath::Sin(Root->Parent->Theta);
    Root->Parent->Z = ConvertirADistanciaEuclideana(Root->RadioFrame + Root->Parent->RadioFrame) * FMath::Cos(Root->Parent->Phi);
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
        Root->Sons[i]->X = ConvertirADistanciaEuclideana(Root->RadioFrame + Root->Sons[i]->RadioFrame) * FMath::Sin(Root->Sons[i]->Phi) * FMath::Cos(Root->Sons[i]->Theta);
        Root->Sons[i]->Y = ConvertirADistanciaEuclideana(Root->RadioFrame + Root->Sons[i]->RadioFrame) * FMath::Sin(Root->Sons[i]->Phi) * FMath::Sin(Root->Sons[i]->Theta);
        Root->Sons[i]->Z = ConvertirADistanciaEuclideana(Root->RadioFrame + Root->Sons[i]->RadioFrame) * FMath::Cos(Root->Sons[i]->Phi);
        //Root->Sons[i]->X = ConvertirADistanciaEuclideana(Root->RadioFrame) * FMath::Sin(Root->Sons[i]->Phi) * FMath::Cos(Root->Sons[i]->Theta);
        //Root->Sons[i]->Y = ConvertirADistanciaEuclideana(Root->RadioFrame) * FMath::Sin(Root->Sons[i]->Phi) * FMath::Sin(Root->Sons[i]->Theta);
        //Root->Sons[i]->Z = ConvertirADistanciaEuclideana(Root->RadioFrame) * FMath::Cos(Root->Sons[i]->Phi);
        //Root->Sons[i]->X = Root->RadioFrame * FMath::Sin(Root->Sons[i]->Phi) * FMath::Cos(Root->Sons[i]->Theta);
        //Root->Sons[i]->Y = Root->RadioFrame * FMath::Sin(Root->Sons[i]->Phi) * FMath::Sin(Root->Sons[i]->Theta);
        //Root->Sons[i]->Z = Root->RadioFrame * FMath::Cos(Root->Sons[i]->Phi);
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
            //V->Sons[i]->Phi = PI / 4;
            V->Sons[i]->Phi = CalcularDeltaPhi(V->Sons[i]->RadioFrame, V->RadioFrame);
            PhiTotal += V->Sons[i]->Phi;
        }
        for (int i = 0; i < V->Sons.Num(); i++) {
            V->Sons[i]->Phi = PhiTotal - V->Sons[i]->Phi;//bastaria con asignar defrente Pi?4
            V->Sons[i]->Theta = (i & 1) * PI;// +(V->Nivel & 1) * (PI / 2);//si es el primer hijo, le toca Theta 0, si es el segundo le toca Theta PI, y a ello dependeindo del nivel se le agrega La variacion de theta
            //V->Sons[i]->X= V->RadioFrame * FMath::Sin(V->Sons[i]->Phi) * FMath::Cos(V->Sons[i]->Theta);
            //V->Sons[i]->Y= V->RadioFrame * FMath::Sin(V->Sons[i]->Phi) * FMath::Sin(V->Sons[i]->Theta);
            //V->Sons[i]->Z= V->RadioFrame * FMath::Cos(V->Sons[i]->Phi);
            //V->Sons[i]->X = ConvertirADistanciaEuclideana(V->Sons[i]->RadioFrame) * FMath::Sin(V->Sons[i]->Phi) * FMath::Cos(V->Sons[i]->Theta);
            //V->Sons[i]->Y = ConvertirADistanciaEuclideana(V->Sons[i]->RadioFrame) * FMath::Sin(V->Sons[i]->Phi) * FMath::Sin(V->Sons[i]->Theta);
            //V->Sons[i]->Z = ConvertirADistanciaEuclideana(V->Sons[i]->RadioFrame) * FMath::Cos(V->Sons[i]->Phi);
            V->Sons[i]->X = ConvertirADistanciaEuclideana(V->Sons[i]->RadioFrame + V->RadioFrame) * FMath::Sin(V->Sons[i]->Phi) * FMath::Cos(V->Sons[i]->Theta);
            V->Sons[i]->Y = ConvertirADistanciaEuclideana(V->Sons[i]->RadioFrame + V->RadioFrame) * FMath::Sin(V->Sons[i]->Phi) * FMath::Sin(V->Sons[i]->Theta);
            V->Sons[i]->Z = ConvertirADistanciaEuclideana(V->Sons[i]->RadioFrame + V->RadioFrame) * FMath::Cos(V->Sons[i]->Phi);
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

void AH3VRVisualization::CalcularRadioHemiesferaH3(ANodo * V) {
    //rp es el Radio Frame
    if (V->Valid) {
        V->RadioFrame = RadioHoja;
        //V->RadioFrame = K * std::asinh(std::sqrt(0.005f / (2*PI * K * K)));
        //V->RadioFrame = std::asinh(std::sqrt(LeafArea / (2*PI)));
        //V->RadioFrame = std::asinh(std::sqrt(1.0f / (2*PI)));
        //V->RadioFrame = std::asinh(std::sqrt(0.5f / (2*PI)));
        //UE_LOG(LogClass, Log, TEXT("Valid Nodo = %d, RadioFrame %f"), V->Id, V->RadioFrame);
    }
    else{
        CalcularRadioHemiesferaH3(V->Sons[0]);
        CalcularRadioHemiesferaH3(V->Sons[1]);
        //float HAp = PI*V->Sons[0]->RadioFrame*V->Sons[0]->RadioFrame + PI*V->Sons[1]->RadioFrame*V->Sons[1]->RadioFrame;
        //V->RadioFrame = FMath::Sqrt(HAp/(2*PI));
        //float HAp = 2*PI*(std::cosh(V->Sons[0]->RadioFrame)-1) + 2*PI*(std::cosh(V->Sons[1]->RadioFrame)-1);
        //V->RadioFrame = std::asinh(FMath::Sqrt(HAp/(2*PI)));
        float HAp = 2*PI*(std::cosh(V->Sons[0]->RadioFrame)-1) + 2*PI*(std::cosh(V->Sons[1]->RadioFrame)-1);
        //HAp *= 100.0f;
        //HAp *= 7.2f;
        V->RadioFrame = std::asinh(FMath::Sqrt(HAp/(2*PI)));
        //UE_LOG(LogClass, Log, TEXT("InValid Nodo = %d, RadioFrame %f"), V->Id, V->RadioFrame);
    }
}

void AH3VRVisualization::LayoutBaseH3() {// layout del paper h3
    TQueue<ANodo *> Cola;
    Calculos2();
    Calc();//no estaba antes
    ANodo * Root = Nodos[Nodos.Num() - 1];

    //calculamos los radios
    float DeltaRadio = RadioHoja / Root->Altura + 1;
    CalcularRadioHemiesferaH3(Root->Sons[0]);
    CalcularRadioHemiesferaH3(Root->Sons[1]);
    CalcularRadioHemiesferaH3(Root->Parent);
    //fin clculo radios
    float HAp = 2*PI*(std::cosh(Root->Sons[0]->RadioFrame)-1) + 2*PI*(std::cosh(Root->Sons[1]->RadioFrame)-1);
    HAp += 2*PI*(std::cosh(Root->Parent->RadioFrame)-1);
    Root->RadioFrame = std::asinh(FMath::Sqrt(HAp/(2*PI)));
    //Root->RadioFrame = 0.1f;

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

    //Root->Parent->RadioFrame = 0.1f;
    //Root->Sons[0]->RadioFrame = 0.1f;
    //Root->Sons[1]->RadioFrame = 0.1f;
    Root->Parent->Phi = 0;
    Root->Parent->Theta = 0;
    Root->Parent->X = Root->RadioFrame * FMath::Sin(Root->Parent->Phi) * FMath::Cos(Root->Parent->Theta);
    Root->Parent->Y = Root->RadioFrame * FMath::Sin(Root->Parent->Phi) * FMath::Sin(Root->Parent->Theta);
    Root->Parent->Z = Root->RadioFrame * FMath::Cos(Root->Parent->Phi);
    //Root->Parent->X = ConvertirADistanciaEuclideana(Root->RadioFrame) * FMath::Sin(Root->Parent->Phi) * FMath::Cos(Root->Parent->Theta);
    //Root->Parent->Y = ConvertirADistanciaEuclideana(Root->RadioFrame) * FMath::Sin(Root->Parent->Phi) * FMath::Sin(Root->Parent->Theta);
    //Root->Parent->Z = ConvertirADistanciaEuclideana(Root->RadioFrame) * FMath::Cos(Root->Parent->Phi);
    //Root->Parent->X = ConvertirADistanciaEuclideana(Root->RadioFrame + Root->Parent->RadioFrame) * FMath::Sin(Root->Parent->Phi) * FMath::Cos(Root->Parent->Theta);
    //Root->Parent->Y = ConvertirADistanciaEuclideana(Root->RadioFrame + Root->Parent->RadioFrame) * FMath::Sin(Root->Parent->Phi) * FMath::Sin(Root->Parent->Theta);
    //Root->Parent->Z = ConvertirADistanciaEuclideana(Root->RadioFrame + Root->Parent->RadioFrame) * FMath::Cos(Root->Parent->Phi);
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
        //Root->Sons[i]->X = ConvertirADistanciaEuclideana(Root->RadioFrame + Root->Sons[i]->RadioFrame) * FMath::Sin(Root->Sons[i]->Phi) * FMath::Cos(Root->Sons[i]->Theta);
        //Root->Sons[i]->Y = ConvertirADistanciaEuclideana(Root->RadioFrame + Root->Sons[i]->RadioFrame) * FMath::Sin(Root->Sons[i]->Phi) * FMath::Sin(Root->Sons[i]->Theta);
        //Root->Sons[i]->Z = ConvertirADistanciaEuclideana(Root->RadioFrame + Root->Sons[i]->RadioFrame) * FMath::Cos(Root->Sons[i]->Phi);
        //Root->Sons[i]->X = ConvertirADistanciaEuclideana(Root->RadioFrame) * FMath::Sin(Root->Sons[i]->Phi) * FMath::Cos(Root->Sons[i]->Theta);
        //Root->Sons[i]->Y = ConvertirADistanciaEuclideana(Root->RadioFrame) * FMath::Sin(Root->Sons[i]->Phi) * FMath::Sin(Root->Sons[i]->Theta);
        //Root->Sons[i]->Z = ConvertirADistanciaEuclideana(Root->RadioFrame) * FMath::Cos(Root->Sons[i]->Phi);
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
        //V->RadioFrame = 0.1f;
        for (int i = 0; i < V->Sons.Num(); i++) {
            //V->Sons[i]->Phi = FMath::Atan(V->Sons[i]->RadioFrame / V->RadioFrame);
            //V->Sons[i]->Phi = PI / 4;
            V->Sons[i]->Phi = CalcularDeltaPhi(V->Sons[i]->RadioFrame, V->RadioFrame);
            PhiTotal += V->Sons[i]->Phi;
        }
        for (int i = 0; i < V->Sons.Num(); i++) {
            V->Sons[i]->Phi = PhiTotal - V->Sons[i]->Phi;//bastaria con asignar defrente Pi?4
            V->Sons[i]->Theta = (i & 1) * PI;// +(V->Nivel & 1) * (PI / 2);//si es el primer hijo, le toca Theta 0, si es el segundo le toca Theta PI, y a ello dependeindo del nivel se le agrega La variacion de theta
            V->Sons[i]->X = V->RadioFrame * FMath::Sin(V->Sons[i]->Phi) * FMath::Cos(V->Sons[i]->Theta);
            V->Sons[i]->Y = V->RadioFrame * FMath::Sin(V->Sons[i]->Phi) * FMath::Sin(V->Sons[i]->Theta);
            V->Sons[i]->Z = V->RadioFrame * FMath::Cos(V->Sons[i]->Phi);
            //V->Sons[i]->X = ConvertirADistanciaEuclideana(V->RadioFrame) * FMath::Sin(V->Sons[i]->Phi) * FMath::Cos(V->Sons[i]->Theta);
            //V->Sons[i]->Y = ConvertirADistanciaEuclideana(V->RadioFrame) * FMath::Sin(V->Sons[i]->Phi) * FMath::Sin(V->Sons[i]->Theta);
            //V->Sons[i]->Z = ConvertirADistanciaEuclideana(V->RadioFrame) * FMath::Cos(V->Sons[i]->Phi);
            //V->Sons[i]->X = ConvertirADistanciaEuclideana(V->Sons[i]->RadioFrame + V->RadioFrame) * FMath::Sin(V->Sons[i]->Phi) * FMath::Cos(V->Sons[i]->Theta);
            //V->Sons[i]->Y = ConvertirADistanciaEuclideana(V->Sons[i]->RadioFrame + V->RadioFrame) * FMath::Sin(V->Sons[i]->Phi) * FMath::Sin(V->Sons[i]->Theta);
            //V->Sons[i]->Z = ConvertirADistanciaEuclideana(V->Sons[i]->RadioFrame + V->RadioFrame) * FMath::Cos(V->Sons[i]->Phi);
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

void AH3VRVisualization::ActualizarLayout() {//este actulizar deberia ser general
    for (int i = 0; i < Nodos.Num(); i++) {
        FVector NuevaPosicion;
        NuevaPosicion.X = Nodos[i]->Xcoordinate;
        NuevaPosicion.Y = Nodos[i]->Ycoordinate;
        NuevaPosicion.Z = Nodos[i]->Zcoordinate;
        UE_LOG(LogClass, Log, TEXT("NodoHyp id = %d, (%f,%f,%f), RadioFrame = %f"), Nodos[i]->Id, NuevaPosicion.X, NuevaPosicion.Y, NuevaPosicion.Z, Nodos[i]->RadioFrame);
        NuevaPosicion *= EscalaSalida;
        //Nodos[i]->SetActorLocation(NuevaPosicion);
        Nodos[i]->SetActorRelativeLocation(NuevaPosicion);
    }
    for (int i = 0; i < Aristas.Num(); i++) {
        Aristas[i]->Actualizar();
    }

}

void AH3VRVisualization::ActualizarLayoutH3() {//este actulizar deberia ser general
    for (int i = 0; i < Nodos.Num(); i++) {
        FVector NuevaPosicion;
        if(Nodos[i]->Xcoordinate < 0.0f){
            NuevaPosicion.X = -1*ConvertirADistanciaEuclideana(-1*Nodos[i]->Xcoordinate);
        }
        else {
            NuevaPosicion.X = ConvertirADistanciaEuclideana(Nodos[i]->Xcoordinate);
        }
        if(Nodos[i]->Ycoordinate < 0.0f){
            NuevaPosicion.Y = -1*ConvertirADistanciaEuclideana(-1*Nodos[i]->Ycoordinate);
        }
        else {
            NuevaPosicion.Y = ConvertirADistanciaEuclideana(Nodos[i]->Ycoordinate);
        }
        if(Nodos[i]->Zcoordinate < 0.0f){
            NuevaPosicion.Z = -1*ConvertirADistanciaEuclideana(-1*Nodos[i]->Zcoordinate);
        }
        else {
            NuevaPosicion.Z = ConvertirADistanciaEuclideana(Nodos[i]->Zcoordinate);
        }
        
        UE_LOG(LogClass, Log, TEXT("NodoHyp id = %d, (%f,%f,%f), RadioFrame = %f"), Nodos[i]->Id, NuevaPosicion.X, NuevaPosicion.Y, NuevaPosicion.Z, Nodos[i]->RadioFrame);
        NuevaPosicion *= EscalaSalida;//escala de salida adecuada es 200
        //Nodos[i]->SetActorLocation(NuevaPosicion);
        Nodos[i]->SetActorRelativeLocation(NuevaPosicion);
    }
    for (int i = 0; i < Aristas.Num(); i++) {
        Aristas[i]->Actualizar();
    }

}


FMatrix AH3VRVisualization::MatrizTraslacion(float x, float y, float z) {
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

FMatrix AH3VRVisualization::MatrizRotacionX(float angle) {
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

FMatrix AH3VRVisualization::MatrizRotacionY(float angle) {
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

FMatrix AH3VRVisualization::MatrizRotacionZ(float angle) {
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

FMatrix AH3VRVisualization::MultiplicacionMatriz(FMatrix a, FMatrix b) {
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

void AH3VRVisualization::ImprimirMatriz(FMatrix m) {
    for (int i = 0; i < 4; i++) {
        UE_LOG(LogClass, Log, TEXT("[%.4f,%.4f,%.4f,%.4f]"), m.M[i][0], m.M[i][1], m.M[i][2], m.M[i][3]);
    }
}


float AH3VRVisualization::ConvertirADistanciaEuclideana(float x) {
    float y = std::cosh(x / 2);
    return std::sqrt(1.0 - 1.0 / (y*y));
}

bool AH3VRVisualization::IsFinite(FMatrix M) {
    bool res = true;
    for (int i = 0; res && i < 4; i++) {
        for (int j = 0; res && j < 4; j++) {
            res &= std::isfinite(M.M[i][j]);
        }
    }
    return res;
}


float AH3VRVisualization::CalcularDeltaPhi(float RadioHijo, float RadioPadre) {
    return std::atan(std::tanh(RadioHijo / K) / std::sinh(RadioPadre / K));
}

//valores por defecto para las escalas 1.25, 0.75, 0.25
