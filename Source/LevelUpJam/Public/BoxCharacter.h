// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "BoxCharacter.generated.h"

// Forward declarations
class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;
class ARespawnPoint;

UCLASS(BlueprintType, Blueprintable)
class LEVELUPJAM_API ABoxCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABoxCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Camera Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* CameraComponent;

	// Enhanced Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveForwardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveBackwardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* JumpAction;

	// Input Functions
	void MoveLeftRight(const FInputActionValue& Value);
	void MoveForward(const FInputActionValue& Value);
	void MoveBackward(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	// Health system
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	int32 Health = 100;

	// Called when health reaches zero
	UFUNCTION(BlueprintNativeEvent, Category = "Health")
	void OnDeath();
	virtual void OnDeath_Implementation();

	// Blueprint event for custom death logic (respawn, effects, etc.)
	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
	void BP_OnDeath();

	// Last respawn point the player touched
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Respawn")
	ARespawnPoint* CurrentRespawnPoint;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Returns current health
	UFUNCTION(BlueprintPure, Category = "Health")
	int32 GetHealth() const { return Health; }

	// Called when the character takes damage
	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual void ReceiveDamage(int32 DamageAmount);

	// Set the current respawn point
	UFUNCTION(BlueprintCallable, Category = "Respawn")
	void SetRespawnPoint(ARespawnPoint* NewRespawnPoint) { CurrentRespawnPoint = NewRespawnPoint; }
	// Get the current respawn point
	UFUNCTION(BlueprintPure, Category = "Respawn")
	ARespawnPoint* GetRespawnPoint() const { return CurrentRespawnPoint; }

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
