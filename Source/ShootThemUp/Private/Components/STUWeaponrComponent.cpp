// ShootThemUp Game


#include "Components/STUWeaponrComponent.h"
#include "Weapon/STUBaseWeapon.h"
#include "GameFramework/Character.h"
#include "Animations/STUEquipFinishedAnimNotify.h"
#include "Animations/STU_ReloadFinishedAnimNotify.h"
#include "Animations/AnimUtils.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeaponComponent, All, All)

USTUWeaponrComponent::USTUWeaponrComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

constexpr static int32 WeaponNum = 2;

void USTUWeaponrComponent::BeginPlay()
{
    Super::BeginPlay();

    checkf(WeaponData.Num() == WeaponNum, TEXT("Our character can hold only %i weapon items"), WeaponNum);
    
    CurrentWeaponIndex = 0;
    InitAnimations();
    SpawnWeapons();
    EquipWeapon(CurrentWeaponIndex);
}

void USTUWeaponrComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
    CurrentWeapon = nullptr;

    for(auto Weapon: Weapons) {
        Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        Weapon->Destroy();
    }

    Weapons.Empty();
    Super::EndPlay(EndPlayReason);
}

void USTUWeaponrComponent::SpawnWeapons() {
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character || !GetWorld()) return;

    for (auto OneWeaponData: WeaponData) {
        auto Weapon = GetWorld()->SpawnActor<ASTUBaseWeapon>(OneWeaponData.WeaponClass);
        if (!Weapon) continue;

        Weapon->OnClipEmpty.AddUObject(this, &USTUWeaponrComponent::OnEmptyClip);
        Weapon->SetOwner(Character);
        Weapons.Add(Weapon);

        AttachToWeaponToSocket(Weapon, Character->GetMesh(), WeaponArmorySocketName);
    }
        
}

void USTUWeaponrComponent::AttachToWeaponToSocket(ASTUBaseWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName) {
    if (!Weapon || !SceneComponent) return;
    
    FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
    Weapon->AttachToComponent(SceneComponent, AttachmentRules, SocketName);
}

void USTUWeaponrComponent::EquipWeapon(int32 WeaponIndex) {
    if (WeaponIndex < 0 || WeaponIndex >= Weapons.Num()) {
        UE_LOG(LogWeaponComponent, Warning, TEXT("Invalid weapon index"));
        return;
    }
    
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character) return;

    if (CurrentWeapon) {
        CurrentWeapon->StopFire();
        AttachToWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponArmorySocketName);
    }
    
    CurrentWeapon = Weapons[WeaponIndex];
    // CurrentReloadAnimMontage = WeaponData[WeaponIndex].ReloadAnimMontage;
    const auto CurrentWeaponData = WeaponData.FindByPredicate([&](const FWeaponData& Data) { //
            return Data.WeaponClass == CurrentWeapon->GetClass();
    });
    CurrentReloadAnimMontage = CurrentWeaponData ? CurrentWeaponData->ReloadAnimMontage : nullptr;
    
    AttachToWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponEquipSocketName);

    EquipAnimInProgress = true;
    PlayAnimMontage(EquipAnimMontage);
}

void USTUWeaponrComponent::StartFire() {
    if (!CanFire()) return;

    CurrentWeapon->StartFire();
}

void USTUWeaponrComponent::StopFire() {
    if(!CurrentWeapon) return;

    CurrentWeapon->StopFire();
}

void USTUWeaponrComponent::NextWeapon() {
    if (!CanEquip()) return;
    
    CurrentWeaponIndex = (CurrentWeaponIndex + 1) % Weapons.Num();
    EquipWeapon(CurrentWeaponIndex);
}

void USTUWeaponrComponent::Reload() {
    ChangeClip();
}

void USTUWeaponrComponent::PlayAnimMontage(UAnimMontage* Animation) {
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character) return;

    Character->PlayAnimMontage(Animation);
}

void USTUWeaponrComponent::InitAnimations() {
    auto EquipFinishedNotify = AnimUtils::FindNotifyByClass<USTUEquipFinishedAnimNotify>(EquipAnimMontage);
    if (EquipFinishedNotify) {
        EquipFinishedNotify->OnNotified.AddUObject(this, &USTUWeaponrComponent::OnEquipFinished);
    } else {
        UE_LOG(LogWeaponComponent, Error, TEXT("Equip anim notify is forgotten to set"));
        checkNoEntry();
    }

    for(auto OneWeaponData : WeaponData) {
        auto ReloadFinishedNotify = AnimUtils::FindNotifyByClass<USTU_ReloadFinishedAnimNotify>(OneWeaponData.ReloadAnimMontage);
        if (!ReloadFinishedNotify) {
            UE_LOG(LogWeaponComponent, Error, TEXT("Reload anim notify is forgotten to set"));
            checkNoEntry();
        }
        
        ReloadFinishedNotify->OnNotified.AddUObject(this, &USTUWeaponrComponent::OnReloadFinished);
    }
}

bool USTUWeaponrComponent::GetCurrentWeaponUIData(FWeaponUIData& UIData) const {
    if(CurrentWeapon) {
        UIData = CurrentWeapon->GetUIData();
        return true;
    }

    return false;
}

bool USTUWeaponrComponent::GetCurrentWeaponAmmoData(FAmmoData& AmmoData) const {
    if(CurrentWeapon) {
        AmmoData = CurrentWeapon->GetAmmoData();
        return true;
    }

    return false;
}

bool USTUWeaponrComponent::TryToAddAmmo(TSubclassOf<ASTUBaseWeapon> WeaponType, int32 ClipsAmount) {
    for(const auto Weapon : Weapons) {
        if(Weapon && Weapon->IsA(WeaponType)) {
            return Weapon->TryToAddAmmo(ClipsAmount);
        }
    }
    return false;
}

void USTUWeaponrComponent::OnEquipFinished(USkeletalMeshComponent* MeshComponent) {
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character || Character->GetMesh() != MeshComponent) return;

    EquipAnimInProgress = false;

}

void USTUWeaponrComponent::OnReloadFinished(USkeletalMeshComponent* MeshComponent) {
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character || Character->GetMesh() != MeshComponent) return;

    ReloadAnimInProgress = false;
}

bool USTUWeaponrComponent::CanFire() const{
    return CurrentWeapon && !EquipAnimInProgress && !ReloadAnimInProgress;
}

bool USTUWeaponrComponent::CanEquip() const {
    return !EquipAnimInProgress && !ReloadAnimInProgress;
}

bool USTUWeaponrComponent::CanReload() const {
    return CurrentWeapon &&
           !EquipAnimInProgress &&
           !ReloadAnimInProgress &&
           CurrentWeapon->CanReload();
}

void USTUWeaponrComponent::OnEmptyClip(ASTUBaseWeapon* AmmoEmptyWeapon) {
    if (!AmmoEmptyWeapon) return;
    
    if(CurrentWeapon == AmmoEmptyWeapon) {
        ChangeClip();
    } else {
        for (const auto Weapon: Weapons) {
            if(Weapon == AmmoEmptyWeapon) {
                Weapon->ChangeClip();
            }
        }
    }
}

void USTUWeaponrComponent::ChangeClip() {
    if (!CanReload()) return;
    CurrentWeapon->StopFire();
    CurrentWeapon->ChangeClip();
    
    ReloadAnimInProgress = true;
    PlayAnimMontage(CurrentReloadAnimMontage);
}