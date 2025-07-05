#include "Obstacle.h"

#include "Components/BoxComponent.h"

// Sets default values
AObstacle::AObstacle()
{
 	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	Collider->SetupAttachment(Mesh);
	Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collider->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collider->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	Collider->OnComponentBeginOverlap.AddDynamic(this, &AObstacle::HandleBeginOverlap);
}

void AObstacle::Activate()
{
	OnActivated.Broadcast();
}

void AObstacle::Deactivate()
{
	OnDeactivated.Broadcast();
	
	// Check if we should reactivate after deactivation is called.
	if (AutoResetActivationDelay > 0.0f) // Reactivate after a delay > 0
	{
		GetWorldTimerManager().SetTimer(ResetTimerHandle, this, &AObstacle::Activate, AutoResetActivationDelay, false);
	}
}

// Called when the game starts or when spawned
void AObstacle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AObstacle::HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bActivateOnPlayerProximity)
	{
		Activate();
	}
}

