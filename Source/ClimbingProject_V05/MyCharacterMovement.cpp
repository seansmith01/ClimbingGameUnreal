//assain// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterMovement.h"

// Sets default values
AMyCharacterMovement::AMyCharacterMovement()
{
	currentMovement = Movement::NotClimbing;

	SetRayOffsets();


	normals.Init(FVector(0), startOffsets.Num());
	impactPoints.Init(FVector(0), startOffsets.Num());

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false; // pointless coz the editor overrides it!!!!! >:(((((((((((((((
	bUseControllerRotationRoll = false; // leaving this here

	GetCharacterMovement()->bOrientRotationToMovement = true; //set character to face where going
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.5f;

	//Bubble_Object = GetDefaultSubobjectByName("Bubble");
	//Bubble_Object.SetVisibility();
	// 
	//Bubble = GetOwner()->FindComponentByClass<USphereComponent>();
	//Bubble->SetVisibility(false);

	//auto Owner = GetOwner();
	//if (!ensure(Owner != nullptr)) { return; }
	//Bubble = GetOwner()->FindComponentByClass<USphereComponent>();




	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera_Boom"));
	//parent it to the root in bp
	CameraBoom->SetupAttachment(RootComponent);
	//CameraBoom->TargetArmLength = 800.f;
	CameraBoom->bUsePawnControlRotation = true; // rotate the arm based on controller

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow_Camera"));
	//make it child of camera boom
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	
	FollowCamera->bUsePawnControlRotation = false; //false because the camera boom has this control

}

// Called to bind functionality to input
void AMyCharacterMovement::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("LookRightMouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUpMouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyCharacterMovement::MyJumpPress);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMyCharacterMovement::MyJumpRelease);
	PlayerInputComponent->BindAction("Grab", IE_Pressed, this, &AMyCharacterMovement::Grabbing);
	PlayerInputComponent->BindAction("Grab", IE_Released, this, &AMyCharacterMovement::NotGrabbing);
	PlayerInputComponent->BindAction("ToggleBool", IE_Released, this, &AMyCharacterMovement::NotGrabbing);


	PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacterMovement::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacterMovement::MoveRight);


}

// Called when the game starts or when spawned
void AMyCharacterMovement::BeginPlay()
{
	Super::BeginPlay();
}

void AMyCharacterMovement::SetRayOffsets() {

	startMantleOffsetForward = FVector(0, 90, -20); //has to start further back on can clip through walls
	endMantleOffsetForward = FVector(0, 90, 70);
	startMantleOffsetDown = FVector(0, 120, 70);
	endMantleOffsetDown = FVector(0, -50, 70);

	startOffsets.Add(FVector(0, 0, 0));
	__endOffsets.Add(FVector(0, 0, 100));
	float yLower = 50.f;
	float yUpper = 80.f;
	float z = 100.f;

	//bottom Left
	startOffsets.Add(FVector(-50, -yUpper, 0));
	__endOffsets.Add(FVector(-20, -yUpper, z));
	//bottom right
	startOffsets.Add(FVector(50, -yUpper, 0));
	__endOffsets.Add(FVector(20, -yUpper, z));
	//top right
	startOffsets.Add(FVector(50, yUpper, 0));
	__endOffsets.Add(FVector(20, yUpper, z));
	//top left
	startOffsets.Add(FVector(-50, yUpper, 0));
	__endOffsets.Add(FVector(-20, yUpper, z));
	//top centre
	startOffsets.Add(FVector(0, yUpper, 0));
	__endOffsets.Add(FVector(0, yUpper, z));
	//bottom centre
	startOffsets.Add(FVector(0, -yUpper, 0));
	__endOffsets.Add(FVector(0, -yUpper, z));
	//Middle ones to even out rotations
	startOffsets.Add(FVector(-30, 20, 0));
	__endOffsets.Add(FVector(-30, 55, z));

	startOffsets.Add(FVector(30, 20, 0));
	__endOffsets.Add(FVector(30, 55, z));

	startOffsets.Add(FVector(-30, -20, 0));
	__endOffsets.Add(FVector(-30, -55, z));

	startOffsets.Add(FVector(30, -20, 0));
	__endOffsets.Add(FVector(30, -55, z));
	//
	startOffsets.Add(FVector(-30, 0, 0));
	__endOffsets.Add(FVector(-30, 55, z));

	startOffsets.Add(FVector(30, 0, 0));
	__endOffsets.Add(FVector(30, 55, z));

	startOffsets.Add(FVector(-30, -0, 0));
	__endOffsets.Add(FVector(-30, -55, z));

	startOffsets.Add(FVector(30, -0, 0));
	__endOffsets.Add(FVector(30, -55, z));

	//up looking up and down looking down
	startOffsets.Add(FVector(-30, 50, 0));
	__endOffsets.Add(FVector(-30, 120, 50));

	startOffsets.Add(FVector(30, 50, 0));
	__endOffsets.Add(FVector(30, 120, 50));

	startOffsets.Add(FVector(-30, -50, 0));
	__endOffsets.Add(FVector(-30, -120, 50));

	startOffsets.Add(FVector(30, -50, 0));
	__endOffsets.Add(FVector(30, -120, 50));


}

