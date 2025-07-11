#include "SafeZoneTrigger.h"
#include "Drone.h"
#include "BoxCharacter.h"
#include "Components/BoxComponent.h"

ASafeZoneTrigger::ASafeZoneTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ASafeZoneTrigger::BeginPlay()
{
	Super::BeginPlay();
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ASafeZoneTrigger::OnBoxBeginOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ASafeZoneTrigger::OnBoxEndOverlap);
}

void ASafeZoneTrigger::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ADrone* Drone = DroneRef)
	{
		if (ABoxCharacter* Player = Cast<ABoxCharacter>(OtherActor))
		{
			Drone->ForceEndChase();
		}
	}
}

void ASafeZoneTrigger::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ADrone* Drone = DroneRef)
	{
		if (ABoxCharacter* Player = Cast<ABoxCharacter>(OtherActor))
		{
			Drone->bSafeZoneActive = false;
		}
	}
}
