// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"
// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Player/Input/IMC_Player.IMC_Player'"));
	if (InputMappingContextRef.Object)
	{
		InputMappingContext = InputMappingContextRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/IA_Move.IA_Move'"));
	if (MoveActionRef.Object)
	{
		MoveAction = MoveActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> LookActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/IA_Look.IA_Look'"));
	if (LookActionRef.Object)
	{
		LookAction = LookActionRef.Object;
	}
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem
		= ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
	//GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;

	
}


// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);


	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
}


void APlayerCharacter::Move(const FInputActionValue& Value)
{

	FVector2D MovementVector = Value.Get<FVector2D>();

	// 입력 값이 0이면 회전하지 않도록 예외 처리
	if (MovementVector.IsZero())
	{
		return;
	}

	// Forward 및 Right 방향 벡터 계산
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); // 앞 방향
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);    // 오른쪽 방향

	// 캐릭터 이동 처리
	AddMovementInput(ForwardDirection, MovementVector.Y); // W/S에 해당
	AddMovementInput(RightDirection, MovementVector.X);    // A/D에 해당

	// 캐릭터가 이동하는 방향 계산
	FVector MovementDirection = (ForwardDirection * MovementVector.Y) + (RightDirection * MovementVector.X);

	// 벡터가 0이 아니라면 회전 처리
	if (!MovementDirection.IsNearlyZero())
	{
		// 이동 방향으로 회전
		FRotator TargetRotation = MovementDirection.Rotation();  // 이동 방향의 회전값
		TargetRotation.Pitch = 0; // Pitch는 고정
		TargetRotation.Roll = 0;  // Roll은 고정

		// 부드러운 회전을 위해 Lerp 사용
		FRotator CurrentRotation = GetActorRotation();
		FRotator NewRotation = FMath::Lerp(CurrentRotation, TargetRotation, 0.1f); // 0.1f는 회전 속도 조정

		// 캐릭터의 회전 설정
		SetActorRotation(NewRotation);
	}
}