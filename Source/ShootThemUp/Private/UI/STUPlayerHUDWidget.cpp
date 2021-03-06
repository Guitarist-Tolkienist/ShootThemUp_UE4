// ShootThemUp Game


#include "UI/STUPlayerHUDWidget.h"
#include "Components/STUHealthComponent.h"
#include "Components/STUWeaponrComponent.h"
#include "STUUtils.h"

float USTUPlayerHUDWidget::GetHealthPercent() const {
    const auto HealthComponent = STUUTils::GetSTUPlayerComponent<USTUHealthComponent>(GetOwningPlayerPawn());
    if(!HealthComponent) return 0.0f;
    
    return HealthComponent->GetHealthPercent();
}

bool USTUPlayerHUDWidget::GetCurrentWeaponUIData(FWeaponUIData& UIData) const {
    const auto WeaponComponent = STUUTils::GetSTUPlayerComponent<USTUWeaponrComponent>(GetOwningPlayerPawn());
    if(!WeaponComponent) return false;
        
    return WeaponComponent->GetCurrentWeaponUIData(UIData);
}

bool USTUPlayerHUDWidget::GetCurrentWeaponAmmoData(FAmmoData& AmmoData) const {
    const auto WeaponComponent = STUUTils::GetSTUPlayerComponent<USTUWeaponrComponent>(GetOwningPlayerPawn());
    if(!WeaponComponent) return false;
    
    return WeaponComponent->GetCurrentWeaponAmmoData(AmmoData);
}

bool USTUPlayerHUDWidget::IsPlayerAlive() const {
    const auto HealthComponent = STUUTils::GetSTUPlayerComponent<USTUHealthComponent>(GetOwningPlayerPawn());

    return HealthComponent && !HealthComponent->IsDead();
}

bool USTUPlayerHUDWidget::IsPLayerSpectating() {
    const auto Controller = GetOwningPlayer();
    return Controller && Controller->GetStateName() == NAME_Spectating;
}
