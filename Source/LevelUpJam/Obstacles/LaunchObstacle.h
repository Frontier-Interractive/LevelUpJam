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
	ALaunchObstacle();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/** Direction of the launch force */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Launch")
	FVector LaunchDirection = FVector::UpVector;

	/** Magnitude of the launch force */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Launch")
	float LaunchStrength = 1000.f;

	/** Apply as an impulse (recommended for instant launch) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Launch")
	bool bUseImpulse = true;

	/** Apply force every frame while overlapping */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Launch")
	bool bApplyContinuousLaunch = false;

	/** Components currently overlapping */
	TSet<UPrimitiveComponent*> OverlappingComponents;

	UFUNCTION()
	void OnLaunchBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
							  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
							  bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnLaunchEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
							UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void ApplyLaunchToComponent(UPrimitiveComponent* PrimComp);
};
