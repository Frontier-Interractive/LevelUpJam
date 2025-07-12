#include "MovingObstacle.h"

#include "Components/BoxComponent.h"

AMovingObstacle::AMovingObstacle()
{
	
}

void AMovingObstacle::Activate()
{
	bMovingUp = true;
	bShouldMove = true;

	Super::Activate(); // Broadcasts event
}

void AMovingObstacle::Deactivate()
{
	bMovingUp = false;
	bShouldMove = true;
	
	Super::Deactivate(); // Broadcasts event
}

void AMovingObstacle::SetupAutoLoop()
{
	bActivateOnStart = true;
	MoveSpeed = 5.0f;
	AutoResetActivationDelay = 1.0f;
	AutoResetDeactivationDelay = 1.0f;
}

void AMovingObstacle::BeginPlay()
{
	Super::BeginPlay();

	// Store the original position
	StartLocation = Collider->GetRelativeLocation();
}

void AMovingObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bShouldMove)
		return;

	FVector CurrentLocation = Collider->GetRelativeLocation();
	FVector DesiredLocation = bMovingUp ? StartLocation + (MoveDirection * MoveAmount) : StartLocation;

	FVector NewLocation = FMath::VInterpTo(CurrentLocation, DesiredLocation, DeltaTime, MoveSpeed);
	Collider->SetRelativeLocation(NewLocation);

	// Stop moving when close enough
	if (FVector::Dist(NewLocation, DesiredLocation) < 1.0f)
	{
		Collider->SetRelativeLocation(DesiredLocation);
		bShouldMove = false;
	}
}