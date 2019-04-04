/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mathutil/umath.h"
#include "mathutil/umath_float16_compressor.h"
#include "mathutil/umath_random.hpp"
#include <string>
#include <glm/glm.hpp>
#include <glm/detail/type_half.hpp>

#define SMODF(a,n) fmod((fmod(a,n) +n),n)

static std::random_device random_device;
static std::mt19937 random_generator(random_device());
std::mt19937 &umath::get_default_random_generator() {return random_generator;}

/* epsilon surrounding for near zero values */
const auto EQN_EPS = 1e-9;
bool umath::is_zero(double x)
{
	return ((x > -EQN_EPS && x < EQN_EPS)) ? true : false;
}

Int32 umath::random(Int32 min,Int32 max)
{
	if(max < min)
		return random(max,min);
	std::uniform_int_distribution<Int32> dis(min,max);
	return dis(random_generator);
}

Float umath::random(Float min,Float max)
{
	if(max < min)
		return random(max,min);
	std::uniform_real_distribution<Float> dis(min,max);
	return dis(random_generator);
}

Double umath::deg_to_rad(Double deg) {return (deg /180.0) *Double(M_PI);}
Double umath::rad_to_deg(Double rad) {return (rad *180.0) /Double(M_PI);}

Double umath::normalize_angle(Double ang,Double base)
{
	Double max = base +360;
	while(ang > max)
		ang -= 360;
	while(ang <= base)
		ang += 360;
	return ang;
}

Double umath::normalize_angle(Double ang)
{
	while(ang > 180)
		ang -= 360;
	while(ang <= -180)
		ang += 360;
	return ang;
}

Double umath::sin(Double x) {return ::sin(x);}
Double umath::cos(Double x) {return ::cos(x);}
Double umath::tan(Double x) {return ::tan(x);}
Double umath::asin(Double x)
{
	if(x > 1.0f)
		x = 1.0f;
	else if(x < -1.0f)
		x = -1.0f;
	return ::asin(x);
}
Double umath::acos(Double x)
{
	if(x > 1.0f)
		x = 1.0f;
	else if(x < -1.0f)
		x = -1.0f;
	return ::acos(x);
}
Double umath::atan2(Double y,Double x)
{
	if(y == 0.0f && x == 0.0f)
		return 0.0f;
	return ::atan2(y,x);
}
Double umath::lerp(Double start,Double end,Double percent)
{
	return (start +percent *(end -start));
}
Double umath::lerp_angle(Double angA,Double angB,Double amount)
{
	amount = get_angle_difference(angA,angB) *amount;
	return approach_angle(angA,angB,amount);
}
Double umath::clamp_angle(Double ang,Double min,Double max)
{
	auto nMin = normalize_angle(min -ang);
	auto nMax = normalize_angle(max -ang);

	if(nMin <= 0.f && nMax >= 0.f)
		return ang;
	if(abs(nMin) < abs(nMax))
		return min;
	return max;
}
bool umath::is_angle_in_range(Double ang,Double min,Double max)
{
	auto nMin = normalize_angle(min -ang);
	auto nMax = normalize_angle(max -ang);
	return (nMin <= 0.f && nMax >= 0.f) ? true : false;
}
Double umath::get_angle_difference(Double angA,Double angB)
{
	return SMODF((angB -angA) +180.f,360.f) -180.f;
}
Double umath::approach_angle(Double angA,Double angB,Double amount)
{
	Double diff = get_angle_difference(angA,angB);
	if(diff > amount)
		diff = amount;
	else if(diff < -amount)
		diff = -amount;
	return angA +diff;
}

Int32 umath::ceil(Float f) {return static_cast<Int32>(::ceilf(f));}
Int32 umath::ceil(Double d) {return static_cast<Int32>(::ceil(d));}
Int64 umath::ceil(LDouble d) {return static_cast<Int64>(::ceill(d));}
Int32 umath::floor(Float f) {return static_cast<Int32>(::floorf(f));}
Int32 umath::floor(Double d) {return static_cast<Int32>(::floor(d));}
Int64 umath::floor(LDouble d) {return static_cast<Int64>(::floorl(d));}
Int32 umath::round(Float f) {return static_cast<Int32>(::roundf(f));}
Int32 umath::round(Double d) {return static_cast<Int32>(::round(d));}
Int64 umath::round(LDouble d) {return static_cast<Int64>(::roundl(d));}

