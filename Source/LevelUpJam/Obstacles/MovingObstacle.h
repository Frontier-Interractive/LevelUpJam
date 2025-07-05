#pragma once

#include "CoreMinimal.h"
#include "Obstacle.h"
#include "MovingObstacle.generated.h"

/**
 * 
 */
UCLASS()
class LEVELUPJAM_API AMovingObstacle : public AObstacle
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Obstacle|Move")
	FVector StartLocation;

	/** How far the door moves up */
	UPROPERTY(EditAnywhere, Category = "Obstacle|Move")
	float MoveAmount = 100.0f;

	// Door movement speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle|Move")
	float MoveSpeed = 3.0f;
	
	AMovingObstacle();

	virtual void Activate() override;
	virtual void Deactivate() override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Internal state flags
	bool bMovingUp = false;
	bool bShouldMove = false;
};
