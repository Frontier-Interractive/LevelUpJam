#include "Obstacle.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

AObstacle::AObstacle()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	
	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collider->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collider->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Collider->SetupAttachment(RootComponent);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	/*Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);*/
	Mesh->SetupAttachment(Collider);
}

void AObstacle::Activate()
{
	OnActivated.Broadcast();

	if (AutoResetDeactivationDelay > 0.0f) // Deactivate after a delay > 0
	{
		GetWorldTimerManager().SetTimer(DeactivationResetTimerHandle, this, &AObstacle::Deactivate, AutoResetDeactivationDelay, false);
	}

	if (bPlayEffectsOnActivate)
	{
		PlayEffects();
	}
}

void AObstacle::Deactivate()
{
	OnDeactivated.Broadcast();

	if (AutoResetActivationDelay > 0.0f) // Reactivate after a delay > 0
	{
		GetWorldTimerManager().SetTimer(ActivationResetTimerHandle, this, &AObstacle::Activate, AutoResetActivationDelay, false);
	}
}

void AObstacle::PlayEffects()
{
	FVector SpawnLocation = GetActorLocation();

	// 🔊 Play sound
	if (ActivateSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ActivateSound, SpawnLocation);
	}

	// ✨ Play particle
	if (CascadeLaunchEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), CascadeLaunchEffect, SpawnLocation);
	}

	// 🌌 Play Niagara effect
	if (NiagaraLaunchEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraLaunchEffect, SpawnLocation);
	}
}

void AObstacle::HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bActivateOnObjectProximity)
	{
		Activate();
	}
	else if (bActivateOnPlayerProximity)
	{
		if (const ACharacter* Character = Cast<ACharacter>(OtherActor); Character && Character->IsPlayerControlled())
		{
			Activate();
		}
	}
}

// Called when the game starts or when spawned
void AObstacle::BeginPlay()
{
	Super::BeginPlay();

	if (bActivateOnStart)
	{
		if (AutoResetActivationDelay > 0.0f) // Reactivate after a delay > 0
		{
			GetWorldTimerManager().SetTimer(ActivationResetTimerHandle, this, &AObstacle::Activate, AutoResetActivationDelay, false);
		}
	}
}

// Called every frame
void AObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AObstacle::SetupAutoLoop()
{
	bActivateOnStart = true;
	AutoResetActivationDelay = 1.0f;
	AutoResetDeactivationDelay = 1.0f;
}