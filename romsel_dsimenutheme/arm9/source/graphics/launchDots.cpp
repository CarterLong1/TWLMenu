#include "launchDots.h"
#include "ThemeTextures.h"
#include "common/gl2d.h"
#include <algorithm>

// F(p, t) = (R(t), p*pi/6 + Vt)
// 32 degrees = 3276 brads

// Spacing between dots in binary radians 
#define DOT_INTERVAL 2730

// Initial radius of the circle
#define DOT_INIT_RADIUS 36

// 90 degrees in binary radians
#define BRAD_90_DEG 8192

// number of timesteps to animate
#define DOTS_NUM_TIMESTEPS 64

inline int getRadius(int frame) {
     return DOT_INIT_RADIUS + std::min(2 * frame, 16); 
}

inline int getVelocity(int frame) {
	if (frame < 16)
		return 0;
	return 182 * frame; // 182 - 2 degrees of rotation at frame.
}

inline int getDotRadiusFrame(int dotIndex, int frame) {
	return 5;
	// return ((frame % 4) + dotIndex) % 5; return 5
}

inline int getDotX(int dotIndex, int frame) {
	return getRadius(frame) *
	       fixedToFloat(cosLerp((dotIndex * DOT_INTERVAL) - BRAD_90_DEG - (frame * getVelocity(frame))), 12);
}

inline int getDotY(int dotIndex, int frame) {
	return getRadius(frame) *
	       fixedToFloat(sinLerp((dotIndex * DOT_INTERVAL) - BRAD_90_DEG - (frame * getVelocity(frame))), 12);
}

static int radFrame = 0;

void LaunchDots::drawFrame(int frame) {
	for (int i = 0; i < 12; i++) {
		int X = getDotX(i, frame);
		int Y = getDotY(i, frame);
		int dotFrame = getDotRadiusFrame(i, frame);
		if (dotFrame == -1)
			continue;
		glSprite((128 - (getRadius(0) >> 2)) + X, (96 + (getRadius(0) >> 2) + (getRadius(0) >> 3)) + Y,
			 GL_FLIP_NONE, &tex().launchdotImage()[dotFrame & 15]);
	}
}
void LaunchDots::drawAuto() {
    drawFrame(radFrame);
	if (radFrame < DOTS_NUM_TIMESTEPS) {
		radFrame++;
	} else {
		radFrame = 0;
	}
}