// Called every frame
void AMyCharacterMovement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	
	
	FVector newLocation = GetActorLocation();
	FVector lookFromRotation;
	//if grabbing
	if (grabbing) {
		//If you still have stamina
		if (CurrentStamina > 0) {
			//check if can climb and save the impact points from the function
			if (IsClimbing(lookFromRotation, newLocation)) {
				currentMovement = Movement::Climbing;
			}
			else {
				currentMovement = Movement::NotClimbing;
			}
		}
		//ran out of stamina
		else {
			currentMovement = Movement::NotClimbing;
		}
	}
	//not grabbing and no floating or mantling
	else if (currentMovement != Movement::Mantling && currentMovement != Movement::Hovering) {
		currentMovement = Movement::NotClimbing;
	}
	FString decreaseRateStr = FString::SanitizeFloat(StaminaDecreaseRate);
	FString staminaStr = FString::SanitizeFloat(CurrentStamina);
	//GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *decreaseRateStr);
	GEngine->AddOnScreenDebugMessage(-2, 1.0, FColor::Red, *staminaStr, true, FVector2D(2,2));
	

	if (currentMovement == Movement::Mantling) {
		isMantling = true;
		isClimbing = true;
		Mantle();
	}
	else if (currentMovement == Movement::Climbing) {
		//check if can mantle
		MantleCheck();
		//if not grounded, decrease stamina
		//if (!GetCharacterMovement()->IsMovingOnGround()) {
			DecreaseStamina();
		//}
		SetActorLocation(newLocation);
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(lookFromRotation, impactPoints[0]));

		if (!climbingToggle) {
			//called on first frame on climbing
			GetCharacterMovement()->SetMovementMode(MOVE_Flying);
			climbingToggle = true;
			ClimbEnter();
		}
		//jump apex check
		ClimbingJumpApexCheck();
		if (holdingJump) {
			MyJumpHold();
		}
	}
	else if (currentMovement == Movement::NotClimbing) {
		if (climbingToggle) {//called on first frame of stopping climbing
			
			climbingToggle = false;
			isClimbing = false;
			isMantling = false;

			GetCharacterMovement()->SetMovementMode(MOVE_Falling);
			GetCharacterMovement()->bOrientRotationToMovement = true;

			SetActorRotation(FQuat(0, 0, GetActorRotation().Quaternion().Z, 1));
		}
		//if grounded
		if (GetCharacterMovement()->IsMovingOnGround()) {
			//called on first frame of being grounded
			if (!groundedForSingleFrame) {
				groundedForSingleFrame = true;
				holdingJump = false;
			}
			CurrentStamina = DefaultStamina;
			//jump
			if (holdingJump) {
				MyJumpHold();
			}

		}
		else { // in air
			//called on first frame of being in air
			if (groundedForSingleFrame) {
				groundedForSingleFrame = false;
			}
			if (holdingJump) {
				currentMovement = Movement::Hovering;
			}
		}
	}
	if (currentMovement == Movement::Hovering) {
		
		GetCharacterMovement()->Velocity = FVector(GetCharacterMovement()->Velocity.X, GetCharacterMovement()->Velocity.Y, hoveringFallSpeed);

		if (!hovering) {
			//called on first frame of hovering
			isClimbJumping = false;
			isMantling = false;
			hovering = true;
			//GetCharacterMovement()->Velocity = FVector(GetCharacterMovement()->GetLastUpdateVelocity().X, GetCharacterMovement()->GetLastUpdateVelocity().Y, 0);
			//GetCharacterMovement()->GravityScale = 0.15f;
			GetCharacterMovement()->AirControl = 2.5f;

		}
		if (GetCharacterMovement()->IsMovingOnGround()) {
			currentMovement = Movement::NotClimbing;
		}
		
		
	}
	else {
		hovering = false;
		GetCharacterMovement()->GravityScale = 1.f;
		GetCharacterMovement()->AirControl = 0.5f;
	}
}
bool AMyCharacterMovement::IsClimbing(FVector& newLookFromRotation, FVector& newLocation)
{
	//bool isClimbing = false;
	//climbing check
	//for each start offsets (or end offset because theres the same amount) there will be a ray going from the start to end position
	for (int i = 0; i < startOffsets.Num(); i++)
	{
		//initialise empty vector variables for the normal and hitpoint that will get their value from the raycast function if the ray hits something
		FVector impactPoint, normal;
		if (Raycast(startOffsets[i], __endOffsets[i], impactPoint, normal, false)) {
			
			if (normal.Z < 0.7f) {
				//insert hit and normal value into arrays
				//using insert because add would keep adding on new impact points and normals every frame
				impactPoints.Insert(impactPoint, i);
				normals.Insert(normal, i);
			}
		}
		//ray cast misses
		else {
			//if centre ray missed, not climbing
			if (i == 0) {
				return false;
			}
			//if the ray is missing set the impactpoint and normals to 0
			impactPoints[i] = FVector(0);
			normals[i] = FVector(0);
		}
	}

	//new arrays initialized for the hits and normals that are in use (not set to 0)
	TArray<FVector> pointsInUse;
	TArray<FVector> normalsInUse;
	//for each potential ray normal and hit info (that qould be 0 if not in use), add their data into to new array if they are being used (not 0)
	for (int i = 0; i < startOffsets.Num(); i++)
	{
		//ignore the data if their normals and impact points are 0 (that ray is missing)
		if (impactPoints[i] != FVector(0) && normals[i] != FVector(0)) {
			//normals and impact points are in use
			pointsInUse.Add(impactPoints[i]);
			normalsInUse.Add(normals[i]);
		}
	}
	//get mid point between the hit point of the central forward ray and the mid of points of all the normals the other rays are hitting added onto that central point
	FVector midPointsPlusNormals;
	for (int i = 0; i < pointsInUse.Num(); i++)
	{
		//add all the points in use
		midPointsPlusNormals += (pointsInUse[0] + (normalsInUse[i] * 50.f));
		//DrawDebugLine(GetWorld(), pointsInUse[0], pointsInUse[0] + (normalsInUse[i] * 50.f), FColor::Blue, false, 0.f, 0, 1);
		//DrawDebugLine(GetWorld(), pointsInUse[i], pointsInUse[i] + (normalsInUse[i] * 50.f), FColor::Blue, false, 0.f, 0, 1);
	}
	//divide by the amount of points (getting the midpoint)
	midPointsPlusNormals /= pointsInUse.Num();

	//DrawDebugLine(GetWorld(), impactPoints[0], midPointsPlusNormals, FColor::Red, false, 0.0f, 0, 1);
	newLookFromRotation = midPointsPlusNormals;
	newLocation = midPointsPlusNormals;
	//the rotation will be set to the first hit point in front of the player + the average of the points plus each of their normals
	return true;
}
void AMyCharacterMovement::ClimbEnter() {
	isClimbing = true;
	isMantling = false;
	isClimbJumping = false;
	holdingJump = false;
	GetCharacterMovement()->Velocity = FVector(0, 0, 0);
	GetCharacterMovement()->bOrientRotationToMovement = false;
}