Double umath::round(Double v,Int32 to)
{
    Double places = pow(10.0,Double(to));
    return ::round(v *places) /places;
}

UInt64 umath::next_power_of_2(UInt64 v)
{
	UInt64 powerOf2 = 1;
	if(v > 0)
	{
		while(v)
		{
			v >>= 1;
			powerOf2 <<= 1;
		}
	}
	return powerOf2;
}

UInt64 umath::previous_power_of_2(UInt64 v)
{
	UInt64 powerOf2 = 1;
	if(v > 1)
	{
		v--;
		while(v)
		{
			v >>= 1;
			powerOf2 <<= 1;
		}
		powerOf2 >>= 1;
	}
	return powerOf2;
}

std::vector<UInt64> umath::get_power_of_2_values(UInt64 v)
{
	std::vector<UInt64> values;
	auto next = next_power_of_2(v) /2;
	while(v > 0 && next > 0)
	{
		if((v &next) != 0)
		{
			values.push_back(next);
			v &= ~next;
		}
		next /= 2;
	}
	return values;
}

UInt32 umath::get_number_of_decimals(Float f)
{
	f -= floor(f);
	auto str = std::to_string(f);
	auto p = str.find_first_of('.');
	if(p == size_t(-1))
		return 0;
	auto len = str.length();
	UInt32 numTrailingZeroes = 0;
	while(len > 0 && str[len -1] == '0')
	{
		numTrailingZeroes++;
		len--;
	}
	str = str.substr(0,str.length() -numTrailingZeroes);
	return CUInt32(str.length() -p -1);
}

Int8 umath::sign(Int32 i) {return (i >= 0) ? 1 : -1;}
Int8 umath::sign(Float f) {return (f >= 0) ? 1 : -1;}

Int32 umath::snap_to_grid(Float f,UInt32 gridSize)
{
	if(gridSize == 0)
		return 0;
	auto s = sign(f);
	auto r = static_cast<Int32>(f *s);
	auto d = r %gridSize;
	if(d < (CFloat(gridSize) *0.5f))
		r -= d;
	else
		r += gridSize -d;
	return r *s;
}

UInt8 umath::get_highest_bit(UInt8 n)
{
	n |= (n >> 1);
	n |= (n >> 2);
	n |= (n >> 4);
	return n -(n >> 1);
}
UInt16 umath::get_highest_bit(UInt16 n)
{
	n |= (n >> 1);
	n |= (n >> 2);
	n |= (n >> 4);
	n |= (n >> 8);
	return n -(n >> 1);
}
UInt32 umath::get_highest_bit(UInt32 n)
{
	n |= (n >> 1);
	n |= (n >> 2);
	n |= (n >> 4);
	n |= (n >> 8);
	n |= (n >> 16);
	return n -(n >> 1);
}
UInt64 umath::get_highest_bit(UInt64 n)
{
	n |= (n >> 1);
	n |= (n >> 2);
	n |= (n >> 4);
	n |= (n >> 8);
	n |= (n >> 16);
	n |= (n >> 32);
	return n -(n >> 1);
}

float umath::sqrt(float v) {return sqrtf(v);}
double umath::sqrt(double v) {return ::sqrt(v);}
long double umath::sqrt(long double v) {return sqrtl(v);}

uint16_t umath::float32_to_float16(float f) {return Float16Compressor::compress(f);}
float umath::float16_to_float32(uint16_t v) {return Float16Compressor::decompress(v);}

int16_t umath::float32_to_float16_glm(float f) {return glm::detail::toFloat16(f);}
float umath::float16_to_float32_glm(int16_t v) {return glm::detail::toFloat32(v);}