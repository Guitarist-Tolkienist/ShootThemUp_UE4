// ShootThemUp Game


#include "Animations/STU_AnimNotify.h"


void USTU_AnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) {
    OnNotified.Broadcast(MeshComp);
    Super::Notify(MeshComp, Animation);
}