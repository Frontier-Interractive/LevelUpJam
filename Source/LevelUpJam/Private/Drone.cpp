// Fill out your copyright notice in the Description page of Project Settings.

#include "Drone.h"
#include "BoxCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Engine/TargetPoint.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Engine.h"

// Sets default values
ADrone::ADrone()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Root Scene Component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Create and setup Drone Mesh
	DroneMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DroneMesh"));
	DroneMesh->SetupAttachment(RootComponent);

	// Create and setup Detection Sphere
	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->SetupAttachment(RootComponent);
	DetectionSphere->SetSphereRadius(DetectionRadius);
	DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// Create and setup Interaction Sphere
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->SetSphereRadius(InteractionRadius);
	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// Create and setup Floating Movement Component
	FloatingMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingMovement"));
	FloatingMovement->MaxSpeed = PatrolSpeed;
	FloatingMovement->Acceleration = 1000.0f;
	FloatingMovement->Deceleration = 1000.0f;

	// Initialize state
	CurrentState = EDroneState::Patrolling;
	CurrentPatrolIndex = 0;
	DetectedPlayer = nullptr;
	CarriedPlayer = nullptr;
	bIsWaitingAtPatrol = false;
	bPlayerInSight = false;

	// Bind overlap events
	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &ADrone::OnDetectionSphereBeginOverlap);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &ADrone::OnDetectionSphereEndOverlap);
	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &ADrone::OnInteractionSphereBeginOverlap);
}

// Called when the game starts or when spawned
void ADrone::BeginPlay()
{
	Super::BeginPlay();
	
	// Update sphere radii based on configured values
	DetectionSphere->SetSphereRadius(DetectionRadius);
	InteractionSphere->SetSphereRadius(InteractionRadius);

	// Start patrolling if we have patrol points
	if (PatrolPoints.Num() > 0)
	{
		ChangeState(EDroneState::Patrolling);
		SetNewPatrolTarget();
	}
}

// Called every frame
void ADrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update sight detection
	if (DetectedPlayer)
	{
		bPlayerInSight = CanSeePlayer(DetectedPlayer);
	}

	UpdateCurrentState();
}

// Called to bind functionality to input
void ADrone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// State Management
void ADrone::ChangeState(EDroneState NewState)
{
	if (CurrentState != NewState)
	{
		// Exit previous state
		switch (CurrentState)
		{
		case EDroneState::Patrolling:
			GetWorld()->GetTimerManager().ClearTimer(PatrolTimer);
			break;
		case EDroneState::Chasing:
			ClearLosePlayerTimer();
			break;
		}

		CurrentState = NewState;

		// Enter new state
		switch (NewState)
		{
		case EDroneState::Patrolling:
			FloatingMovement->MaxSpeed = PatrolSpeed;
			if (PatrolPoints.Num() > 0)
			{
				SetNewPatrolTarget();
			}
			break;
		case EDroneState::Chasing:
			FloatingMovement->MaxSpeed = ChaseSpeed;
			break;
		case EDroneState::Carrying:
			if (DropOffPoint)
			{
				CarryPlayerToDropOff();
			}
			break;
		}

		// Debug output
		if (GEngine)
		{
			FString StateString = UEnum::GetValueAsString(CurrentState);
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, 
				FString::Printf(TEXT("Drone State: %s"), *StateString));
		}
	}
}

