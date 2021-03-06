// ShootThemUp Game


#include "Weapon/STULauncherWeapon.h"
#include "Weapon/STUProjectile.h"

void ASTULauncherWeapon::StartFire() {
    MakeShot();
}

void ASTULauncherWeapon::MakeShot() {
    if (!GetWorld() || IsAmmoEmpty()) {
        StopFire();
        return;
    };

    FVector TraceStart, TraceEnd;
    if (!GetTraceData(TraceStart, TraceEnd)) {
        StopFire();
        return;
    };

    FHitResult HitResult;
    MakeHit(HitResult, TraceStart, TraceEnd);

    const FVector EndPoint = HitResult.bBlockingHit ? HitResult.ImpactPoint : TraceEnd;
    const FVector Direction = (EndPoint - GetMuzzleWorldLocation()).GetSafeNormal();
    
    const FTransform SpawnTransform(FRotator::ZeroRotator, GetMuzzleWorldLocation());
    ASTUProjectile* Projectile = GetWorld()->SpawnActorDeferred<ASTUProjectile>(ProjectileClass, SpawnTransform);

    if (Projectile) {
        Projectile->SetShootDirection(Direction);
        Projectile->SetOwner(GetOwner());
        Projectile->FinishSpawning(SpawnTransform);
    }

    DecreaseAmmo();
}
