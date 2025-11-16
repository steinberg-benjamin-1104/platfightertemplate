#include "FixedVector2D.h"
#include "Logging/LogMacros.h"

constexpr int64_t ONE  = 1LL << 32;
constexpr int64_t HALF = 1LL << 31;

FIXED_32 FIXED_32::FromUnits(int32_t whole, uint32_t frac)
{
	int64_t w = (int64_t)whole * ONE;
	checkSlow(whole == 0 || w / whole == ONE);  // overflow guard
	return FIXED_32(w + frac);
}

int32_t FIXED_32::GetWhole() const
{
	return static_cast<int32_t>(v >> 32);
}

uint32_t FIXED_32::GetFrac() const
{
	return static_cast<uint32_t>(v & 0xFFFFFFFFu);
}

FIXED_32 FIXED_32::operator*(FIXED_32 o) const
{
	return FIXED_32((int64_t)(((__int128_t)v * (__int128_t)o.v) >> 32));
}

FIXED_32 FIXED_32::operator/(FIXED_32 o) const
{
	checkSlow(o.v != 0);
	return FIXED_32((int64_t)(((__int128_t)v << 32) / (__int128_t)o.v));
}

FIXED_32 FIXED_32::Sqrt() const
{
	if (v <= 0) return FIXED_32(0);
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
	return FIXED_32((int64_t)(res << 16));  // 32 frac bits → 16 after sqrt
}

FIXED_32 FIXED_32::MulHigh(FIXED_32 b) const
{
	/* portable 64×64→high-64 */
	uint64_t al = (uint64_t)(uint32_t)v;
	uint64_t ah = (uint64_t)(uint32_t)(v >> 32);
	uint64_t bl = (uint64_t)(uint32_t)b.v;
	uint64_t bh = (uint64_t)(uint32_t)(b.v >> 32);

	uint64_t lo = al * bl;
	uint64_t t  = ah * bl + ((lo >> 32) & 0xFFFFFFFFULL);
	uint64_t hi = ah * bh + (t >> 32);
	return FIXED_32((int64_t)hi);
}

FIXED_32 FIXED_32::poly5(FIXED_32 x2)
{
	// coefficients as Q32.32
	static const FIXED_32 c1(0xFFFFFFFFD5555555LL); // -1/6
	static const FIXED_32 c2(0x02222222LL);         //  1/120
	static const FIXED_32 c3(0xFFFFFFFFF2FF30LL);   // -1/5040
	
	FIXED_32 t = c3;
	t = t.MulHigh(x2) + c2;
	t = t.MulHigh(x2) + c1;
	t = t.MulHigh(x2) + FIXED_32(ONE); // 1.0 in Q32.32
	return t;
}

FIXED_32 FIXED_32::Sin() const
{
	FIXED_32 y = *this;

	/* 1. range reduce to [-π, π] */
	int32_t n = (y * INV_TWO_Pi()).GetWhole();
	y = y - FIXED_32(n) * TWO_Pi();
	if (y >  Pi()) y = y - TWO_Pi();
	if (y < -Pi()) y = y + TWO_Pi();

	/* 2. sin(x) ≈ x * poly5(x²) */
	bool neg = y.v < 0;
	y = y.Abs();
	if (y > HALF_Pi())
		y = Pi() - y;

	FIXED_32 x2 = y.MulHigh(y);
	return (neg ? -y : y).MulHigh(poly5(x2));
}

FIXED_32 FIXED_32::Cos() const
{
	return (*this + HALF_PI()).Sin();
}