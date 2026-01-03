#pragma once

class AFighterPawn;

class IPoolable
{
public:
	virtual ~IPoolable() {}

	virtual void Initialize(AFighterPawn* InOwner) = 0;
	virtual bool IsActive() const = 0;
};