// ShootThemUp Game


#include "Pickups/STUAmmoPickup.h"
#include "Components/STUHealthComponent.h"
#include "Components/STUWeaponrComponent.h"
#include "STUUtils.h"

DEFINE_LOG_CATEGORY_STATIC(LogAmmoPickup, All, All);

bool ASTUAmmoPickup::GivePickupTo(APawn *PlayerPawn) {
    const auto HealthComponent = STUUTils::GetSTUPlayerComponent<USTUHealthComponent>(PlayerPawn);
    if(!HealthComponent || HealthComponent->IsDead()) return false;

    const auto WeaponComponent = STUUTils::GetSTUPlayerComponent<USTUWeaponrComponent>(PlayerPawn);
    if(!WeaponComponent) return false;
    
    return WeaponComponent->TryToAddAmmo(WeaponType, ClipsAmount);
}
