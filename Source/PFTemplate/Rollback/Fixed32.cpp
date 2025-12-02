#include "Fixed32.h"
#include "Logging/LogMacros.h"

FFixed_32 FFixed_32::FromUnits(int32_t whole, uint32_t frac)
{
	int64_t w = (int64_t)whole * ONE;
	checkSlow(whole == 0 || w / whole == ONE);  // overflow guard
	return FFixed_32(w + frac);
}

int32_t FFixed_32::GetWhole() const
{
	return static_cast<int32_t>(v >> 32);
}

uint32_t FFixed_32::GetFrac() const
{
	return static_cast<uint32_t>(v & 0xFFFFFFFFu);
}

FFixed_32 FFixed_32::operator*(FFixed_32 o) const
{
	return FFixed_32((int64_t)(((__int128_t)v * (__int128_t)o.v) >> 32));
}

FFixed_32 FFixed_32::operator/(FFixed_32 o) const
{
	checkSlow(o.v != 0);
	return FFixed_32((int64_t)(((__int128_t)v << 32) / (__int128_t)o.v));
}

FFixed_32 FFixed_32::Sqrt() const
{
	if (v <= 0) return FFixed_32(0);
	uint64_t r = (uint64_t)v;
	uint64_t b = 1ULL << 62;
	while (b > r) b >>= 2;
	uint64_t res = 0;
	while (b != 0)
	{
		if (r >= res + b) { r -= res + b; res = (res >> 1) + b; }
		else res >>= 1;
		b >>= 2;
	}
	return FFixed_32((int64_t)(res << 16));  // 32 frac bits → 16 after sqrt
}

FFixed_32 FFixed_32::MulHigh(FFixed_32 b) const
{
	/* portable 64×64→high-64 */
	uint64_t al = (uint64_t)(uint32_t)v;
	uint64_t ah = (uint64_t)(uint32_t)(v >> 32);
	uint64_t bl = (uint64_t)(uint32_t)b.v;
	uint64_t bh = (uint64_t)(uint32_t)(b.v >> 32);

	uint64_t lo = al * bl;
	uint64_t t  = ah * bl + ((lo >> 32) & 0xFFFFFFFFULL);
	uint64_t hi = ah * bh + (t >> 32);
	return FFixed_32((int64_t)hi);
}

FFixed_32 FFixed_32::poly5(FFixed_32 x2)
{
	static const FFixed_32 c1 = FromRaw(0xFFFFFFFFD5555555LL); // -1/6
	static const FFixed_32 c2 = FromRaw(0x0000000002222222LL); //  1/120
	static const FFixed_32 c3 = FromRaw(0xFFFFFFFFF2FF30LL);   // -1/5040

	FFixed_32 t = c3;
	t = t.MulHigh(x2) + c2;
	t = t.MulHigh(x2) + c1;
	t = t.MulHigh(x2) + FFixed_32(1.0f);   // 1.0 as float → Q32.32
	return t;
}

FFixed_32 FFixed_32::Sin() const
{
	FFixed_32 y = *this;
	
	int32_t n = (y * (FFixed_32(1) / (FFixed_32(2) * Fixed_Pi))).GetWhole();
	y = y - FFixed_32(n) * 2*Fixed_Pi;
	if (y >  Fixed_Pi) y = y - 2*Fixed_Pi;
	if (y < -Fixed_Pi) y = y + 2*Fixed_Pi;

	/* 2. sin(x) ≈ x * poly5(x²) */
	bool neg = y.v < 0;
	y = y.Abs();
	if (y > half_pi)
		y = Fixed_Pi - y;

	FFixed_32 x2 = y.MulHigh(y);
	return (neg ? -y : y).MulHigh(poly5(x2));
}

FFixed_32 FFixed_32::Cos() const
{
	return (*this + HALF_PI()).Sin();
}