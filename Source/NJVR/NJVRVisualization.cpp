// Fill out your copyright notice in the Description page of Project Settings.

#include "NJVR.h"
#include "NJVRVisualization.h"
#include "Nodo.h"
#include "Arista.h"
#include "VRPawn.h"
#include "MotionControllerComponent.h"
#include "Kismet/KismetSystemLibrary.h"

ANJVRVisualization::ANJVRVisualization() {
    
}
void ANJVRVisualization::CreateNodos() {
    FXmlNode * rootnode = XmlSource.GetRootNode();
    if (GEngine) {
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, rootnode->GetTag());
    }
    TArray<FXmlNode *> XMLnodes = rootnode->GetChildrenNodes();
    TArray<FXmlNode*> XMLvertexs;
    //for(FXmlNode* nodo : nodos){
        //if (nodo->GetTag() == "vertex") {
            //vertexs.Add(nodo);
        //}
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
    //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::FromInt(vertexs.Num()));
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
            ANodo * const NodoInstanciado = World->SpawnActor<ANodo>(TipoNodo, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
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
            //Nodos[i]->ContenidoString = ObtenerContenidoNodo(Nodos[i]);
            Nodos[i]->Color = Colores[Nodos[i]->ColorNum];
            Nodos[i]->Actualizar();
        }
    }

}

void ANJVRVisualization::CreateAristas() {//el ultimo nodoe debe tener una arista hacia el que aparece como su padre
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
                //AArista * const AristaInstanciado = World->SpawnActor<AArista>(TipoArista, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);//creo que es mejor si yo hago los calculos de los angulos, para generar el rotator
                AristaInstanciado->Id = count;
                AristaInstanciado->SourceId = padre;
                AristaInstanciado->TargetId = hijo;
                AristaInstanciado->SourceNodo = Nodos[padre];
                AristaInstanciado->TargetNodo = Nodos[hijo];
                AristaInstanciado->Escala = Escala;
                AristaInstanciado->Radio = RadioAristas;
                AristaInstanciado->Actualizar();
                AristaInstanciado->ActualizarCollision();
                //AristaInstanciado->AttachRootComponentToActor(this);
                AristaInstanciado->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);

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
        AristaInstanciado->Actualizar();
        AristaInstanciado->ActualizarCollision();
        //AristaInstanciado->AttachRootComponentToActor(this);
        AristaInstanciado->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);

        Aristas.Add(AristaInstanciado);
        count++;
    }
}

void ANJVRVisualization::AplicarTraslacion(FVector Traslacion) {
    for (int i = 0; i < NodosSeleccionados.Num(); i++) {
        //NodosSeleccionados[i]->AddActorLocalOffset(Traslacion);//no esoty seguro si esto funcone
        NodosSeleccionados[i]->SetActorLocation(NodosSeleccionados[i]->GetActorLocation() + Traslacion);
        //produce errores graciososNodosSeleccionados[i]->SetActorRelativeLocation(NodosSeleccionados[i]->GetActorLocation() + Traslacion);
    }

}

