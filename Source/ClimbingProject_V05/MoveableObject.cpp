// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveableObject.h"

// Sets default values
AMoveableObject::AMoveableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMoveableObject::BeginPlay()
{
	Super::BeginPlay();
	startPos = GetActorLocation();
	tempStartPos = startPos;
	targetPos = startPos + offsets[0];
	//and starting pos
	offsets.Add(FVector(0.f, 0.f, 0.f));
}

// Called every frame
void AMoveableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	
	
	if (timeElapsed < duration) {
		timeElapsed += GetWorld()->DeltaTimeSeconds;
		//targetPos = startPos += positions[i];
		SetActorLocation(FMath::Lerp(tempStartPos, startPos + offsets[i], timeElapsed / duration));
		//SetActorLocation(FMath::Lerp(tempStartPos, targetPos, timeElapsed / duration));
	}
	else {
		timeElapsed = 0.f;
		if (i + 1 < offsets.Num()) {

			tempStartPos = GetActorLocation();

			i++;

		}
		else {
			tempStartPos = startPos;
			
			i = 0; 
		}
	}

}