bool AMyCharacterMovement::Raycast(FVector startOffset, FVector endOffset, FVector& impactPoint, FVector& normal, bool drawRay)
{
	FHitResult hit;

	FVector startPos = GetActorLocation() + ((GetActorRightVector() * startOffset.X) + (GetActorUpVector() * startOffset.Y) + (GetActorForwardVector() * startOffset.Z));
	FVector endPos = GetActorLocation() + ((GetActorRightVector() * endOffset.X) + (GetActorUpVector() * endOffset.Y) + (GetActorForwardVector() * endOffset.Z));
	//draw ray
	if (drawRay) {
		//DrawDebugLine(GetWorld(), startPos, endPos, FColor::Green, false, 0.0f, 0, 1);
	}
	//ignore player
	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(this->GetOwner());

	bool isHit = GetWorld()->LineTraceSingleByChannel(hit, startPos, endPos, ECC_Visibility, collisionParams);

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("normal: %s"), *hit.Normal.Z.ToString()));

	//FString TheFloatStr = FString::SanitizeFloat(hit.Normal.Z);
	//GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *TheFloatStr);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(hit.Normal.Z));
	//UE_LOG(LogTemp, Warning, TEXT(hit.Normal.ToString()));

	if (isHit) {

		normal = hit.Normal;
		impactPoint = hit.ImpactPoint;

		return true;
	}
	return false;
}