void ANJVRVisualization::AplicarRotacionRelativaANodo(ANodo* NodoReferencia, FVector PuntoReferencia) {
    //debo llenar el vector de seleccionados antes, pero sin el resaltado que hace la tarea seleccion, esete arreglo se mantiene mientras este moviendo la rama, luego la vacion cuadno suelte el trrigger
    PuntoReferencia = GetActorTransform().TransformPosition(PuntoReferencia);
    //UE_LOG(LogClass, Log, TEXT("PuntoReferencai x = %f, y = %f, z = %f"), PuntoReferencia.X, PuntoReferencia.Y, PuntoReferencia.Z);
    ANodo* NodoCentro = NodoReferencia->Parent;
    //este trasnforma debo convertirlo a relativo, seguramente lo estoy recibiendo como worldspace
    //calcular el angulo del vector hacia el punto de referencia, con respecto al vetor hacia el padre, esto deberia ser en en el plano relativo, para trabajr sobre un solo plano
    FVector CentroToNodoReference = NodoReferencia->GetActorTransform().GetLocation() - NodoCentro->GetActorTransform().GetLocation();
    FVector CentroToPointReference = PuntoReferencia - NodoCentro->GetActorTransform().GetLocation();
    //CentroToNodoReference = CentroToNodoReference.GetClampedToSize(1.0f, 1.0f);//en teoria no necesito esto
    //CentroToPointReference = CentroToPointReference.GetClampedToSize(1.0f, 1.0f);
    //calcular el angulo que se agreaga
    float DeltaAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(CentroToPointReference.GetClampedToSize(1.0f, 1.0f), CentroToNodoReference.GetClampedToSize(1.0f, 1.0f))));
    float DeltaSing = FVector::CrossProduct(CentroToPointReference.GetClampedToSize(1.0f, 1.0f), CentroToNodoReference.GetClampedToSize(1.0f, 1.0f)).X;//esto es por que el signo es impotante para saber si fue un angulo mayor de 180 o no
    if (DeltaSing >= 0) {
        DeltaAngle = 360-DeltaAngle;
    }
    FRotator VariacionRotation(0.0f, 0.0f, DeltaAngle);//esta es la varaicion en teoria, ahora debo agregarla a los demas, no se si va en angle, o en algun otro
    //esta es la variacion respecto al vector del hacia el apdre del nodo centro, esta variacion, es la que se debe aplicar a los otros, 
    for (int i = 0; i < NodosSeleccionados.Num(); i++) {
        FVector CentroToNodo = NodosSeleccionados[i]->GetActorTransform().GetLocation() - NodoCentro->GetActorTransform().GetLocation();
        float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(CentroToNodo.GetClampedToSize(1.0f, 1.0f), FVector::RightVector)));
        float Sing = FVector::CrossProduct(CentroToNodo.GetClampedToSize(1.0f, 1.0f), FVector::RightVector).X;//esto es por que el signo es impotante para saber si fue un angulo mayor de 180 o no
        if (Sing >= 0) {
            Angle = 360 - Angle;
        }
        float NewAngle = (Angle + DeltaAngle);
        if (NewAngle >= 360) {
            NewAngle -= 360;
        }
        //UE_LOG(LogClass, Log, TEXT("DeltaAngle = %f, Angle = %f, NewAngle = %f"), DeltaAngle, Angle, NewAngle);
        FVector NewRelativeLocation(0.0f, CentroToNodo.Size()*FMath::Cos(FMath::DegreesToRadians(NewAngle)), CentroToNodo.Size()*FMath::Sin(FMath::DegreesToRadians(NewAngle)));
        //UE_LOG(LogClass, Log, TEXT("NewRealtiveLocation x = %f, y = %f, z = %f"), NewRelativeLocation.X, NewRelativeLocation.Y, NewRelativeLocation.Z);
        FTransform NewTransform = NodosSeleccionados[i]->GetActorTransform();
        //UE_LOG(LogClass, Log, TEXT("oldLocation x = %f, y = %f, z = %f"), NewTransform.GetLocation().X, NewTransform.GetLocation().Y, NewTransform.GetLocation().Z);
        NewTransform.SetLocation(NodoCentro->GetActorTransform().GetLocation() + NewRelativeLocation);
        //UE_LOG(LogClass, Log, TEXT("newLocation x = %f, y = %f, z = %f"), NewTransform.GetLocation().X, NewTransform.GetLocation().Y, NewTransform.GetLocation().Z);
        NodosSeleccionados[i]->SetActorTransform(NewTransform);
        //NodosSeleccionados[i]->SetActorTransform(NodosSeleccionados[i]->GetActorTransform().SetLocation(NodoCentro->GetActorTransform().GetLocation() + NewRelativeLocation));
    }

}

FVector ANJVRVisualization::InterseccionLineaSuperficie() {//retorna en espacio local
    //solo si esta frente al controlo, no por detras, corregir esto
    FVector Punto = RightController->GetComponentTransform().GetLocation();
    Punto = GetTransform().InverseTransformPosition(Punto);
    FVector Vector = RightController->GetForwardVector();
    Vector = GetTransform().InverseTransformVector(Vector);

    if (Vector.X == 0) {
        return FVector(-1.0f);
    }
    float t = -Punto.X / Vector.X;
    if (t >= 0) {
        return FVector (0.0f, Punto.Y + t*Vector.Y, Punto.Z + t*Vector.Z);
    }
    return FVector(-1.0f);
}

void ANJVRVisualization::TraslacionConNodoGuia() {
    ImpactPoint = InterseccionLineaSuperficie();
    //UE_LOG(LogClass, Log, TEXT("Impact = %f, %f, %f"), ImpactPoint.X, ImpactPoint.Y, ImpactPoint.Z);
    //if (ImpactPoint != FVector::ZeroVector) {
    if (ImpactPoint.X != -1.0f) {//punto imposible para la el layout
        AplicarTraslacion(ImpactPoint - NodoGuia->GetActorLocation());
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

void ANJVRVisualization::RotacionRama() {
    ImpactPoint = InterseccionLineaSuperficie();
    //UE_LOG(LogClass, Log, TEXT("Impact = %f, %f, %f"), ImpactPoint.X, ImpactPoint.Y, ImpactPoint.Z);
    //if (ImpactPoint != FVector::ZeroVector) {
    if (ImpactPoint.X != -1.0f) {//punto imposible para la el layout
        //AplicarTraslacion(ImpactPoint - NodoGuia->GetActorLocation());
        AplicarRotacionRelativaANodo(NodoGuia, ImpactPoint);
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
