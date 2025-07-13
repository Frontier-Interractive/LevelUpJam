#pragma once

#include "CoreMinimal.h"
#include "MovingObstacle.h"
#include "LaunchObstacle.generated.h"

/**
 * 
 */
UCLASS()
class LEVELUPJAM_API ALaunchObstacle : public AMovingObstacle
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Launch")
	bool bShouldMoveTowardsTarget = false;
	
	ALaunchObstacle();

protected:
	/** Direction of the launch force */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Launch")
	FVector LaunchDirection = FVector::Zero();

	/** Magnitude of the launch force */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Launch")
	float LaunchStrength = 1000.f;

	/** Apply as an impulse (recommended for instant launch) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Launch")
	bool bUseImpulse = true;

	/** Apply force every frame while overlapping */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Launch")
	bool bApplyContinuousLaunch = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Launch")
	TObjectPtr<class USphereComponent> TriggerComponent;
	
	/** Components currently overlapping */
	TSet<UPrimitiveComponent*> OverlappingComponents;

	virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
	
	virtual void HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                                bool bFromSweep, const FHitResult& SweepResult) override;
	
	UFUNCTION()
	void OnLaunchBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
							  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
							  bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnLaunchEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
							UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	virtual void ApplyLaunchToComponent(UPrimitiveComponent* PrimComp);
};
