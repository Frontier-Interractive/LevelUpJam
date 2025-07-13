// Fill out your copyright notice in the Description page of Project Settings.

#include "BoxCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "RespawnPoint.h"
#include "EngineUtils.h"

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

	// Initialize Health
	Health = 100.0f;
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

	// If no respawn point is set, search for one with RespawnID "Start"
	if (!CurrentRespawnPoint)
	{
		for (TActorIterator<ARespawnPoint> It(GetWorld()); It; ++It)
		{
			if (It->RespawnID == "Start")
			{
				CurrentRespawnPoint = *It;
				break;
			}
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

// Called when the character takes damage
void ABoxCharacter::ReceiveDamage(int32 DamageAmount)
{
	if (Health <= 0) return;

	Health -= DamageAmount;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("BoxCharacter Health: %d"), Health));
	}

	if (Health <= 0)
	{
		Health = 0;
		OnDeath();
	}
}

void ABoxCharacter::OnDeath_Implementation()
{
	// Core C++ death logic: disable input, destroy actor, etc.
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}
	// Example: destroy the actor after a short delay
	SetLifeSpan(2.0f);

	// Call Blueprint event for respawn and effects
	BP_OnDeath();
}

void ABoxCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    if (ARespawnPoint* Respawn = Cast<ARespawnPoint>(OtherActor))
    {
        FName NewID = Respawn->RespawnID;
        FName CurrentID = CurrentRespawnPoint ? CurrentRespawnPoint->RespawnID : NAME_None;
        if (NewID != CurrentID)
        {
            SetRespawnPoint(Respawn);
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Checkpoint reached: %s"), *NewID.ToString()));
            }
            // Call Blueprint logic for checkpoint update
            BP_OnDeath(); // Or your BP respawn/update function
        }
    }
}