void ADrone::UpdateCurrentState()
{
	switch (CurrentState)
	{
	case EDroneState::Patrolling:
		{
			if (DetectedPlayer && bPlayerInSight && !bSafeZoneActive)
			{
				StartChasing(DetectedPlayer);
			}
			else if (!bIsWaitingAtPatrol && PatrolPoints.Num() > 0)
			{
				// Check if we've reached our current patrol target
				float DistanceToTarget = FVector::Dist(GetActorLocation(), CurrentTarget);
				if (DistanceToTarget < 100.0f)
				{
					StartPatrolWait();
				}
				else
				{
					MoveToLocation(CurrentTarget, PatrolSpeed);
				}
			}
		}
		break;

	case EDroneState::Chasing:
		{
			if (bSafeZoneActive)
			{
				ForceEndChase();
			}
			else if (DetectedPlayer && bPlayerInSight)
			{
				float DistanceToPlayer = FVector::Dist(GetActorLocation(), DetectedPlayer->GetActorLocation());
				if (DistanceToPlayer < InteractionRadius)
				{
					GrabPlayer(DetectedPlayer);
				}
				else
				{
					MoveToLocation(DetectedPlayer->GetActorLocation(), ChaseSpeed);
				}
			}
			else
			{
				// Lost sight of player, start timer to lose them
				if (!GetWorld()->GetTimerManager().IsTimerActive(LosePlayerTimer))
				{
					StartLosePlayerTimer();
				}
			}
		}
		break;

	case EDroneState::Carrying:
		{
			if (DropOffPoint && CarriedPlayer)
			{
				float DistanceToDropOff = FVector::Dist(GetActorLocation(), DropOffPoint->GetActorLocation());
				if (DistanceToDropOff < 200.0f)
				{
					DropPlayer();
				}
				else
				{
					MoveToLocation(DropOffPoint->GetActorLocation(), ChaseSpeed);
				}
			}
		}
		break;

	case EDroneState::Returning:
		{
			if (PatrolPoints.Num() > 0)
			{
				float DistanceToPatrol = FVector::Dist(GetActorLocation(), CurrentTarget);
				if (DistanceToPatrol < 100.0f)
				{
					ChangeState(EDroneState::Patrolling);
				}
				else
				{
					MoveToLocation(CurrentTarget, PatrolSpeed);
				}
			}
		}
		break;
	}
}

// Movement Functions
void ADrone::MoveToLocation(const FVector& Location, float Speed)
{
	if (FloatingMovement)
	{
		FloatingMovement->MaxSpeed = Speed;
		FVector Direction = (Location - GetActorLocation()).GetSafeNormal();
		FloatingMovement->AddInputVector(Direction);

		// Rotate to face movement direction
		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Location);
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), 2.0f));
	}
}

void ADrone::SetNewPatrolTarget()
{
	if (PatrolPoints.Num() > 0)
	{
		CurrentPatrolIndex = (CurrentPatrolIndex + 1) % PatrolPoints.Num();
		if (PatrolPoints[CurrentPatrolIndex])
		{
			CurrentTarget = PatrolPoints[CurrentPatrolIndex]->GetActorLocation();
		}
	}
}

void ADrone::StartPatrolWait()
{
	bIsWaitingAtPatrol = true;
	GetWorld()->GetTimerManager().SetTimer(PatrolTimer, this, &ADrone::EndPatrolWait, PatrolWaitTime, false);
}

void ADrone::EndPatrolWait()
{
	bIsWaitingAtPatrol = false;
	SetNewPatrolTarget();
}

// Detection Functions
bool ADrone::CanSeePlayer(ABoxCharacter* Player)
{
	if (!Player) return false;

	// Check if player is in sight cone
	if (!IsPlayerInSightCone(Player)) return false;

	// Perform line trace to check for obstacles
	FVector Start = GetActorLocation();
	FVector End = Player->GetActorLocation();

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = false;

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams);

	// If we hit something, check if it's the player
	if (bHit)
	{
		return HitResult.GetActor() == Player;
	}

	// No obstacles in the way
	return true;
}

bool ADrone::IsPlayerInSightCone(ABoxCharacter* Player)
{
	if (!Player) return false;

	FVector ToPlayer = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	FVector ForwardVector = GetActorForwardVector();

	float DotProduct = FVector::DotProduct(ForwardVector, ToPlayer);
	float AngleToPlayer = FMath::RadiansToDegrees(FMath::Acos(DotProduct));

	// Use a wider cone (135 deg) during chase, normal otherwise
	float EffectiveAngle = (CurrentState == EDroneState::Chasing) ? 135.0f : SightAngle;
	return AngleToPlayer <= EffectiveAngle / 2.0f;
}

