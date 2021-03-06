// ShootThemUp Game

#pragma once

#include "CoreMinimal.h"

#include "STUBaseWeapon.h"
#include "Components/ActorComponent.h"
#include "STUCoreTypes.h"
#include "STUWeaponrComponent.generated.h"


class ASTUBaseWeapon;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTTHEMUP_API USTUWeaponrComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USTUWeaponrComponent();

        void StartFire();
        void StopFire();
        void NextWeapon();
        void Reload();

        bool GetCurrentWeaponUIData(FWeaponUIData& UIData) const;
        bool GetCurrentWeaponAmmoData(FAmmoData& AmmoData) const;

        bool TryToAddAmmo(TSubclassOf<ASTUBaseWeapon> WeaponType, int32 ClipsAmount);
protected:

        UPROPERTY(EditDefaultsOnly, Category="Weapon")
        TArray<FWeaponData> WeaponData;
	
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	FName WeaponEquipSocketName = "WeaponSocket";

        UPROPERTY(EditDefaultsOnly, Category="Weapon")
        FName WeaponArmorySocketName = "ArmorySocket";

        UPROPERTY(EditDefaultsOnly, Category="Animation")
        UAnimMontage* EquipAnimMontage;
        
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
        UPROPERTY()
        ASTUBaseWeapon* CurrentWeapon = nullptr;

        UPROPERTY()
        TArray<ASTUBaseWeapon*> Weapons;

        UPROPERTY()
        UAnimMontage* CurrentReloadAnimMontage = nullptr;
    
        int32 CurrentWeaponIndex = 0;

        bool EquipAnimInProgress = false; 
        bool ReloadAnimInProgress = false; 
        void SpawnWeapons();
        void AttachToWeaponToSocket(ASTUBaseWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName);
        void EquipWeapon(int32 WeaponIndex);

        void PlayAnimMontage(UAnimMontage* Animation);
        void InitAnimations();
        void OnEquipFinished(USkeletalMeshComponent* MeshComponent);
        void OnReloadFinished(USkeletalMeshComponent* MeshComponent);

        bool CanFire() const;
        bool CanEquip() const;
        bool CanReload() const;

        void OnEmptyClip(ASTUBaseWeapon* AmmoEmptyWeapon);
        void ChangeClip();

    
};
