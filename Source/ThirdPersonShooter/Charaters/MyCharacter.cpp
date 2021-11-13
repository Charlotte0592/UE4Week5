// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimMontage.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../System/AmmoSystem.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 注册摄像机手臂组件
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	// 把这个组件绑定到根组件
	SpringArm->SetupAttachment(RootComponent);
	// 设置摄像机手臂和根组件之间的距离
	SpringArm->TargetArmLength = 300.0f;
	// 我们使用模型组件去进行旋转,如果不设置设个的话,Pitch轴无法进行视角移动
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = true;

	// 注册摄像机组件
	TpsCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamera"));
	// 把摄像机绑定到摄像机手臂上
	TpsCamera->SetupAttachment(SpringArm);

	GunWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunWeapon"));
	GunWeapon->SetupAttachment(GetMesh(), TEXT("Weapon"));

	AmmoSystem = CreateDefaultSubobject<UAmmoSystem>(TEXT("AmmoSystem"));
	AmmoSystem->CurrentAmmo = 30;
	AmmoSystem->MaxAmmo = 30;
	AmmoSystem->AmmoInventory = 120;

	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	this->bUseControllerRotationYaw = false;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->MaxWalkSpeed = 270;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 170;

}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	bIsHip = true;
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bIsHip)
	{
		/* 射击第一种解决方案
		FVector FireStart = TpsCamera->GetComponentLocation();
		FVector FireEnd = TpsCamera->GetForwardVector() * 5000 + FireStart;

		TArray<AActor*> ActorToIngore;
		UKismetSystemLibrary::LineTraceSingle(this, FireStart, FireEnd, ETraceTypeQuery::TraceTypeQuery2, false, ActorToIngore, EDrawDebugTrace::ForOneFrame, OutHit, true);
		*/
		// 第二种
		FVector FireStart = GunWeapon->GetSocketLocation(TEXT("Muzzle"));
		FVector FireEnd = TpsCamera->GetForwardVector() * 5000 + FireStart;

		TArray<AActor*> ActorToIngore;
		UKismetSystemLibrary::LineTraceSingle(this, FireStart, FireEnd, ETraceTypeQuery::TraceTypeQuery2, false, ActorToIngore, EDrawDebugTrace::ForOneFrame, OutHit, true);
	}
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AMyCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AMyCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("TurnRate"), this, &AMyCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &AMyCharacter::CrouchDown);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Released, this, &AMyCharacter::CrouchUp);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AMyCharacter::JumpStart);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &AMyCharacter::JumpEnd);
	PlayerInputComponent->BindAction(TEXT("Ironsight"), IE_Pressed, this, &AMyCharacter::IronsightDown);
	PlayerInputComponent->BindAction(TEXT("Ironsight"), IE_Released, this, &AMyCharacter::IronsightUp);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &AMyCharacter::FireDown);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &AMyCharacter::FireUp);
	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &AMyCharacter::Reload);

}

float AMyCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, FString::SanitizeFloat(DamageAmount));
	if (DamageAmount >= CurrentHealth)
	{
		IsDead = true;
	}
	else
	{
		IsDead = false;
	}
	CurrentHealth -= DamageAmount;
	return 0.0f;
}

void AMyCharacter::MoveForward(float value)
{
	if (!IsDead)
	{
		AddMovementInput(UKismetMathLibrary::GetForwardVector(GetControlRotation()), value);

	}
}

void AMyCharacter::MoveRight(float value)
{
	if (!IsDead)
	{
		AddMovementInput(UKismetMathLibrary::GetRightVector(GetControlRotation()), value);
	}
}

void AMyCharacter::CrouchDown()
{
	if (!IsDead)
	{
		Crouch();
	}
}

void AMyCharacter::CrouchUp()
{
	if (!IsDead)
	{
		UnCrouch();
	}
}

void AMyCharacter::JumpStart()
{
	if (!IsDead)
	{
		bPressedJump = true;
	}
}

void AMyCharacter::JumpEnd()
{
	if (!IsDead)
	{
		bPressedJump = false;
	}
}

void AMyCharacter::IronsightDown()
{
	if (!IsDead)
	{
		bIsHip = false;
		// GetCharacterMovement()->bOrientRotationToMovement = false;
		// GetCharacterMovement()->bUseControllerDesiredRotation = true;
		SpringArm->SetRelativeLocation(FVector(10, 100, 70));
		SpringArm->TargetArmLength = 200;
	}
}

void AMyCharacter::IronsightUp()
{
	if (!IsDead)
	{
		bIsHip = true;
		// GetCharacterMovement()->bOrientRotationToMovement = true;
		// GetCharacterMovement()->bUseControllerDesiredRotation = false;
		SpringArm->SetRelativeLocation(FVector(10, 10, 70));
		SpringArm->TargetArmLength = 300;
	}
	OutHit.Init();
}

void AMyCharacter::FireDown()
{
	if (!IsDead)
	{
		GetWorld()->GetTimerManager().SetTimer(FireTimer, this, &AMyCharacter::Fire, 0.1f, true, 0.f);
		// Fire();
	}
}

void AMyCharacter::FireUp()
{
	GetWorld()->GetTimerManager().ClearTimer(FireTimer);
}

void AMyCharacter::Fire()
{
	if (!IsDead)
	{
		if (!bIsHip)
		{
			if (AmmoSystem->CurrentAmmo > 0)
			{
				PlayAnimMontage(FireMontage);
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
				AmmoSystem->CurrentAmmo -= 1;

			}
			else
			{
				// No bullet
			}
			/*
			FVector FireStart = GunWeapon->GetSocketLocation(TEXT("Muzzle"));
			FVector FireEnd = TpsCamera->GetForwardVector() * 5000 + FireStart;
			FHitResult OutHit;
			TArray<AActor*> ActorToIngore;
			bool IsHit = UKismetSystemLibrary::LineTraceSingle(this, FireStart, FireEnd, ETraceTypeQuery::TraceTypeQuery2, false, ActorToIngore, EDrawDebugTrace::ForDuration, OutHit, true);

			if (IsHit)
			{
				UGameplayStatics::ApplyDamage(OutHit.GetActor(), 10.f, nullptr, this, nullptr);
			}
			*/
			if (UKismetSystemLibrary::IsValid(OutHit.GetActor()))
			{
				UGameplayStatics::ApplyDamage(OutHit.GetActor(), 10.0f, nullptr, this, nullptr);
			}
		}
	}

}

void AMyCharacter::Reload()
{
	if (AmmoSystem->CurrentAmmo < AmmoSystem->MaxAmmo)
	{
		if (bIsHip)
		{
			PlayAnimMontage(ReloadMontage_Hip);
		} 
		else
		{
			PlayAnimMontage(ReloadMontage_Aim);
		}
	}
}
