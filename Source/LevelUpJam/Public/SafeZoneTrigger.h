#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "SafeZoneTrigger.generated.h"

UCLASS()
class LEVELUPJAM_API ASafeZoneTrigger : public AActor
{
	GENERATED_BODY()

public:
	ASafeZoneTrigger();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* TriggerBox;

	// Reference to the drone to notify
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SafeZone")
	class ADrone* DroneRef;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
