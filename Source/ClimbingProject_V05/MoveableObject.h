// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "Containers/Array.h"



#include "MoveableObject.generated.h"

UCLASS()
class CLIMBINGPROJECT_V05_API AMoveableObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMoveableObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	float timeElapsed =0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float duration = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector>	offsets;
	FVector	startPos;
	FVector	tempStartPos;
	FVector	targetPos;
	int i;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