void AMyCharacterMovement::MoveForward(float axis)
{
	InputVector = FVector2D(InputVector.X, axis * 100.f);
	FVector dir_player_relative;
	FVector dir_camera_relative;

	FRotator rot = Controller->GetControlRotation();;
	FRotator yaw_rot = FRotator(0.f, rot.Yaw, 0.f);
	dir_player_relative = GetActorUpVector();  //gets forward up relative to player
	dir_camera_relative = FRotationMatrix(yaw_rot).GetUnitAxis(EAxis::X); //gets right direction relative to camera
	FVector cross = FVector::CrossProduct(GetActorRightVector(), dir_camera_relative);

	if (currentMovement == Movement::Climbing) {
		AddMovementInput(dir_player_relative, axis);
		//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("cross: %s"), *cross.ToString()));
		if (cross.Z > 0.5f && axis > 0) { //if looking backwards but holding forwards, jump off the wall
			shouldJumpOffWall1 = true;
		}
		else if (axis < 0) { //if holding back, jump off the wall 
			shouldJumpOffWall1 = true;
		}
		else {
			shouldJumpOffWall1 = false;
		}
		bonusForward = axis;
		
	}
	else {

		AddMovementInput(dir_camera_relative, axis);
		bonusForward = 0;
		shouldJumpOffWall1 = false;
	}

	
}
void AMyCharacterMovement::MoveRight(float axis)
{
	InputVector = FVector2D(axis * 100.f, InputVector.Y);
	FVector dir_player_relative;
	FVector dir_camera_relative;

	FRotator rot = Controller->GetControlRotation();;
	FRotator yaw_rot = FRotator(0.f, rot.Yaw, 0.f);
	dir_player_relative = GetActorRightVector(); //gets right direction relative to player
	dir_camera_relative = FRotationMatrix(yaw_rot).GetUnitAxis(EAxis::Y); //gets right direction relative to camera
	FVector cross = FVector::CrossProduct(dir_player_relative, dir_camera_relative);

	if (currentMovement == Movement::Climbing) {
		AddMovementInput(dir_player_relative, axis);

		//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("cross: %s"), *cross.ToString()));
		if (cross.Z > 0.8f && axis > 0) { //if looking and holding right jump off the wall right
			shouldJumpOffWall2 = true;
		}
		else if(cross.Z < -0.8f && axis < 0) {//if looking and holding left jump off the wall left
			shouldJumpOffWall2 = true;
		}
		else {
			shouldJumpOffWall2 = false;
		}
		bonusRight = axis;
	}
	else {
		shouldJumpOffWall2 = false;
		AddMovementInput(dir_camera_relative, axis);
		bonusRight = 0;
	}
}

