#include "Fixed32.h"

const int64_t FFixed_32::SIN_TABLE[91] = {
	0LL, 74957514LL, 149892196LL, 224781220LL,
	299601773LL, 374331064LL, 448946331LL, 523424844LL,
	597743917LL, 671880911LL, 745813244LL, 819518394LL,
	892973912LL, 966157421LL, 1039046629LL, 1111619334LL,
	1183853428LL, 1255726910LL, 1327217884LL, 1398304576LL,
	1468965330LL, 1539178623LL, 1608923067LL, 1678177418LL,
	1746920580LL, 1815131612LL, 1882789738LL, 1949874349LL,
	2016365008LL, 2082241463LL, 2147483647LL, 2212071687LL,
	2275985909LL, 2339206843LL, 2401715232LL, 2463492035LL,
	2524518435LL, 2584775842LL, 2644245901LL, 2702910498LL,
	2760751761LL, 2817752073LL, 2873894071LL, 2929160652LL,
	2983534983LL, 3037000499LL, 3089540917LL, 3141140230LL,
	3191782721LL, 3241452965LL, 3290135830LL, 3337816488LL,
	3384480415LL, 3430113397LL, 3474701532LL, 3518231240LL,
	3560689261LL, 3602062661LL, 3642338838LL, 3681505523LL,
	3719550786LL, 3756463038LL, 3792231035LL, 3826843881LL,
	3860291034LL, 3892562305LL, 3923647863LL, 3953538241LL,
	3982224332LL, 4009697399LL, 4035949074LL, 4060971359LL,
	4084756634LL, 4107297651LL, 4128587546LL, 4148619834LL,
	4167388411LL, 4184887562LL, 4201111955LL, 4216056649LL,
	4229717092LL, 4242089121LL, 4253168969LL, 4262953261LL,
	4271439015LL, 4278623648LL, 4284504971LL, 4289081192LL,
	4292350917LL, 4294313151LL, 4294967296LL
};

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
FFixed_32 FFixed_32::Sin() const
{
	// 1. Convert fixed point to absolute degrees (assuming this->Value is 32.32)
	// We want the integer part of degrees and the fractional part for interpolation
	int64_t rawValue = v; 
    
	// Normalize to [0, 360) degrees
	// (1LL << 32) * 360 is the value of 360.0 in 32.32
	int64_t fullCircle = 360LL << 32;
	rawValue %= fullCircle;
	if (rawValue < 0) rawValue += fullCircle;

	// 2. Determine Quadrant and map to 0-90
	int64_t angleIdx = rawValue >> 32;        // Integer degrees
	int64_t fraction = rawValue & 0xFFFFFFFF; // Fractional part (0 to 1.0)
    
	int64_t result = 0;
	int sign = 1;

	if (angleIdx >= 270) {      // Quadrant 4
		sign = -1;
		angleIdx = 360 - angleIdx;
		// Invert fraction for reverse mapping
		if (fraction > 0) { angleIdx--; fraction = (1LL << 32) - fraction; }
	} else if (angleIdx >= 180) { // Quadrant 3
		sign = -1;
		angleIdx -= 180;
	} else if (angleIdx >= 90) {  // Quadrant 2
		angleIdx = 180 - angleIdx;
		if (fraction > 0) { angleIdx--; fraction = (1LL << 32) - fraction; }
	}

	// 3. Linear Interpolation
	// L(x) = y0 + (y1 - y0) * fraction
	int64_t y0 = SIN_TABLE[angleIdx];
	int64_t y1 = (angleIdx < 90) ? SIN_TABLE[angleIdx + 1] : y0;
    
	// Use 64-bit math for the interpolation product to avoid overflow
	int64_t diff = y1 - y0;
	int64_t interpolation = (diff * fraction) >> 32;
    
	result = (y0 + interpolation) * sign;

	return FFixed_32::FromRaw(result);
}

FFixed_32 FFixed_32::Cos() const
{
	// cos(x) = sin(x + 90)
	int64_t ninetyDegrees = 90LL << 32;
	FFixed_32 shiftedAngle = FFixed_32::FromRaw(v + ninetyDegrees);
	return shiftedAngle.Sin();
}