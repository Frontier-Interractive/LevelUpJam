// RespawnPoint.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RespawnPoint.generated.h"

UCLASS(Blueprintable)
class LEVELUPJAM_API ARespawnPoint : public AActor
{
	GENERATED_BODY()

public:
	ARespawnPoint();

	// Unique identifier for this respawn point
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	FName RespawnID;

	// The transform of the respawn point (location, rotation, scale)
	UFUNCTION(BlueprintCallable, Category = "Respawn")
	FTransform GetRespawnTransform() const { return GetActorTransform(); }
};
