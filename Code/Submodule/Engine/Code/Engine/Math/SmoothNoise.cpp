//-----------------------------------------------------------------------------------------------
// SmoothNoise.cpp
//
#include "Engine/Math/SmoothNoise.hpp"
#include "Engine/Math/RawNoise.hpp"			// for raw bit-noise base functions (SquirrelNoise4)
#include "Engine/Math/MathUtils.hpp"	// for SmoothStep3(); see "SmoothStep" on Wikipedia
#include "Engine/Math/Vec2.hpp"			// for Vec2( float x,y ) class/struct
#include "Engine/Math/Vec3.hpp"			// for Vec3( float x,y,z ) class/struct
#include "Engine/Math/Vec4.hpp"			// for Vec4( float x,y,z,w ) class/struct

/////////////////////////////////////////////////////////////////////////////////////////////////
// For all fractal (and Perlin) noise functions, the following internal naming conventions
//	are used, primarily to help me visualize 3D and 4D constructs clearly.  They need not
//	have any actual bearing on / relationship to actual external coordinate systems.
//
// 1D noise: only X (+east / -west)
// 2D noise: also Y (+north / -south)
// 3D noise: also Z (+above / -below)
// 4D noise: also T (+after / -before)
/////////////////////////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------------------------
float Compute1DFractalNoise(const float position, const float scale, const unsigned int num_octaves,
                            const float octave_persistence, const float octave_scale, const bool renormalize,
                            unsigned int seed)
{
	const float octave_offset = 0.636764989593174f; // Translation/bias to add to each octave

	float total_noise = 0.f;
	float total_amplitude = 0.f;
	float current_amplitude = 1.f;
	float current_position = position * (1.f / scale);

	for (unsigned int octave_num = 0; octave_num < num_octaves; ++ octave_num)
	{
		// Determine noise values at nearby integer "grid point" positions
		const float position_floor = floorf(current_position);
		const int index_west = static_cast<int>(position_floor);
		const int index_east = index_west + 1;
		const float value_west = Get1DNoiseZeroToOne(index_west, seed);
		const float value_east = Get1DNoiseZeroToOne(index_east, seed);

		// Do a smoothed (nonlinear) weighted average of nearby grid point values
		const float distance_from_west = current_position - position_floor;
		const float weight_east = SmoothStep3(distance_from_west); // Gives rounder (nonlinear) results
		const float weight_west = 1.f - weight_east;
		const float noise_zero_to_one = (value_west * weight_west) + (value_east * weight_east);
		const float noise_this_octave = 2.f * (noise_zero_to_one - 0.5f); // Map from [0,1] to [-1,1]

		// Accumulate results and prepare for next octave (if any)
		total_noise += noise_this_octave * current_amplitude;
		total_amplitude += current_amplitude;
		current_amplitude *= octave_persistence;
		current_position *= octave_scale;
		current_position += octave_offset; // Add "irrational" offset to de-align octave grids
		++ seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if (renormalize && total_amplitude > 0.f)
	{
		total_noise /= total_amplitude; // Amplitude exceeds 1.0 if octaves are used!
		total_noise = (total_noise * 0.5f) + 0.5f; // Map to [0,1]
		total_noise = SmoothStep3(total_noise); // Push towards extents (octaves pull us away)
		total_noise = (total_noise * 2.0f) - 1.f; // Map back to [-1,1]
	}

	return total_noise;
}


//-----------------------------------------------------------------------------------------------
float Compute2DFractalNoise(const float pos_x, const float pos_y, const float scale, const unsigned int num_octaves,
                            const float octave_persistence, const float octave_scale, const bool renormalize,
                            unsigned int seed)
{
	const float octave_offset = 0.636764989593174f; // Translation/bias to add to each octave

	float total_noise = 0.f;
	float total_amplitude = 0.f;
	float current_amplitude = 1.f;
	const float inv_scale = (1.f / scale);
	Vec2 current_pos(pos_x * inv_scale, pos_y * inv_scale);

	for (unsigned int octave_num = 0; octave_num < num_octaves; ++ octave_num)
	{
		// Determine noise values at nearby integer "grid point" positions
		Vec2 cell_mins(floorf(current_pos.x), floorf(current_pos.y));
		const int index_west_x = static_cast<int>(cell_mins.x);
		const int index_south_y = static_cast<int>(cell_mins.y);
		const int index_east_x = index_west_x + 1;
		const int index_north_y = index_south_y + 1;
		const float value_south_west = Get2DNoiseZeroToOne(index_west_x, index_south_y, seed);
		const float value_south_east = Get2DNoiseZeroToOne(index_east_x, index_south_y, seed);
		const float value_north_west = Get2DNoiseZeroToOne(index_west_x, index_north_y, seed);
		const float value_north_east = Get2DNoiseZeroToOne(index_east_x, index_north_y, seed);

		// Do a smoothed (nonlinear) weighted average of nearby grid point values
		const Vec2 displacement_from_mins = current_pos - cell_mins;
		const float weight_east = SmoothStep3(displacement_from_mins.x);
		const float weight_north = SmoothStep3(displacement_from_mins.y);
		const float weight_west = 1.f - weight_east;
		const float weight_south = 1.f - weight_north;

		const float blend_south = (weight_east * value_south_east) + (weight_west * value_south_west);
		const float blend_north = (weight_east * value_north_east) + (weight_west * value_north_west);
		const float blend_total = (weight_south * blend_south) + (weight_north * blend_north);
		const float noise_this_octave = 2.f * (blend_total - 0.5f); // Map from [0,1] to [-1,1]

		// Accumulate results and prepare for next octave (if any)
		total_noise += noise_this_octave * current_amplitude;
		total_amplitude += current_amplitude;
		current_amplitude *= octave_persistence;
		current_pos *= octave_scale;
		current_pos.x += octave_offset; // Add "irrational" offsets to noise position components
		current_pos.y += octave_offset; //	at each octave to break up their grid alignment
		++ seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if (renormalize && total_amplitude > 0.f)
	{
		total_noise /= total_amplitude; // Amplitude exceeds 1.0 if octaves are used
		total_noise = (total_noise * 0.5f) + 0.5f; // Map to [0,1]
		total_noise = SmoothStep3(total_noise); // Push towards extents (octaves pull us away)
		total_noise = (total_noise * 2.0f) - 1.f; // Map back to [-1,1]
	}

	return total_noise;
}


//-----------------------------------------------------------------------------------------------
float Compute3DFractalNoise(const float pos_x, const float pos_y, const float pos_z, const float scale,
                            const unsigned int num_octaves, const float octave_persistence, const float octave_scale,
                            const bool renormalize, unsigned int seed)
{
	const float octave_offset = 0.636764989593174f; // Translation/bias to add to each octave

	float total_noise = 0.f;
	float total_amplitude = 0.f;
	float current_amplitude = 1.f;
	const float inv_scale = (1.f / scale);
	Vec3 current_pos(pos_x * inv_scale, pos_y * inv_scale, pos_z * inv_scale);

	for (unsigned int octave_num = 0; octave_num < num_octaves; ++ octave_num)
	{
		// Determine noise values at nearby integer "grid point" positions
		Vec3 cell_mins(floorf(current_pos.x), floorf(current_pos.y), floorf(current_pos.z));
		const int index_west_x = static_cast<int>(cell_mins.x);
		const int index_south_y = static_cast<int>(cell_mins.y);
		const int index_below_z = static_cast<int>(cell_mins.z);
		const int index_east_x = index_west_x + 1;
		const int index_north_y = index_south_y + 1;
		const int index_above_z = index_below_z + 1;

		// Noise grid cell has 8 corners in 3D
		const float above_south_west = Get3DNoiseZeroToOne(index_west_x, index_south_y, index_above_z, seed);
		const float above_south_east = Get3DNoiseZeroToOne(index_east_x, index_south_y, index_above_z, seed);
		const float above_north_west = Get3DNoiseZeroToOne(index_west_x, index_north_y, index_above_z, seed);
		const float above_north_east = Get3DNoiseZeroToOne(index_east_x, index_north_y, index_above_z, seed);
		const float below_south_west = Get3DNoiseZeroToOne(index_west_x, index_south_y, index_below_z, seed);
		const float below_south_east = Get3DNoiseZeroToOne(index_east_x, index_south_y, index_below_z, seed);
		const float below_north_west = Get3DNoiseZeroToOne(index_west_x, index_north_y, index_below_z, seed);
		const float below_north_east = Get3DNoiseZeroToOne(index_east_x, index_north_y, index_below_z, seed);

		// Do a smoothed (nonlinear) weighted average of nearby grid point values
		const Vec3 displacement_from_mins = current_pos - cell_mins;

		const float weight_east = SmoothStep3(displacement_from_mins.x);
		const float weight_north = SmoothStep3(displacement_from_mins.y);
		const float weight_above = SmoothStep3(displacement_from_mins.z);
		const float weight_west = 1.f - weight_east;
		const float weight_south = 1.f - weight_north;
		const float weight_below = 1.f - weight_above;

		// 8-way blend (8 -> 4 -> 2 -> 1)
		const float blend_below_south = (weight_east * below_south_east) + (weight_west * below_south_west);
		const float blend_below_north = (weight_east * below_north_east) + (weight_west * below_north_west);
		const float blend_above_south = (weight_east * above_south_east) + (weight_west * above_south_west);
		const float blend_above_north = (weight_east * above_north_east) + (weight_west * above_north_west);
		const float blend_below = (weight_south * blend_below_south) + (weight_north * blend_below_north);
		const float blend_above = (weight_south * blend_above_south) + (weight_north * blend_above_north);
		const float blend_total = (weight_below * blend_below) + (weight_above * blend_above);
		const float noise_this_octave = 2.f * (blend_total - 0.5f); // Map from [0,1] to [-1,1]

		// Accumulate results and prepare for next octave (if any)
		total_noise += noise_this_octave * current_amplitude;
		total_amplitude += current_amplitude;
		current_amplitude *= octave_persistence;
		current_pos *= octave_scale;
		current_pos.x += octave_offset; // Add "irrational" offsets to noise position components
		current_pos.y += octave_offset; //	at each octave to break up their grid alignment
		current_pos.z += octave_offset;
		++ seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if (renormalize && total_amplitude > 0.f)
	{
		total_noise /= total_amplitude; // Amplitude exceeds 1.0 if octaves are used
		total_noise = (total_noise * 0.5f) + 0.5f; // Map to [0,1]
		total_noise = SmoothStep3(total_noise); // Push towards extents (octaves pull us away)
		total_noise = (total_noise * 2.0f) - 1.f; // Map back to [-1,1]
	}

	return total_noise;
}


//-----------------------------------------------------------------------------------------------
float Compute4DFractalNoise(const float pos_x, const float pos_y, const float pos_z, const float pos_t,
                            const float scale, const unsigned int num_octaves, const float octave_persistence,
                            const float octave_scale, const bool renormalize, unsigned int seed)
{
	const float octave_offset = 0.636764989593174f; // Translation/bias to add to each octave

	float total_noise = 0.f;
	float total_amplitude = 0.f;
	float current_amplitude = 1.f;
	const float inv_scale = (1.f / scale);
	Vec4 current_pos(pos_x * inv_scale, pos_y * inv_scale, pos_z * inv_scale, pos_t * inv_scale);

	for (unsigned int octave_num = 0; octave_num < num_octaves; ++ octave_num)
	{
		// Determine noise values at nearby integer "grid point" positions
		Vec4 cell_mins(floorf(current_pos.x), floorf(current_pos.y), floorf(current_pos.z), floorf(current_pos.w));
		const int index_west_x = static_cast<int>(cell_mins.x);
		const int index_south_y = static_cast<int>(cell_mins.y);
		const int index_below_z = static_cast<int>(cell_mins.z);
		const int index_before_t = static_cast<int>(cell_mins.w);
		const int index_east_x = index_west_x + 1;
		const int index_north_y = index_south_y + 1;
		const int index_above_z = index_below_z + 1;
		const int index_after_t = index_before_t + 1;

		// Noise grid cell has 16 "corners" in 4D
		const float before_below_sw = Get4DNoiseZeroToOne(index_west_x, index_south_y, index_below_z, index_before_t,
		                                                  seed);
		const float before_below_se = Get4DNoiseZeroToOne(index_east_x, index_south_y, index_below_z, index_before_t,
		                                                  seed);
		const float before_below_nw = Get4DNoiseZeroToOne(index_west_x, index_north_y, index_below_z, index_before_t,
		                                                  seed);
		const float before_below_ne = Get4DNoiseZeroToOne(index_east_x, index_north_y, index_below_z, index_before_t,
		                                                  seed);
		const float before_above_sw = Get4DNoiseZeroToOne(index_west_x, index_south_y, index_above_z, index_before_t,
		                                                  seed);
		const float before_above_se = Get4DNoiseZeroToOne(index_east_x, index_south_y, index_above_z, index_before_t,
		                                                  seed);
		const float before_above_nw = Get4DNoiseZeroToOne(index_west_x, index_north_y, index_above_z, index_before_t,
		                                                  seed);
		const float before_above_ne = Get4DNoiseZeroToOne(index_east_x, index_north_y, index_above_z, index_before_t,
		                                                  seed);

		const float after_below_sw = Get4DNoiseZeroToOne(index_west_x, index_south_y, index_below_z, index_after_t,
		                                                 seed);
		const float after_below_se = Get4DNoiseZeroToOne(index_east_x, index_south_y, index_below_z, index_after_t,
		                                                 seed);
		const float after_below_nw = Get4DNoiseZeroToOne(index_west_x, index_north_y, index_below_z, index_after_t,
		                                                 seed);
		const float after_below_ne = Get4DNoiseZeroToOne(index_east_x, index_north_y, index_below_z, index_after_t,
		                                                 seed);
		const float after_above_sw = Get4DNoiseZeroToOne(index_west_x, index_south_y, index_above_z, index_after_t,
		                                                 seed);
		const float after_above_se = Get4DNoiseZeroToOne(index_east_x, index_south_y, index_above_z, index_after_t,
		                                                 seed);
		const float after_above_nw = Get4DNoiseZeroToOne(index_west_x, index_north_y, index_above_z, index_after_t,
		                                                 seed);
		const float after_above_ne = Get4DNoiseZeroToOne(index_east_x, index_north_y, index_above_z, index_after_t,
		                                                 seed);

		// Do a smoothed (nonlinear) weighted average of nearby grid point values
		const Vec4 displacement_from_mins = current_pos - cell_mins;

		const float weight_east = SmoothStep3(displacement_from_mins.x);
		const float weight_north = SmoothStep3(displacement_from_mins.y);
		const float weight_above = SmoothStep3(displacement_from_mins.z);
		const float weight_after = SmoothStep3(displacement_from_mins.w);
		const float weight_west = 1.f - weight_east;
		const float weight_south = 1.f - weight_north;
		const float weight_below = 1.f - weight_above;
		const float weight_before = 1.f - weight_after;

		// 16-way blend (16 -> 8 -> 4 -> 2 -> 1)
		const float blend_before_below_south = (weight_east * before_below_se) + (weight_west * before_below_sw);
		const float blend_before_below_north = (weight_east * before_below_ne) + (weight_west * before_below_nw);
		const float blend_before_above_south = (weight_east * before_above_se) + (weight_west * before_above_sw);
		const float blend_before_above_north = (weight_east * before_above_ne) + (weight_west * before_above_nw);
		const float blend_after_below_south = (weight_east * after_below_se) + (weight_west * after_below_sw);
		const float blend_after_below_north = (weight_east * after_below_ne) + (weight_west * after_below_nw);
		const float blend_after_above_south = (weight_east * after_above_se) + (weight_west * after_above_sw);
		const float blend_after_above_north = (weight_east * after_above_ne) + (weight_west * after_above_nw);
		const float blend_before_below = (weight_south * blend_before_below_south) + (weight_north *
			blend_before_below_north);
		const float blend_before_above = (weight_south * blend_before_above_south) + (weight_north *
			blend_before_above_north);
		const float blend_after_below = (weight_south * blend_after_below_south) + (weight_north *
			blend_after_below_north);
		const float blend_after_above = (weight_south * blend_after_above_south) + (weight_north *
			blend_after_above_north);
		const float blend_before = (weight_below * blend_before_below) + (weight_above * blend_before_above);
		const float blend_after = (weight_below * blend_after_below) + (weight_above * blend_after_above);
		const float blend_total = (weight_before * blend_before) + (weight_after * blend_after);
		const float noise_this_octave = 2.f * (blend_total - 0.5f); // Map from [0,1] to [-1,1]

		// Accumulate results and prepare for next octave (if any)
		total_noise += noise_this_octave * current_amplitude;
		total_amplitude += current_amplitude;
		current_amplitude *= octave_persistence;
		current_pos *= octave_scale;
		current_pos.x += octave_offset; // Add "irrational" offsets to noise position components
		current_pos.y += octave_offset; //	at each octave to break up their grid alignment
		current_pos.z += octave_offset;
		current_pos.w += octave_offset;
		++ seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if (renormalize && total_amplitude > 0.f)
	{
		total_noise /= total_amplitude; // Amplitude exceeds 1.0 if octaves are used
		total_noise = (total_noise * 0.5f) + 0.5f; // Map to [0,1]
		total_noise = SmoothStep3(total_noise); // Push towards extents (octaves pull us away)
		total_noise = (total_noise * 2.0f) - 1.f; // Map back to [-1,1]
	}

	return total_noise;
}


//-----------------------------------------------------------------------------------------------
// Perlin noise is fractal noise with "gradient vector smoothing" applied.
//
// In 1D, the gradients are trivial: -1.0 or 1.0, so resulting noise is boring at one octave.
//
float Compute1DPerlinNoise(const float position, const float scale, const unsigned int num_octaves,
                           const float octave_persistence,
                           const float octave_scale, const bool renormalize, unsigned int seed)
{
	const float octave_offset = 0.636764989593174f; // Translation/bias to add to each octave
	const float gradients[2] = {-1.f, 1.f}; // 1D unit "gradient" vectors; one back, one forward

	float total_noise = 0.f;
	float total_amplitude = 0.f;
	float current_amplitude = 1.f;
	float current_position = position * (1.f / scale);

	for (unsigned int octave_num = 0; octave_num < num_octaves; ++ octave_num)
	{
		// Determine random "gradient vectors" (just +1 or -1 for 1D Perlin) for surrounding corners
		const float position_floor = static_cast<float>(floorf(current_position));
		const int index_west = static_cast<int>(position_floor);
		const int index_east = index_west + 1;
		const float gradient_west = gradients[Get1DNoiseUint(index_west, seed) & 0x00000001];
		const float gradient_east = gradients[Get1DNoiseUint(index_east, seed) & 0x00000001];

		// Dot each point's gradient with displacement from point to position
		const float displacement_from_west = current_position - position_floor; // always positive
		const float displacement_from_east = displacement_from_west - 1.f; // always negative
		const float dot_west = gradient_west * displacement_from_west; // 1D "dot product" is... multiply
		const float dot_east = gradient_east * displacement_from_east;

		// Do a smoothed (nonlinear) weighted average of dot results
		const float weight_east = SmoothStep3(displacement_from_west);
		const float weight_west = 1.f - weight_east;
		const float blend_total = (weight_west * dot_west) + (weight_east * dot_east);
		const float noise_this_octave = 2.f * blend_total; // 1D Perlin is in [-.5,.5]; map to [-1,1]

		// Accumulate results and prepare for next octave (if any)
		total_noise += noise_this_octave * current_amplitude;
		total_amplitude += current_amplitude;
		current_amplitude *= octave_persistence;
		current_position *= octave_scale;
		current_position += octave_offset; // Add "irrational" offset to de-align octave grids
		++ seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if (renormalize && total_amplitude > 0.f)
	{
		total_noise /= total_amplitude; // Amplitude exceeds 1.0 if octaves are used
		total_noise = (total_noise * 0.5f) + 0.5f; // Map to [0,1]
		total_noise = SmoothStep3(total_noise); // Push towards extents (octaves pull us away)
		total_noise = (total_noise * 2.0f) - 1.f; // Map back to [-1,1]
	}

	return total_noise;
}


//-----------------------------------------------------------------------------------------------
// Perlin noise is fractal noise with "gradient vector smoothing" applied.
//
// In 2D, gradients are unit-length vectors in various directions with even angular distribution.
//
float Compute2DPerlinNoise(const float pos_x, const float pos_y, const float scale, const unsigned int num_octaves,
                           const float octave_persistence,
                           const float octave_scale, const bool renormalize, unsigned int seed)
{
	const float octave_offset = 0.636764989593174f; // Translation/bias to add to each octave
	const Vec2 gradients[ 8 ] = // Normalized unit vectors in 8 quarter-cardinal directions
	{
		Vec2(+0.923879533f, +0.382683432f), //  22.5 degrees (ENE)
		Vec2(+0.382683432f, +0.923879533f), //  67.5 degrees (NNE)
		Vec2(-0.382683432f, +0.923879533f), // 112.5 degrees (NNW)
		Vec2(-0.923879533f, +0.382683432f), // 157.5 degrees (WNW)
		Vec2(-0.923879533f, -0.382683432f), // 202.5 degrees (WSW)
		Vec2(-0.382683432f, -0.923879533f), // 247.5 degrees (SSW)
		Vec2(+0.382683432f, -0.923879533f), // 292.5 degrees (SSE)
		Vec2(+0.923879533f, -0.382683432f) // 337.5 degrees (ESE)
	};

	float total_noise = 0.f;
	float total_amplitude = 0.f;
	float current_amplitude = 1.f;
	const float inv_scale = (1.f / scale);
	Vec2 current_pos(pos_x * inv_scale, pos_y * inv_scale);

	for (unsigned int octave_num = 0; octave_num < num_octaves; ++ octave_num)
	{
		// Determine random unit "gradient vectors" for surrounding corners
		const Vec2 cell_mins(floorf(current_pos.x), floorf(current_pos.y));
		const Vec2 cell_maxs(cell_mins.x + 1.f, cell_mins.y + 1.f);
		const int index_west_x = static_cast<int>(cell_mins.x);
		const int index_south_y = static_cast<int>(cell_mins.y);
		const int index_east_x = index_west_x + 1;
		const int index_north_y = index_south_y + 1;

		const unsigned int noise_sw = Get2DNoiseUint(index_west_x, index_south_y, seed);
		const unsigned int noise_se = Get2DNoiseUint(index_east_x, index_south_y, seed);
		const unsigned int noise_nw = Get2DNoiseUint(index_west_x, index_north_y, seed);
		const unsigned int noise_ne = Get2DNoiseUint(index_east_x, index_north_y, seed);

		const Vec2& gradient_sw = gradients[noise_sw & 0x00000007];
		const Vec2& gradient_se = gradients[noise_se & 0x00000007];
		const Vec2& gradient_nw = gradients[noise_nw & 0x00000007];
		const Vec2& gradient_ne = gradients[noise_ne & 0x00000007];

		// Dot each corner's gradient with displacement from corner to position
		Vec2 displacement_from_sw(current_pos.x - cell_mins.x, current_pos.y - cell_mins.y);
		Vec2 displacement_from_se(current_pos.x - cell_maxs.x, current_pos.y - cell_mins.y);
		Vec2 displacement_from_nw(current_pos.x - cell_mins.x, current_pos.y - cell_maxs.y);
		Vec2 displacement_from_ne(current_pos.x - cell_maxs.x, current_pos.y - cell_maxs.y);

		const float dot_south_west = DotProduct(gradient_sw, displacement_from_sw);
		const float dot_south_east = DotProduct(gradient_se, displacement_from_se);
		const float dot_north_west = DotProduct(gradient_nw, displacement_from_nw);
		const float dot_north_east = DotProduct(gradient_ne, displacement_from_ne);

		// Do a smoothed (nonlinear) weighted average of dot results
		const float weight_east = SmoothStep3(displacement_from_sw.x);
		const float weight_north = SmoothStep3(displacement_from_sw.y);
		const float weight_west = 1.f - weight_east;
		const float weight_south = 1.f - weight_north;

		const float blend_south = (weight_east * dot_south_east) + (weight_west * dot_south_west);
		const float blend_north = (weight_east * dot_north_east) + (weight_west * dot_north_west);
		const float blend_total = (weight_south * blend_south) + (weight_north * blend_north);
		const float noise_this_octave = blend_total * (1.f / 0.662578106f);
		// 2D Perlin is in [-.662578106,.662578106]; map to ~[-1,1]

		// Accumulate results and prepare for next octave (if any)
		total_noise += noise_this_octave * current_amplitude;
		total_amplitude += current_amplitude;
		current_amplitude *= octave_persistence;
		current_pos *= octave_scale;
		current_pos.x += octave_offset; // Add "irrational" offset to de-align octave grids
		current_pos.y += octave_offset; // Add "irrational" offset to de-align octave grids
		++ seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if (renormalize && total_amplitude > 0.f)
	{
		total_noise /= total_amplitude; // Amplitude exceeds 1.0 if octaves are used
		total_noise = (total_noise * 0.5f) + 0.5f; // Map to [0,1]
		total_noise = SmoothStep3(total_noise); // Push towards extents (octaves pull us away)
		total_noise = (total_noise * 2.0f) - 1.f; // Map back to [-1,1]
	}

	return total_noise;
}


//-----------------------------------------------------------------------------------------------
// Perlin noise is fractal noise with "gradient vector smoothing" applied.
//
// In 3D, gradients are unit-length vectors in random (3D) directions.
//
float Compute3DPerlinNoise(const float pos_x, const float pos_y, const float pos_z, const float scale,
                           const unsigned int num_octaves,
                           const float octave_persistence, const float octave_scale, const bool renormalize,
                           unsigned int seed)
{
	const float octave_offset = 0.636764989593174f; // Translation/bias to add to each octave

	const Vec3 gradients[ 8 ] = // Traditional "12 edges" requires modulus and isn't any better.
	{
		Vec3(+F_SQRT_3_OVER_3, +F_SQRT_3_OVER_3, +F_SQRT_3_OVER_3), // Normalized unit 3D vectors
		Vec3(-F_SQRT_3_OVER_3, +F_SQRT_3_OVER_3, +F_SQRT_3_OVER_3), //  pointing toward cube
		Vec3(+F_SQRT_3_OVER_3, -F_SQRT_3_OVER_3, +F_SQRT_3_OVER_3), //  corners, so components
		Vec3(-F_SQRT_3_OVER_3, -F_SQRT_3_OVER_3, +F_SQRT_3_OVER_3), //  are all sqrt(3)/3, i.e.
		Vec3(+F_SQRT_3_OVER_3, +F_SQRT_3_OVER_3, -F_SQRT_3_OVER_3), // 0.5773502691896257645091f.
		Vec3(-F_SQRT_3_OVER_3, +F_SQRT_3_OVER_3, -F_SQRT_3_OVER_3), // These are slightly better
		Vec3(+F_SQRT_3_OVER_3, -F_SQRT_3_OVER_3, -F_SQRT_3_OVER_3), // than axes (1,0,0) and much
		Vec3(-F_SQRT_3_OVER_3, -F_SQRT_3_OVER_3, -F_SQRT_3_OVER_3) // faster than edges (1,1,0).
	};

	float total_noise = 0.f;
	float total_amplitude = 0.f;
	float current_amplitude = 1.f;
	const float inv_scale = (1.f / scale);
	Vec3 current_pos(pos_x * inv_scale, pos_y * inv_scale, pos_z * inv_scale);

	for (unsigned int octave_num = 0; octave_num < num_octaves; ++ octave_num)
	{
		// Determine random unit "gradient vectors" for surrounding corners
		const Vec3 cell_mins(floorf(current_pos.x), floorf(current_pos.y), floorf(current_pos.z));
		const Vec3 cell_maxs(cell_mins.x + 1.f, cell_mins.y + 1.f, cell_mins.z + 1.f);
		const int index_west_x = static_cast<int>(cell_mins.x);
		const int index_south_y = static_cast<int>(cell_mins.y);
		const int index_below_z = static_cast<int>(cell_mins.z);
		const int index_east_x = index_west_x + 1;
		const int index_north_y = index_south_y + 1;
		const int index_above_z = index_below_z + 1;

		const unsigned int noise_below_sw = Get3DNoiseUint(index_west_x, index_south_y, index_below_z, seed);
		const unsigned int noise_below_se = Get3DNoiseUint(index_east_x, index_south_y, index_below_z, seed);
		const unsigned int noise_below_nw = Get3DNoiseUint(index_west_x, index_north_y, index_below_z, seed);
		const unsigned int noise_below_ne = Get3DNoiseUint(index_east_x, index_north_y, index_below_z, seed);
		const unsigned int noise_above_sw = Get3DNoiseUint(index_west_x, index_south_y, index_above_z, seed);
		const unsigned int noise_above_se = Get3DNoiseUint(index_east_x, index_south_y, index_above_z, seed);
		const unsigned int noise_above_nw = Get3DNoiseUint(index_west_x, index_north_y, index_above_z, seed);
		const unsigned int noise_above_ne = Get3DNoiseUint(index_east_x, index_north_y, index_above_z, seed);

		Vec3 gradient_below_sw = gradients[noise_below_sw & 0x00000007];
		Vec3 gradient_below_se = gradients[noise_below_se & 0x00000007];
		Vec3 gradient_below_nw = gradients[noise_below_nw & 0x00000007];
		Vec3 gradient_below_ne = gradients[noise_below_ne & 0x00000007];
		Vec3 gradient_above_sw = gradients[noise_above_sw & 0x00000007];
		Vec3 gradient_above_se = gradients[noise_above_se & 0x00000007];
		Vec3 gradient_above_nw = gradients[noise_above_nw & 0x00000007];
		Vec3 gradient_above_ne = gradients[noise_above_ne & 0x00000007];

		// Dot each corner's gradient with displacement from corner to position
		Vec3 displacement_from_below_sw(current_pos.x - cell_mins.x, current_pos.y - cell_mins.y,
		                                current_pos.z - cell_mins.z);
		Vec3 displacement_from_below_se(current_pos.x - cell_maxs.x, current_pos.y - cell_mins.y,
		                                current_pos.z - cell_mins.z);
		Vec3 displacement_from_below_nw(current_pos.x - cell_mins.x, current_pos.y - cell_maxs.y,
		                                current_pos.z - cell_mins.z);
		Vec3 displacement_from_below_ne(current_pos.x - cell_maxs.x, current_pos.y - cell_maxs.y,
		                                current_pos.z - cell_mins.z);
		Vec3 displacement_from_above_sw(current_pos.x - cell_mins.x, current_pos.y - cell_mins.y,
		                                current_pos.z - cell_maxs.z);
		Vec3 displacement_from_above_se(current_pos.x - cell_maxs.x, current_pos.y - cell_mins.y,
		                                current_pos.z - cell_maxs.z);
		Vec3 displacement_from_above_nw(current_pos.x - cell_mins.x, current_pos.y - cell_maxs.y,
		                                current_pos.z - cell_maxs.z);
		Vec3 displacement_from_above_ne(current_pos.x - cell_maxs.x, current_pos.y - cell_maxs.y,
		                                current_pos.z - cell_maxs.z);

		const float dot_below_sw = DotProduct(gradient_below_sw, displacement_from_below_sw);
		const float dot_below_se = DotProduct(gradient_below_se, displacement_from_below_se);
		const float dot_below_nw = DotProduct(gradient_below_nw, displacement_from_below_nw);
		const float dot_below_ne = DotProduct(gradient_below_ne, displacement_from_below_ne);
		const float dot_above_sw = DotProduct(gradient_above_sw, displacement_from_above_sw);
		const float dot_above_se = DotProduct(gradient_above_se, displacement_from_above_se);
		const float dot_above_nw = DotProduct(gradient_above_nw, displacement_from_above_nw);
		const float dot_above_ne = DotProduct(gradient_above_ne, displacement_from_above_ne);

		// Do a smoothed (nonlinear) weighted average of dot results
		const float weight_east = SmoothStep3(displacement_from_below_sw.x);
		const float weight_north = SmoothStep3(displacement_from_below_sw.y);
		const float weight_above = SmoothStep3(displacement_from_below_sw.z);
		const float weight_west = 1.f - weight_east;
		const float weight_south = 1.f - weight_north;
		const float weight_below = 1.f - weight_above;

		// 8-way blend (8 -> 4 -> 2 -> 1)
		const float blend_below_south = (weight_east * dot_below_se) + (weight_west * dot_below_sw);
		const float blend_below_north = (weight_east * dot_below_ne) + (weight_west * dot_below_nw);
		const float blend_above_south = (weight_east * dot_above_se) + (weight_west * dot_above_sw);
		const float blend_above_north = (weight_east * dot_above_ne) + (weight_west * dot_above_nw);
		const float blend_below = (weight_south * blend_below_south) + (weight_north * blend_below_north);
		const float blend_above = (weight_south * blend_above_south) + (weight_north * blend_above_north);
		const float blend_total = (weight_below * blend_below) + (weight_above * blend_above);
		const float noise_this_octave = blend_total * (1.f / 0.793856621f);
		// 3D Perlin is in [-.793856621,.793856621]; map to ~[-1,1]

		// Accumulate results and prepare for next octave (if any)
		total_noise += noise_this_octave * current_amplitude;
		total_amplitude += current_amplitude;
		current_amplitude *= octave_persistence;
		current_pos *= octave_scale;
		current_pos.x += octave_offset; // Add "irrational" offset to de-align octave grids
		current_pos.y += octave_offset; // Add "irrational" offset to de-align octave grids
		current_pos.z += octave_offset; // Add "irrational" offset to de-align octave grids
		++ seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if (renormalize && total_amplitude > 0.f)
	{
		total_noise /= total_amplitude; // Amplitude exceeds 1.0 if octaves are used
		total_noise = (total_noise * 0.5f) + 0.5f; // Map to [0,1]
		total_noise = SmoothStep3(total_noise); // Push towards extents (octaves pull us away)
		total_noise = (total_noise * 2.0f) - 1.f; // Map back to [-1,1]
	}

	return total_noise;
}


//-----------------------------------------------------------------------------------------------
// Perlin noise is fractal noise with "gradient vector smoothing" applied.
//
// In 4D, gradients are unit-length hyper-vectors in random (4D) directions.
//
float Compute4DPerlinNoise(const float pos_x, const float pos_y, const float pos_z, const float pos_t,
                           const float scale, const unsigned int num_octaves,
                           const float octave_persistence, const float octave_scale, const bool renormalize,
                           unsigned int seed)
{
	const float octave_offset = 0.636764989593174f; // Translation/bias to add to each octave

	const Vec4 gradients[ 16 ] = // Hard to tell if this is any better in 4D than just having 8
	{
		Vec4(+0.5f, +0.5f, +0.5f, +0.5f), // Normalized unit 4D vectors pointing toward each
		Vec4(-0.5f, +0.5f, +0.5f, +0.5f), //  of the 16 hypercube corners, so components are
		Vec4(+0.5f, -0.5f, +0.5f, +0.5f), //  all sqrt(4)/4, i.e. one-half.
		Vec4(-0.5f, -0.5f, +0.5f, +0.5f), // 
		Vec4(+0.5f, +0.5f, -0.5f, +0.5f), // It's hard to tell whether these are any better
		Vec4(-0.5f, +0.5f, -0.5f, +0.5f), //  or worse than vectors facing axes (1,0,0,0) or
		Vec4(+0.5f, -0.5f, -0.5f, +0.5f), //  3D edges (.7,.7,0,0) or 4D edges (.57,.57,.57,0)
		Vec4(-0.5f, -0.5f, -0.5f, +0.5f), //  but less-axial gradients looked a little better
		Vec4(+0.5f, +0.5f, +0.5f, -0.5f), //  with 2D and 3D noise so I'm assuming this is as
		Vec4(-0.5f, +0.5f, +0.5f, -0.5f), //  good or better as any other gradient-selection
		Vec4(+0.5f, -0.5f, +0.5f, -0.5f), //  scheme (and is crazy-fast).  *shrug*
		Vec4(-0.5f, -0.5f, +0.5f, -0.5f), // 
		Vec4(+0.5f, +0.5f, -0.5f, -0.5f), // Plus, we want a power-of-two number of evenly-
		Vec4(-0.5f, +0.5f, -0.5f, -0.5f), //  distributed gradients, so we can cheaply select
		Vec4(+0.5f, -0.5f, -0.5f, -0.5f), //  one from bit-noise (use bit-mask, not modulus).
		Vec4(-0.5f, -0.5f, -0.5f, -0.5f) // 
	};

	float total_noise = 0.f;
	float total_amplitude = 0.f;
	float current_amplitude = 1.f;
	const float inv_scale = (1.f / scale);
	Vec4 current_pos(pos_x * inv_scale, pos_y * inv_scale, pos_z * inv_scale, pos_t * inv_scale);

	for (unsigned int octave_num = 0; octave_num < num_octaves; ++ octave_num)
	{
		// Determine random unit "gradient vectors" for 16 surrounding 4D (hypercube) cell corners
		const Vec4 cell_mins(floorf(current_pos.x), floorf(current_pos.y), floorf(current_pos.z),
		                     floorf(current_pos.w));
		const Vec4 cell_maxs(cell_mins.x + 1.f, cell_mins.y + 1.f, cell_mins.z + 1.f, cell_mins.w + 1.f);
		const int index_west_x = static_cast<int>(cell_mins.x);
		const int index_south_y = static_cast<int>(cell_mins.y);
		const int index_below_z = static_cast<int>(cell_mins.z);
		const int index_before_t = static_cast<int>(cell_mins.w);
		const int index_east_x = index_west_x + 1;
		const int index_north_y = index_south_y + 1;
		const int index_above_z = index_below_z + 1;
		const int index_after_t = index_before_t + 1;

		// "BeforeBSW" stands for "BeforeBelowSouthWest" below (i.e. 4D hypercube mins), etc.
		const unsigned int noise_before_bsw = Get4DNoiseUint(index_west_x, index_south_y, index_below_z, index_before_t,
		                                                     seed);
		const unsigned int noise_before_bse = Get4DNoiseUint(index_east_x, index_south_y, index_below_z, index_before_t,
		                                                     seed);
		const unsigned int noise_before_bnw = Get4DNoiseUint(index_west_x, index_north_y, index_below_z, index_before_t,
		                                                     seed);
		const unsigned int noise_before_bne = Get4DNoiseUint(index_east_x, index_north_y, index_below_z, index_before_t,
		                                                     seed);
		const unsigned int noise_before_asw = Get4DNoiseUint(index_west_x, index_south_y, index_above_z, index_before_t,
		                                                     seed);
		const unsigned int noise_before_ase = Get4DNoiseUint(index_east_x, index_south_y, index_above_z, index_before_t,
		                                                     seed);
		const unsigned int noise_before_anw = Get4DNoiseUint(index_west_x, index_north_y, index_above_z, index_before_t,
		                                                     seed);
		const unsigned int noise_before_ane = Get4DNoiseUint(index_east_x, index_north_y, index_above_z, index_before_t,
		                                                     seed);
		const unsigned int noise_after_bsw = Get4DNoiseUint(index_west_x, index_south_y, index_below_z, index_after_t,
		                                                    seed);
		const unsigned int noise_after_bse = Get4DNoiseUint(index_east_x, index_south_y, index_below_z, index_after_t,
		                                                    seed);
		const unsigned int noise_after_bnw = Get4DNoiseUint(index_west_x, index_north_y, index_below_z, index_after_t,
		                                                    seed);
		const unsigned int noise_after_bne = Get4DNoiseUint(index_east_x, index_north_y, index_below_z, index_after_t,
		                                                    seed);
		const unsigned int noise_after_asw = Get4DNoiseUint(index_west_x, index_south_y, index_above_z, index_after_t,
		                                                    seed);
		const unsigned int noise_after_ase = Get4DNoiseUint(index_east_x, index_south_y, index_above_z, index_after_t,
		                                                    seed);
		const unsigned int noise_after_anw = Get4DNoiseUint(index_west_x, index_north_y, index_above_z, index_after_t,
		                                                    seed);
		const unsigned int noise_after_ane = Get4DNoiseUint(index_east_x, index_north_y, index_above_z, index_after_t,
		                                                    seed);

		// Mask with 15 (mod 16) to look up in gradients table
		Vec4 gradient_before_bsw = gradients[noise_before_bsw & 0x0000000F];
		Vec4 gradient_before_bse = gradients[noise_before_bse & 0x0000000F];
		Vec4 gradient_before_bnw = gradients[noise_before_bnw & 0x0000000F];
		Vec4 gradient_before_bne = gradients[noise_before_bne & 0x0000000F];
		Vec4 gradient_before_asw = gradients[noise_before_asw & 0x0000000F];
		Vec4 gradient_before_ase = gradients[noise_before_ase & 0x0000000F];
		Vec4 gradient_before_anw = gradients[noise_before_anw & 0x0000000F];
		Vec4 gradient_before_ane = gradients[noise_before_ane & 0x0000000F];
		Vec4 gradient_after_bsw = gradients[noise_after_bsw & 0x0000000F];
		Vec4 gradient_after_bse = gradients[noise_after_bse & 0x0000000F];
		Vec4 gradient_after_bnw = gradients[noise_after_bnw & 0x0000000F];
		Vec4 gradient_after_bne = gradients[noise_after_bne & 0x0000000F];
		Vec4 gradient_after_asw = gradients[noise_after_asw & 0x0000000F];
		Vec4 gradient_after_ase = gradients[noise_after_ase & 0x0000000F];
		Vec4 gradient_after_anw = gradients[noise_after_anw & 0x0000000F];
		Vec4 gradient_after_ane = gradients[noise_after_ane & 0x0000000F];

		// Dot each corner's gradient with displacement from corner to position
		Vec4 displacement_from_before_bsw(current_pos.x - cell_mins.x, current_pos.y - cell_mins.y,
		                                  current_pos.z - cell_mins.z,
		                                  current_pos.w - cell_mins.w);
		Vec4 displacement_from_before_bse(current_pos.x - cell_maxs.x, current_pos.y - cell_mins.y,
		                                  current_pos.z - cell_mins.z,
		                                  current_pos.w - cell_mins.w);
		Vec4 displacement_from_before_bnw(current_pos.x - cell_mins.x, current_pos.y - cell_maxs.y,
		                                  current_pos.z - cell_mins.z,
		                                  current_pos.w - cell_mins.w);
		Vec4 displacement_from_before_bne(current_pos.x - cell_maxs.x, current_pos.y - cell_maxs.y,
		                                  current_pos.z - cell_mins.z,
		                                  current_pos.w - cell_mins.w);
		Vec4 displacement_from_before_asw(current_pos.x - cell_mins.x, current_pos.y - cell_mins.y,
		                                  current_pos.z - cell_maxs.z,
		                                  current_pos.w - cell_mins.w);
		Vec4 displacement_from_before_ase(current_pos.x - cell_maxs.x, current_pos.y - cell_mins.y,
		                                  current_pos.z - cell_maxs.z,
		                                  current_pos.w - cell_mins.w);
		Vec4 displacement_from_before_anw(current_pos.x - cell_mins.x, current_pos.y - cell_maxs.y,
		                                  current_pos.z - cell_maxs.z,
		                                  current_pos.w - cell_mins.w);
		Vec4 displacement_from_before_ane(current_pos.x - cell_maxs.x, current_pos.y - cell_maxs.y,
		                                  current_pos.z - cell_maxs.z,
		                                  current_pos.w - cell_mins.w);
		Vec4 displacement_from_after_bsw(current_pos.x - cell_mins.x, current_pos.y - cell_mins.y,
		                                 current_pos.z - cell_mins.z,
		                                 current_pos.w - cell_maxs.w);
		Vec4 displacement_from_after_bse(current_pos.x - cell_maxs.x, current_pos.y - cell_mins.y,
		                                 current_pos.z - cell_mins.z,
		                                 current_pos.w - cell_maxs.w);
		Vec4 displacement_from_after_bnw(current_pos.x - cell_mins.x, current_pos.y - cell_maxs.y,
		                                 current_pos.z - cell_mins.z,
		                                 current_pos.w - cell_maxs.w);
		Vec4 displacement_from_after_bne(current_pos.x - cell_maxs.x, current_pos.y - cell_maxs.y,
		                                 current_pos.z - cell_mins.z,
		                                 current_pos.w - cell_maxs.w);
		Vec4 displacement_from_after_asw(current_pos.x - cell_mins.x, current_pos.y - cell_mins.y,
		                                 current_pos.z - cell_maxs.z,
		                                 current_pos.w - cell_maxs.w);
		Vec4 displacement_from_after_ase(current_pos.x - cell_maxs.x, current_pos.y - cell_mins.y,
		                                 current_pos.z - cell_maxs.z,
		                                 current_pos.w - cell_maxs.w);
		Vec4 displacement_from_after_anw(current_pos.x - cell_mins.x, current_pos.y - cell_maxs.y,
		                                 current_pos.z - cell_maxs.z,
		                                 current_pos.w - cell_maxs.w);
		Vec4 displacement_from_after_ane(current_pos.x - cell_maxs.x, current_pos.y - cell_maxs.y,
		                                 current_pos.z - cell_maxs.z,
		                                 current_pos.w - cell_maxs.w);

		const float dot_before_bsw = DotProduct(gradient_before_bsw, displacement_from_before_bsw);
		const float dot_before_bse = DotProduct(gradient_before_bse, displacement_from_before_bse);
		const float dot_before_bnw = DotProduct(gradient_before_bnw, displacement_from_before_bnw);
		const float dot_before_bne = DotProduct(gradient_before_bne, displacement_from_before_bne);
		const float dot_before_asw = DotProduct(gradient_before_asw, displacement_from_before_asw);
		const float dot_before_ase = DotProduct(gradient_before_ase, displacement_from_before_ase);
		const float dot_before_anw = DotProduct(gradient_before_anw, displacement_from_before_anw);
		const float dot_before_ane = DotProduct(gradient_before_ane, displacement_from_before_ane);
		const float dot_after_bsw = DotProduct(gradient_after_bsw, displacement_from_after_bsw);
		const float dot_after_bse = DotProduct(gradient_after_bse, displacement_from_after_bse);
		const float dot_after_bnw = DotProduct(gradient_after_bnw, displacement_from_after_bnw);
		const float dot_after_bne = DotProduct(gradient_after_bne, displacement_from_after_bne);
		const float dot_after_asw = DotProduct(gradient_after_asw, displacement_from_after_asw);
		const float dot_after_ase = DotProduct(gradient_after_ase, displacement_from_after_ase);
		const float dot_after_anw = DotProduct(gradient_after_anw, displacement_from_after_anw);
		const float dot_after_ane = DotProduct(gradient_after_ane, displacement_from_after_ane);

		// Do a smoothed (nonlinear) weighted average of dot results
		const float weight_east = SmoothStep3(displacement_from_before_bsw.x);
		const float weight_north = SmoothStep3(displacement_from_before_bsw.y);
		const float weight_above = SmoothStep3(displacement_from_before_bsw.z);
		const float weight_after = SmoothStep3(displacement_from_before_bsw.w);
		const float weight_west = 1.f - weight_east;
		const float weight_south = 1.f - weight_north;
		const float weight_below = 1.f - weight_above;
		const float weight_before = 1.f - weight_after;

		// 16-way blend (16 -> 8 -> 4 -> 2 -> 1)
		const float blend_before_below_south = (weight_east * dot_before_bse) + (weight_west * dot_before_bsw);
		const float blend_before_below_north = (weight_east * dot_before_bne) + (weight_west * dot_before_bnw);
		const float blend_before_above_south = (weight_east * dot_before_ase) + (weight_west * dot_before_asw);
		const float blend_before_above_north = (weight_east * dot_before_ane) + (weight_west * dot_before_anw);
		const float blend_after_below_south = (weight_east * dot_after_bse) + (weight_west * dot_after_bsw);
		const float blend_after_below_north = (weight_east * dot_after_bne) + (weight_west * dot_after_bnw);
		const float blend_after_above_south = (weight_east * dot_after_ase) + (weight_west * dot_after_asw);
		const float blend_after_above_north = (weight_east * dot_after_ane) + (weight_west * dot_after_anw);
		const float blend_before_below = (weight_south * blend_before_below_south) + (weight_north *
			blend_before_below_north);
		const float blend_before_above = (weight_south * blend_before_above_south) + (weight_north *
			blend_before_above_north);
		const float blend_after_below = (weight_south * blend_after_below_south) + (weight_north *
			blend_after_below_north);
		const float blend_after_above = (weight_south * blend_after_above_south) + (weight_north *
			blend_after_above_north);
		const float blend_before = (weight_below * blend_before_below) + (weight_above * blend_before_above);
		const float blend_after = (weight_below * blend_after_below) + (weight_above * blend_after_above);
		const float blend_total = (weight_before * blend_before) + (weight_after * blend_after);
		const float noise_this_octave = blend_total * (1.f / 0.6875f); // 4D Perlin is in [-.6875,.6875]; map to ~[-1,1]

		// Accumulate results and prepare for next octave (if any)
		total_noise += noise_this_octave * current_amplitude;
		total_amplitude += current_amplitude;
		current_amplitude *= octave_persistence;
		current_pos *= octave_scale;
		current_pos.x += octave_offset; // Add "irrational" offset to de-align octave grids
		current_pos.y += octave_offset; // Add "irrational" offset to de-align octave grids
		current_pos.z += octave_offset; // Add "irrational" offset to de-align octave grids
		current_pos.w += octave_offset; // Add "irrational" offset to de-align octave grids
		++ seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if (renormalize && total_amplitude > 0.f)
	{
		total_noise /= total_amplitude; // Amplitude exceeds 1.0 if octaves are used
		total_noise = (total_noise * 0.5f) + 0.5f; // Map to [0,1]
		total_noise = SmoothStep3(total_noise); // Push towards extents (octaves pull us away)
		total_noise = (total_noise * 2.0f) - 1.f; // Map back to [-1,1]
	}

	return total_noise;
}
