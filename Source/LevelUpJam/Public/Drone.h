// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Engine/TargetPoint.h"
#include "Drone.generated.h"

UENUM(BlueprintType)
enum class EDroneState : uint8
{
	Patrolling		UMETA(DisplayName = "Patrolling"),
	Chasing			UMETA(DisplayName = "Chasing"),
	Carrying		UMETA(DisplayName = "Carrying"),
	Returning		UMETA(DisplayName = "Returning")
};

UCLASS()
class LEVELUPJAM_API ADrone : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADrone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* DroneMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* DetectionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* InteractionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UFloatingPawnMovement* FloatingMovement;

	// Patrol System
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	TArray<ATargetPoint*> PatrolPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	float PatrolSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	float ChaseSpeed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	float PatrolWaitTime = 2.0f;

	// Detection System
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
	float DetectionRadius = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
	float InteractionRadius = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
	float SightAngle = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
	float LosePlayerTime = 2.0f;

	// Drop Off System
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropOff")
	ATargetPoint* DropOffPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropOff")
	float DropOffHeight = 100.0f;

	// State Management
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	EDroneState CurrentState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	class ABoxCharacter* DetectedPlayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	class ABoxCharacter* CarriedPlayer;

private:
	// Internal state variables
	int32 CurrentPatrolIndex;
	FVector CurrentTarget;
	FTimerHandle PatrolTimer;
	FTimerHandle LosePlayerTimer;
	bool bIsWaitingAtPatrol;
	bool bPlayerInSight;

	// Movement functions
	void MoveToLocation(const FVector& Location, float Speed);
	void SetNewPatrolTarget();
	void StartPatrolWait();
	void EndPatrolWait();

	// Detection functions
	bool CanSeePlayer(class ABoxCharacter* Player);
	bool IsPlayerInSightCone(class ABoxCharacter* Player);
	void StartChasing(class ABoxCharacter* Player);
	void LosePlayer();
	void StartLosePlayerTimer();
	void ClearLosePlayerTimer();

	// Player interaction
	void GrabPlayer(class ABoxCharacter* Player);
	void DropPlayer();
	void CarryPlayerToDropOff();

	// State management
	void ChangeState(EDroneState NewState);
	void UpdateCurrentState();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Detection sphere events
	UFUNCTION()
	void OnDetectionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDetectionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Interaction sphere events
	UFUNCTION()
	void OnInteractionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Blueprint callable functions
	UFUNCTION(BlueprintCallable, Category = "Drone")
	void SetPatrolPoints(const TArray<ATargetPoint*>& NewPatrolPoints);

	UFUNCTION(BlueprintCallable, Category = "Drone")
	void SetDropOffPoint(ATargetPoint* NewDropOffPoint);

	UFUNCTION(BlueprintCallable, Category = "Drone")
	void ForceEndChase();

	UFUNCTION(BlueprintPure, Category = "Drone")
	EDroneState GetCurrentState() const { return CurrentState; }

	UFUNCTION(BlueprintPure, Category = "Drone")
	bool HasDetectedPlayer() const { return DetectedPlayer != nullptr; }

	// Used by SafeZoneTrigger to know if player is in safe zone
	bool bSafeZoneActive = false;
};
