// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

UCLASS()
class THIRDPERSONSHOOTER_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// 摄像机弹簧臂组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class USpringArmComponent * SpringArm;
	// 摄像机组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UCameraComponent * TpsCamera;
	// 枪组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class USkeletalMeshComponent * GunWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UAmmoSystem * AmmoSystem;

	// 人物状态
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bIsHip;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bReloading = false;

	UPROPERTY(EditDefaultsOnly)
		class UAnimMontage * FireMontage;
	UPROPERTY(EditDefaultsOnly)
		class UAnimMontage * ReloadMontage_Hip;
	UPROPERTY(EditDefaultsOnly)
		class UAnimMontage * ReloadMontage_Aim;

	UPROPERTY(EditDefaultsOnly)
		class USoundBase * FireSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CurrentHealth = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxHealth = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsDead = false;

	void MoveForward(float value);
	void MoveRight(float value);

	void CrouchDown();
	void CrouchUp();

	void JumpStart();
	void JumpEnd();

	void IronsightDown();
	void IronsightUp();

	void FireDown();
	void FireUp();
	UFUNCTION(BlueprintCallable)
		void Fire();

	UFUNCTION(BlueprintCallable)
		void Reload();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FHitResult OutHit;
	
	FTimerHandle FireTimer;
};
