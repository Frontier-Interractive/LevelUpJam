#include "LaunchObstacle.h"

#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Character.h"

ALaunchObstacle::ALaunchObstacle()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALaunchObstacle::BeginPlay()
{
	Super::BeginPlay();

	if (Collider)
	{
		Collider->OnComponentBeginOverlap.AddDynamic(this, &ALaunchObstacle::OnLaunchBeginOverlap);
		Collider->OnComponentEndOverlap.AddDynamic(this, &ALaunchObstacle::OnLaunchEndOverlap);
	}
}

void ALaunchObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bApplyContinuousLaunch)
	{
		for (UPrimitiveComponent* Comp : OverlappingComponents)
		{
			if (IsValid(Comp) && Comp->IsSimulatingPhysics())
			{
				ApplyLaunchToComponent(Comp);
			}
		}
	}
}

void ALaunchObstacle::OnLaunchBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
										   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
										   bool bFromSweep, const FHitResult& SweepResult)
{
	//Launch player characters
	if (ACharacter* Char = Cast<ACharacter>(OtherActor); Char && Char->IsPlayerControlled())
	{
		Char->LaunchCharacter(LaunchDirection * LaunchStrength, true, true);
		return;
	}
	
	if (!OtherComp || !OtherComp->IsSimulatingPhysics())
	{
		UE_LOG(LogTemp, Warning, TEXT("(OnLaunchBeginOverlap) encountered actor that cannot be launched."));
		return;
	}

	// Launch simulating physics components
	if (bApplyContinuousLaunch)
	{
		OverlappingComponents.Add(OtherComp);
	}
	else
	{
		ApplyLaunchToComponent(OtherComp);
	}
}

void ALaunchObstacle::OnLaunchEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
										 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OverlappingComponents.Remove(OtherComp);
}

void ALaunchObstacle::ApplyLaunchToComponent(UPrimitiveComponent* PrimComp)
{
	FVector Force = LaunchDirection.GetSafeNormal() * LaunchStrength;

	if (bUseImpulse)
	{
		PrimComp->AddImpulse(Force, NAME_None, true);
	}
	else
	{
		PrimComp->AddForce(Force, NAME_None, true);
	}
}