// SPDX-License-Identifier: MIT

// Mathematical operators that don't reuse C++'s behavior

#include "opmath.hpp"

#include <stdint.h>

int32_t op_divide(int32_t dividend, int32_t divisor) {
	// Adjust division to floor toward negative infinity,
	// not truncate toward zero
	return dividend / divisor - ((dividend % divisor < 0) != (divisor < 0));
}

int32_t op_modulo(int32_t dividend, int32_t divisor) {
	int32_t remainder = dividend % divisor;

	// Adjust modulo to have the sign of the divisor,
	// not the sign of the dividend
	return remainder + divisor * ((remainder < 0) != (divisor < 0));
}

int32_t op_exponent(int32_t base, uint32_t power) {
	uint32_t result = 1;

	for (uint32_t ubase = base; power; power /= 2) {
		if (power % 2) {
			result = static_cast<uint64_t>(result) * ubase;
		}
		ubase = static_cast<uint64_t>(ubase) * ubase;
	}

	// Avoid arithmetic overflow runtime error
	return result <= INT32_MAX ? result : -static_cast<int32_t>(~result) - 1;
}

int32_t op_shift_left(int32_t value, int32_t amount) {
	// Get the easy cases out of the way
	if (amount == 0) {
		return value;
	}
	if (value == 0 || amount >= 32) {
		return 0;
	}
	if (amount < -31) {
		return (value < 0) ? -1 : 0;
	}
	if (amount < 0) {
		return op_shift_right(value, -amount);
	}

	// Use unsigned to force a bitwise shift
	// Casting back is OK because the types implement two's complement behavior
	return static_cast<uint32_t>(value) << amount;
}

int32_t op_shift_right(int32_t value, int32_t amount) {
	// Repeat the easy cases here to avoid INT_MIN funny business
	if (amount == 0) {
		return value;
	}
	if (value == 0 || amount < -31) {
		return 0;
	}
	if (amount > 31) {
		return (value < 0) ? -1 : 0;
	}
	if (amount < 0) {
		return op_shift_left(value, -amount);
	}

	if (value > 0) {
		return static_cast<uint32_t>(value) >> amount;
	}

	// Calculate an OR mask for sign extension
	// 1->0x80000000, 2->0xC0000000, ..., 31->0xFFFFFFFE
	uint32_t amount_high_bits = -(UINT32_C(1) << (32 - amount));

	// The C++ standard leaves shifting right negative values
	// undefined, so use a left shift manually sign-extended
	return (static_cast<uint32_t>(value) >> amount) | amount_high_bits;
}

int32_t op_shift_right_unsigned(int32_t value, int32_t amount) {
	// Repeat the easy cases here to avoid INT_MIN funny business
	if (amount == 0) {
		return value;
	}
	if (value == 0 || amount < -31 || amount > 31) {
		return 0;
	}
	if (amount < 0) {
		return op_shift_left(value, -amount);
	}

	return static_cast<uint32_t>(value) >> amount;
}
