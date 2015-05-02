// Footsteps
// Main Source

// Includes
#include <stdio.h>
#include <stdfix.h>
#include <string.h>
#include <stdlib.h>
#include "defs.h"
#include "util.h"
#include "main.h"

// Function
int ZanSqrt (int number)
{
	if (number <= 3)
		return number > 0;
	
	int oldAns = number >> 1,
	newAns = (oldAns + number / oldAns) >> 1;
	
	while (newAns < oldAns)
	{
		oldAns = newAns;
		newAns = (oldAns + number / oldAns) >> 1;
	}
	
	return oldAns;
}

// Scripts
Z_SCRIPT(SCR_FOOTSTEPS) ENTER void Footsteps (void)
{
	// Variables
	char *flat, *token;
	__str langdef, sound;
	accum volume, vx, vy, speed;
	int delay;
	
	while (true)
	{
		if (!(ACS_GetActorZ(0) - ACS_GetActorFloorZ(0)))
		{
			flat = ACSStrToChar(ACS_StrParam("%LS", s"STEP_FLATS"));
			token = strtok(flat, ":");
			
			vx = ACS_GetActorVelX(0);
			vy = ACS_GetActorVelY(0);
			#ifdef ZAN_ACS
			// Zandronum doesn't support FixedSqrt yet, so
			// we'll need to use a custom function.
			speed = ((accum)ZanSqrt(vx * vx + vy * vy)) / 16k;
			#else
			speed = ACS_FixedSqrt(vx * vx + vy * vy) / 16k;
			#endif
			
			if (speed > 1k)
				speed = 1k;
			
			delay = (35k - (25 * speed)) * ACS_GetCVarFixed(s"fs_delay_mul");
			volume = speed * ACS_GetCVarFixed(s"fs_volume_mul");
			
			while (token != NULL)
			{
				if (ACS_CheckActorFloorTexture(0, ctoacstr(token)))
				{
					langdef = ACS_StrParam("STEP_%S", ctoacstr(token));
					sound = ACS_StrParam("%LS", langdef);
					if (!ACS_StrICmp(langdef, sound))
						sound = ACS_StrParam("%LS", s"STEP_DEFAULT");
				}
				token = strtok(NULL, ":");
			}
			
			ACS_ActivatorSound(sound, (127k * volume) >> 16);
			sound = ACS_StrParam("%LS", s"STEP_DEFAULT");
		}
		if (delay > 1)
			ACS_Delay(delay);
		else
			ACS_Delay(1);
	}
}