void ADrone::StartChasing(ABoxCharacter* Player)
{
	DetectedPlayer = Player;
	ChangeState(EDroneState::Chasing);
	ClearLosePlayerTimer();
}

void ADrone::LosePlayer()
{
	DetectedPlayer = nullptr;
	bPlayerInSight = false;
	ChangeState(EDroneState::Returning);
}

void ADrone::StartLosePlayerTimer()
{
	GetWorld()->GetTimerManager().SetTimer(LosePlayerTimer, this, &ADrone::LosePlayer, LosePlayerTime, false);
}

void ADrone::ClearLosePlayerTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(LosePlayerTimer);
}

// Player Interaction
void ADrone::GrabPlayer(ABoxCharacter* Player)
{
	if (Player && !CarriedPlayer)
	{
		CarriedPlayer = Player;
		
		// Attach player to drone
		Player->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		
		// Disable player input
		if (APlayerController* PC = Player->GetController<APlayerController>())
		{
			Player->DisableInput(PC);
		}

		ChangeState(EDroneState::Carrying);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Player Captured!"));
		}
	}
}

void ADrone::DropPlayer()
{
	if (CarriedPlayer && DropOffPoint)
	{
		// Calculate drop position at DropOffPoint + DropOffHeight
		FVector DropLocation = DropOffPoint->GetActorLocation() + FVector(0, 0, DropOffHeight);

		// Detach player
		CarriedPlayer->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CarriedPlayer->SetActorLocation(DropLocation);

		// Re-enable player input
		if (APlayerController* PC = CarriedPlayer->GetController<APlayerController>())
		{
			CarriedPlayer->EnableInput(PC);
		}

		CarriedPlayer = nullptr;
		DetectedPlayer = nullptr;
		bPlayerInSight = false;

		ChangeState(EDroneState::Returning);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Player Dropped!"));
		}
	}
}

void ADrone::CarryPlayerToDropOff()
{
	if (DropOffPoint && CarriedPlayer)
	{
		// Move towards drop off point
		MoveToLocation(DropOffPoint->GetActorLocation(), ChaseSpeed);
	}
}

// Overlap Events
void ADrone::OnDetectionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ABoxCharacter* Player = Cast<ABoxCharacter>(OtherActor))
	{
		DetectedPlayer = Player;
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("Player Detected"));
		}
	}
}

void ADrone::OnDetectionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ABoxCharacter* Player = Cast<ABoxCharacter>(OtherActor))
	{
		if (DetectedPlayer == Player)
		{
			bPlayerInSight = false;
		}
	}
}

void ADrone::OnInteractionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ABoxCharacter* Player = Cast<ABoxCharacter>(OtherActor))
	{
		if (CurrentState == EDroneState::Chasing && Player == DetectedPlayer)
		{
			GrabPlayer(Player);
		}
	}
}

// Blueprint Callable Functions
void ADrone::SetPatrolPoints(const TArray<ATargetPoint*>& NewPatrolPoints)
{
	PatrolPoints = NewPatrolPoints;
	CurrentPatrolIndex = 0;
	
	if (PatrolPoints.Num() > 0 && CurrentState == EDroneState::Patrolling)
	{
		SetNewPatrolTarget();
	}
}

void ADrone::SetDropOffPoint(ATargetPoint* NewDropOffPoint)
{
	DropOffPoint = NewDropOffPoint;
}

void ADrone::ForceEndChase()
{
	DetectedPlayer = nullptr;
	bPlayerInSight = false;
	bSafeZoneActive = true;
	ChangeState(EDroneState::Returning);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, TEXT("Chase ended: Player in safe zone!"));
	}
}

