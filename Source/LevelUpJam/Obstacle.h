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

	UPROPERTY(BlueprintAssignable, Category = "Obstacle|Events")
	FOnInteractedDelegate OnInteracted;

	// Timers 
	/** Whether the obstacle auto-triggers activation on overlap */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Trigger")
	bool bActivateOnPlayerProximity = false;
	
	/** Delay before the action activates */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mechanical|Timing")
	float ReactionDelay = 0.5f;
	
	FTimerHandle ReactionTimerHandle;

	/** Delay before the obstacle is able to activate again. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle")
	float AutoResetActivationDelay = 0.f; // 0 means no reset

	FTimerHandle ResetTimerHandle;
	
	AObstacle();
	
	// Expose for blueprint use
	UFUNCTION(BlueprintCallable, Category = "Obstacle")
	virtual void Activate();

	UFUNCTION(BlueprintCallable, Category = "Obstacle")
	virtual void Deactivate();
	
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
