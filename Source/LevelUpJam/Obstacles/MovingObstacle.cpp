#include "MovingObstacle.h"

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
	StartLocation = Mesh->GetRelativeLocation();
}

void AMovingObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bShouldMove)
		return;

	FVector CurrentLocation = Mesh->GetRelativeLocation();
	FVector DesiredLocation = bMovingUp ? StartLocation + FVector(0, 0, MoveAmount) : StartLocation;

	FVector NewLocation = FMath::VInterpTo(CurrentLocation, DesiredLocation, DeltaTime, MoveSpeed);
	Mesh->SetRelativeLocation(NewLocation);

	// Stop moving when close enough
	if (FVector::Dist(NewLocation, DesiredLocation) < 1.0f)
	{
		Mesh->SetRelativeLocation(DesiredLocation);
		bShouldMove = false;
	}
}