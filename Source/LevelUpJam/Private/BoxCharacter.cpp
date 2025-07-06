// Fill out your copyright notice in the Description page of Project Settings.

#include "BoxCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

// Sets default values
ABoxCharacter::ABoxCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Spring Arm Component
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 600.0f;
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->bInheritPitch = true;
	SpringArmComponent->bInheritYaw = true;
	SpringArmComponent->bInheritRoll = false;

	// Create Camera Component
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);

	// Configure Character Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Don't rotate when the controller rotates. Let the camera follow the controller
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

// Called when the game starts or when spawned
void ABoxCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

// Called every frame
void ABoxCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABoxCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Cast to Enhanced Input Component
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Bind Move Action (for left/right with Scale modifier)
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABoxCharacter::MoveLeftRight);

		// Bind individual Forward/Back actions (Boolean inputs)
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &ABoxCharacter::MoveForward);
		EnhancedInputComponent->BindAction(MoveBackwardAction, ETriggerEvent::Triggered, this, &ABoxCharacter::MoveBackward);

		// Bind Look Action
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABoxCharacter::Look);

		// Bind Jump Action
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	}
}

void ABoxCharacter::MoveLeftRight(const FInputActionValue& Value)
{
	// Get movement vector for left/right movement only
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Get camera-relative right direction
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Add left/right movement (only X component since this is A/D keys)
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABoxCharacter::Look(const FInputActionValue& Value)
{
	// Get look axis vector
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(-LookAxisVector.Y);  // Negative Y to invert up/down
	}
}

void ABoxCharacter::MoveForward(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		// Get forward direction based on camera
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		
		// Add forward movement
		AddMovementInput(ForwardDirection, 1.0f);
	}
}

void ABoxCharacter::MoveBackward(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		// Get forward direction based on camera
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		
		// Add backward movement (negative forward)
		AddMovementInput(ForwardDirection, -1.0f);
	}
}