void AMyCharacterMovement::MantleCheck() {

	FVector impactPoint, normal;
	//if forward check is missing
	if (!Raycast(startMantleOffsetForward, endMantleOffsetForward, impactPoint, normal, true)) {

		//if down ward ray check is hitting
		if (Raycast(startMantleOffsetDown, endMantleOffsetDown, impactPoint, normal, true)) {

			//DrawDebugLine(GetWorld(), startMantleOffset, endMantleOffset, FColor::Red, false, 0.0f, 0, 1);
			//FString TheFloatStr = FString::SanitizeFloat(normal.Z);
			//GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *TheFloatStr);
			//check if not too steep
			if (normal.Z >= 0.7f) {

				//canMantle = true;
				//grabbing = false;
				//currentMovement = Movement::NotClimbing;

				grabbing = false;
				//saved for mantle function (incase the moving object)
				matlePos = impactPoint;
				//only saved once
				//if (currentMovement != Movement::Mantling) {
					startmantlePos = GetActorLocation();

				//}
				currentMovement = Movement::Mantling;

				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("MANTLE")));
			}
		}
	}
	else {
		//canMantle = true;
	}
}
void AMyCharacterMovement::Mantle() {
	if (mantleTimeElapsed < mantleDuration) {
		mantleTimeElapsed += GetWorld()->DeltaTimeSeconds;
		//SetActorLocation(FMath::Lerp(GetActorLocation(), matlePos + FVector(0, 0, 50) + (GetActorForwardVector() * 6), mantleTimeElapsed / mantleDuration));
		SetActorLocation(FMath::Lerp(startmantlePos, matlePos + FVector(0, 0, 50), mantleTimeElapsed / mantleDuration));
	}
	else {
		mantleTimeElapsed = 0;
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		currentMovement = Movement::NotClimbing;
	}
}
void AMyCharacterMovement::DecreaseStamina() {
	StaminaDecreaseRate = 3.f * ((-normals[0].Z + 1.f) * 2.f) / 1.1f; // make normals always positive with +1 then
	CurrentStamina -= StaminaDecreaseRate * GetWorld()->DeltaTimeSeconds;
}

void AMyCharacterMovement::ClimbingJumpApexCheck() {

	if (!grabbing) {
		return;
	}

	//check if going down and is still grabbing
	if (GetCharacterMovement()->GetLastUpdateVelocity().Z < 0) {
		//REACHED APEX OF JUMP
		if (!reachedJumpApexForSingleFrame) {
			reachedJumpApexForSingleFrame = true;
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Down")));
			isClimbJumping = false;
			currentMovement = Movement::Climbing;
			climbingToggle = false; //reset this so it acts like we've hit the wall for the first time again
		}

	}
	else if (GetCharacterMovement()->GetLastUpdateVelocity().Z >= 0) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("up")));
		reachedJumpApexForSingleFrame = false;

	}
}
void AMyCharacterMovement::MyJumpPress() {
	JumpPower = 0.f;
	holdingJump = true;
}
//called if holdingJump is true
void AMyCharacterMovement::MyJumpHold() {

	if (JumpPower < MaxJumpPower) {

		JumpPower += JumpPowerIncreaseRate * GetWorld()->DeltaTimeSeconds;
		//FString jumpPwrStr = FString::SanitizeFloat(JumpPower);
		//GEngine->AddOnScreenDebugMessage(-2, 1.0, FColor::Red, *jumpPwrStr, true, FVector2D(2, 2));
	}
	
}
void AMyCharacterMovement::MyJumpRelease()
{
	if (!holdingJump) { //can be set to false by lnading or entering climbing(so player doesnt jump after hovering to climbing and then letting go of space)
		return;
	}
	if(currentMovement==Movement::Climbing)
		isClimbJumping = true;

	holdingJump = false;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	//GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	if (currentMovement==Movement::Hovering) {
		currentMovement = Movement::NotClimbing;
		return;
	}

	float jumpStaminaCost = 10.f;
	//if player wont still have stamina after jump, jump with more force as a sort of last gasp effort
	if (CurrentStamina - jumpStaminaCost < 0) {
		JumpPower += 200.f;;
	}	
	CurrentStamina -= jumpStaminaCost;

	float totalJumpPower = JumpPower + 250.f;
	if (shouldJumpOffWall1 || shouldJumpOffWall2) {
		
		GetCharacterMovement()->Launch(((GetActorUpVector() * totalJumpPower) + (GetActorForwardVector() * -300.f) + (GetActorRightVector() * bonusRight * 150.f)));
		GEngine->AddOnScreenDebugMessage(-2, 1.0, FColor::Red, TEXT("away"));

		grabbing = false;
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AMyCharacterMovement::Grabbing, 0.2f, false);
	}
	else {
		GetCharacterMovement()->Launch((GetActorUpVector() * totalJumpPower) + (GetActorRightVector() * bonusRight * 150.f));
		GEngine->AddOnScreenDebugMessage(-2, 1.0, FColor::Red, TEXT("NOTaway"));
	}
	//Reset values
	JumpPower = 0.f;
}

void AMyCharacterMovement::Grabbing()
{
	grabbing = true;
	//grabbing = !grabbing;
}

void AMyCharacterMovement::NotGrabbing()
{
	grabbing = false;
}
void AMyCharacterMovement::ToggleBool(bool b)
{
	b = !b;
}
