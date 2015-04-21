#ifndef INC_MAIN_H_
#define INC_MAIN_H_

// Footsteps
// Main Header

// Defines
#define PITCH_LIMIT 0.25k

// Structs
typedef struct t_player
{
	int tid, camTid, wayTid;
	accum angle, pitch;
} t_player;

// Functions
bool KeyDown (int key);

// Scripts
ACS_S_SCRIPT ENTER void CameraMain (void);
ACS_S_SCRIPT void WaypointSpawn (void);
ACS_S_SCRIPT int GetChaseCamPos (int xyz);

#endif
