// Footsteps
// Main Source

// Includes
#include <stdfix.h>
#include "defs.h"
#include "util.h"
#include "main.h"

// Variables
t_player player[MAX_PLAYERS];

// Functions
bool KeyDown (int key)
{
	int buttons = ACS_GetPlayerInput(-1, INPUT_BUTTONS);
	if (buttons & key)
		return true;
	return false;
}

// Scripts
ACS_S_SCRIPT ENTER void CameraMain (void)
{
	int plrHealth, camType, plrNum = ACS_PlayerNumber();
	accum plrAngle = ACS_GetActorAngle(0);
	player[plrNum].angle = plrAngle;
	
	if (!ACS_ActivatorTID())
		ACS_Thing_ChangeTID(0, ACS_UniqueTID(-32678, 0));
	player[plrNum].tid = ACS_ActivatorTID();
	player[plrNum].camTid = ACS_UniqueTID(-32768, 0);
	ACS_SpawnForced(s"ChaseCam", ACS_GetActorX(0), ACS_GetActorY(0), ACS_GetActorZ(0), player[plrNum].camTid, player[plrNum].angle);
	
	while (true)
	{
		plrHealth = ACS_GetActorProperty(0, APROP_Health);
		player[plrNum].angle += ACS_GetPlayerInputFixed(-1, INPUT_YAW);
		player[plrNum].pitch -= ACS_GetPlayerInputFixed(-1, INPUT_PITCH);
		
		if (player[plrNum].pitch > PITCH_LIMIT)
			player[plrNum].pitch = PITCH_LIMIT;
		if (player[plrNum].pitch < -PITCH_LIMIT)
			player[plrNum].pitch = -PITCH_LIMIT;
		
		ACS_LineAttack(0, player[plrNum].angle + 0.5k, -player[plrNum].pitch, 0, s"ChaseCamWaypoint", s"", ACS_GetCVarFixed(s"chase_dist"), FHF_NORANDOMPUFFZ);
		
		ACS_SetActorAngle(player[plrNum].camTid, player[plrNum].angle);
		ACS_SetActorPitch(player[plrNum].camTid, player[plrNum].pitch);
		
		camType = ACS_GetUserCVar(plrNum, s"chase_camtype");
		if (plrHealth)
		{
			switch (camType)
			{
				default:
				case 1:
					plrAngle = player[plrNum].angle;
				break;
				
				case 2:
					if (KeyDown(BT_FORWARD) || KeyDown(BT_BACK) || KeyDown(BT_MOVELEFT) || KeyDown(BT_MOVERIGHT) || KeyDown(BT_ATTACK) || KeyDown(BT_ALTATTACK))
						plrAngle = player[plrNum].angle;
				break;
				
				case 3:
					if (KeyDown(BT_FORWARD) || KeyDown(BT_BACK) || KeyDown(BT_MOVELEFT) || KeyDown(BT_MOVERIGHT))
						plrAngle = player[plrNum].angle;
				break;
			}
		}
		
		if (ACS_GetUserCVar(plrNum, s"chase_active"))
			ACS_ChangeCamera(player[plrNum].camTid, 0, 0);
		else
		{
			if (plrHealth)
				plrAngle = player[plrNum].angle;
			ACS_ChangeCamera(0, 0, 0);
		}
		
		ACS_SetActorAngle(0, plrAngle);
		ACS_SetActorPitch(0, player[plrNum].pitch);
		
		ACS_Delay(1);
	}
	
	return;
}

ACS_S_SCRIPT void WaypointSpawn (void)
{
	if (!ACS_ActivatorTID())
		ACS_Thing_ChangeTID(0, ACS_UniqueTID(-32768, 0));
	int tid = ACS_ActivatorTID();
	ACS_SetActivatorToTarget(0);
	int plrNum = ACS_PlayerNumber();
	if (plrNum != -1)
	{
		player[plrNum].wayTid = tid;
		ACS_SetActorAngle(tid, player[plrNum].angle);
		ACS_SetActorPitch(tid, player[plrNum].pitch);
		ACS_SetActorProperty(player[plrNum].camTid, APROP_MasterTID, tid);
		ACS_GiveActorInventory(player[plrNum].camTid, s"ChaseCamWarp", 1);
		ACS_Delay(1);
		ACS_Thing_Remove(tid);
	}
	return;
}

ACS_S_SCRIPT int GetChaseCamPos (int xyz)
{
	int plrNum = -1;
	accum vz, sz, h, ch, d;
	
	// Hacky, but I can't think of another way.
	// Iterate through camera TIDs to find our player number.
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (player[i].camTid == ACS_ActivatorTID())
			plrNum = i;
	}
	
	if (plrNum == -1)
		return 0;
	
	if (xyz == 0)
		return (int)ACS_GetActorX(player[plrNum].wayTid);
		
	if (xyz == 1)
		return (int)ACS_GetActorY(player[plrNum].wayTid);
	
	if (xyz == 2)
	{
		d = ACS_GetCVarFixed(s"chase_dist");
		h = ACS_GetActorPropertyFixed(player[plrNum].tid, APROP_Height);
		ch = ACS_GetCVarFixed(s"chase_height");
		vz = ACS_Sin(player[plrNum].pitch);
		sz = h + ch;

		return (int)(ACS_GetActorZ(player[plrNum].tid) + sz + (vz * d));
	}
}