// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayAbilityTargetActor.h"
#include "GameplayAbilityWorldReticle.h"
#include "GameplayAbilityTargetActor_Trace.generated.h"

/** Intermediate base class for all line-trace type targeting actors. */
UCLASS(Abstract, Blueprintable, notplaceable)
class GAMEPLAYABILITIES_API AGameplayAbilityTargetActor_Trace : public AGameplayAbilityTargetActor
{
	GENERATED_UCLASS_BODY()

public:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual FGameplayAbilityTargetDataHandle StaticGetTargetData(UWorld * World, const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo ActivationInfo) const override;

	void AimWithPlayerController(AActor* InSourceActor, FCollisionQueryParams Params, FVector TraceStart, FVector& TraceEnd) const;

	static bool ClipCameraRayToAbilityRange(FVector CameraLocation, FVector CameraDirection, FVector AbilityCenter, float AbilityRange, FVector& ClippedPosition);

	virtual void StartTargeting(UGameplayAbility* Ability) override;

	virtual void ConfirmTargetingAndContinue() override;

	virtual void Tick(float DeltaSeconds) override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = Projectile)
	float MaxRange;

protected:
	virtual FHitResult PerformTrace(AActor* InSourceActor) const PURE_VIRTUAL(AGameplayAbilityTargetActor_Trace, return FHitResult(););

	FGameplayAbilityTargetDataHandle MakeTargetData(FHitResult HitResult) const;

	TWeakObjectPtr<AGameplayAbilityWorldReticle> ReticleActor;
};