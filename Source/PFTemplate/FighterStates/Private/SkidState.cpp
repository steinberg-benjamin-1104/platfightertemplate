#include "SkidState.h"
#include "FighterPawn.h"
#include "FighterMovementComponent.h"

void USkidState::OnEnter()
{
	FighterPawnRef->SwitchFacingDirection();

	FighterPawnRef->SetCurrentAction("Skid");

	const float InitialSpeed = FMath::Max(FMath::Abs(MoveComp->Velocity.X), 1.f);
	const float EndSpeed = 5.f;

	SkidFriction = FMath::Pow(EndSpeed / InitialSpeed, 1.0f / Duration);
	SkidFriction = FMath::Clamp(SkidFriction, 0.90f, 0.99f);

	InitialSkidDirection = (MoveComp->Velocity.X > 0.f) ? 1.f : -1.f;
}

void USkidState::Tick()
{
	Super::Tick();

	if (SkidFriction > 0.f)
	{
		FVector Velocity = MoveComp->GetVelocity();
		Velocity.X *= SkidFriction;

		if (FMath::Abs(Velocity.X) < 1.f) Velocity.X = 0.f;

		MoveComp->SetVelocity(Velocity);
	}

	MoveComp->HandleLedgeOrFall(true);

	if (StateMachine->FramesInState == Duration)
	{
		const int8 StickX = FighterPawnRef->GetPlayerStickInput().X;

		if (StickX * InitialSkidDirection < 0) StateMachine->TryChangeState("Dash");
		else
		{
			FighterPawnRef->SetCurrentAction("Idle", 3);
			StateMachine->TryChangeState("Idle");
		}
	}
}

bool USkidState::JumpPressed()
{
	return StateMachine->TryChangeState("JumpSquat");
}