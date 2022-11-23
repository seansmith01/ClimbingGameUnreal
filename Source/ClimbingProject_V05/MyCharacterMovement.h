// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/InputComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "UObject/Object.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Math/Vector.h"
#include "DrawDebugHelpers.h"
#include "Containers/Array.h"


#include "MyCharacterMovement.generated.h"

enum Movement
{
	Climbing,
	NotClimbing,
	Mantling,
	Hovering
};
UCLASS()
class CLIMBINGPROJECT_V05_API AMyCharacterMovement : public ACharacter
{

	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacterMovement();

	Movement currentMovement;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		UCameraComponent* FollowCamera;

		USphereComponent* Bubble;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	void MoveForward(float axis);
	void MoveRight(float axis);
	void MyJumpPress();
	void MyJumpHold();
	void MyJumpRelease();
	void ClimbingJumpApexCheck();
	void Grabbing();
	void NotGrabbing();
	void Mantle();
	void DecreaseStamina();
	void ToggleBool(bool b);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FVector2D InputVector;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool hovering;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float hoveringFallSpeed = -2.f;
	bool mantling;
	bool grabbing;
	bool holdingJump;
	bool climbingToggle = false;
	//bool climbedForSingleFrame = false;
	bool groundedForSingleFrame = false;
	bool reachedJumpApexForSingleFrame = false;

	bool Raycast(FVector startOffset, FVector endOffset, FVector& impactPoint, FVector& normal, bool drawRay);

	void ClimbEnter();


	float distFromWall = 70.f;

	void SetRayOffsets();
	FVector startMantleOffsetForward;
	FVector endMantleOffsetForward;
	FVector startMantleOffsetDown;
	FVector endMantleOffsetDown;
	TArray<FVector> startOffsets;
	TArray<FVector> __endOffsets;
	TArray<FVector> impactPoints;
	TArray<FVector>	normals;

	bool IsClimbing(FVector& impactPointsPlusNormals, FVector& impactPointsPlusNormalsForPos);
	void MantleCheck();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isMantling = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isClimbing = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isClimbJumping = false;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
		//FVector endOffset;

	FVector startmantlePos;
	FVector matlePos;
	float mantleDuration = 0.75f;
	float mantleTimeElapsed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float JumpPowerIncreaseRate = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxJumpPower = 0.f;
	float JumpPower = 0.f;

	float CurrentStamina = 0.f;
	float DefaultStamina = 100.f;
	float StaminaDecreaseRate = 0.f;

	bool shouldJumpOffWall1;
	bool shouldJumpOffWall2;

	float bonusRight;
	float bonusForward;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

