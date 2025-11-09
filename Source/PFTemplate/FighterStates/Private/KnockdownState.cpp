#include "KnockdownState.h"

void UKnockdownState::OnEnter()
{
	
}

void UKnockdownState::Tick()
{
	Super::Tick();
}

bool UKnockdownState::Attack()
{
	return false;
}

bool UKnockdownState::ShieldPressed()
{
	return false;
}

void UKnockdownState::OnExit()
{
	Super::OnExit();
}
