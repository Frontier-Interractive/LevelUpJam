#include "LaunchObstacle.h"

#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"

ALaunchObstacle::ALaunchObstacle()
{
	PrimaryActorTick.bCanEverTick = true;

	TriggerComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
	TriggerComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerComponent->SetupAttachment(RootComponent);
}

void ALaunchObstacle::BeginPlay()
{
	Super::BeginPlay();

	// If the launch direction is not set we should launch in the same direction as the obstacle moves.
	LaunchDirection = LaunchDirection.IsZero() ? MoveDirection : LaunchDirection;

	
	Collider->OnComponentBeginOverlap.AddDynamic(this, &ALaunchObstacle::OnLaunchBeginOverlap);
	Collider->OnComponentEndOverlap.AddDynamic(this, &ALaunchObstacle::OnLaunchEndOverlap);
	
	TriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &ALaunchObstacle::HandleBeginOverlap);
	
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

void ALaunchObstacle::HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bShouldMoveTowardsTarget == true)
	{
		MoveTowardsTargetActor(OtherActor);
		LaunchDirection = MoveDirection;
	}
	
	Super::HandleBeginOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void ALaunchObstacle::OnLaunchBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                           bool bFromSweep, const FHitResult& SweepResult)
{
	//Launch player characters
	if (ACharacter* Char = Cast<ACharacter>(OtherActor); Char && Char->IsPlayerControlled())
	{
		Char->LaunchCharacter(LaunchDirection * LaunchStrength, false, false);
		return;
	}
	
	if (!OtherComp || !OtherComp->IsSimulatingPhysics())
	{
		UE_LOG(LogTemp, Warning, TEXT("LaunchObstacle: '%s' overlapped, but component '%s' does not simulate physics."),
	   *OtherActor->GetName(), *OtherComp->GetName());
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
