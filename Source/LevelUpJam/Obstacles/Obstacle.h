#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Obstacle.generated.h"

class UBoxComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActivatedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeactivatedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractedDelegate);

UENUM(BlueprintType)
enum class EObstacleState : uint8
{
	Idle,
	Active,
	Cooldown,
	Disabled
};

UCLASS()
class LEVELUPJAM_API AObstacle : public AActor
{
	GENERATED_BODY()
	
public:
	// Blueprint event dispatchers
	UPROPERTY(BlueprintAssignable, Category = "Obstacle|Events")
	FOnActivatedDelegate OnActivated;

	UPROPERTY(BlueprintAssignable, Category = "Obstacle|Events")
	FOnDeactivatedDelegate OnDeactivated;

	// Sound to play when launch occurs
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Effects")
	USoundBase* LaunchSound = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Effects")
	UParticleSystem* CascadeLaunchEffect = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Effects")
	class UNiagaraSystem* NiagaraLaunchEffect = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Effects")
	bool bPlayEffectsOnActivate = true;
	
	// Timers 
	/** Whether the obstacle auto-triggers activation on overlap with player. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Proximity")
	bool bActivateOnPlayerProximity = false;

	/** Whether the obstacle auto-triggers activation on overlap with any object including. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Proximity")
	bool bActivateOnObjectProximity = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Timing|Auto")
	bool bActivateOnStart = false;
	
	/** Delay before the obstacle is able to activate again. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Timing|Auto")
	float AutoResetActivationDelay = 0.f; // 0 means no reset

	FTimerHandle ActivationResetTimerHandle;

	/** Delay before the obstacle is able to deactivate again. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Timing|Auto")
	float AutoResetDeactivationDelay = 0.f; // 0 means no reset

	FTimerHandle DeactivationResetTimerHandle;

	/** Delay before the action activates */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Timing")
    float ReactionDelay = 0.0f;
    
    FTimerHandle ReactionTimerHandle;
	
	AObstacle();
	
	// Expose for blueprint use
	UFUNCTION(BlueprintCallable, Category = "Obstacle")
	virtual void Activate();

	UFUNCTION(BlueprintCallable, Category = "Obstacle")
	virtual void Deactivate();

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Obstacle")
	virtual void SetupAutoLoop();

	UFUNCTION(BlueprintCallable, Category = "Obstacle|Effects")
	virtual void PlayEffects();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle")
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Obstacle")
	UBoxComponent* Collider;
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
							UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
							bool bFromSweep, const FHitResult& SweepResult);
};