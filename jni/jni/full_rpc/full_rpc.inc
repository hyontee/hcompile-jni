#define vNeon1             0
#define vNeon2             1
#define vNeon3             2
#define vTint              3
#define vSuspensionForce   4
#define vSuspensionBias    5
#define vWheelSize         6
#define vWheelAddFront     7
#define vDrift     16
#define vWheelAddRear      8
#define vVinyl             9
#define vHydraulic         10
#define vLaunchMode        11
#define vFuel              12
#define vHorn              13
#define vLightColor        14
#define vWindowsColor      15
#define vDifferential       17

/* --- êîä çàêîìåíòèğîâàííûé äîáàâëÿé êî âñåì new â .pwn
enum Wheel_Properties
{
	Radius,
	Width[2],
	Alignment[2],
	Departure[2],
}

enum NumberPlate_Info
{
	Type,
	Number[8],
	Region[3],
}

enum Veh_Info
{
	Float:vFuel,
	Float:vMileage,

	vLightColor,
	vHighLights,

	vNeon[3],

	vColorBody,
	vColorWheels,
	vWheel[Wheel_Properties],

	vClearance,
	vSeparate,

	vStrob,

	vHydraulics,

	vNumber[NumberPlate_Info],

	vTonerFront,
	vTonerRear,
	vTonerFrontSide,
	vTonerRearSide,

	
	vNitro,
	vLaunchControl,
	vHornSound,
	vExhaustSound,
	
	vVinyl,
	vVinylTexture[16],

	vSiren,
	vTurnSiren,
};
new vInfo[MAX_VEHICLES][Veh_Info];

*/
stock SetVehicleNeon(vehicleid, color1, color2, color3, setvalue=0) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
	if(setvalue)
	{
		vInfo[vehicleid][vNeon1] = color1;
		vInfo[vehicleid][vNeon2] = color2;
		vInfo[vehicleid][vNeon3] = color3;
	}
	foreach(new playerid: Player) SetVehicleNeonForPlayer(playerid, vehicleid, color1, color2, color3);
}

stock SetVehicleNeonForPlayer(playerid, vehicleid, color1, color2, color3) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
	new BitStream:bs = BS_New();
	BS_WriteValue(bs, PR_UINT16, vehicleid);
	BS_WriteValue(bs, PR_UINT8, 9);   

	BS_WriteValue(bs, PR_UINT32, color1);
	BS_WriteValue(bs, PR_UINT32, color2);
	BS_WriteValue(bs, PR_UINT32, color3);

	PR_SendRPC(bs, playerid, 167, PR_LOW_PRIORITY, PR_RELIABLE_ORDERED);
	BS_Delete(bs);
}

CMD:neon(playerid, params[])
{
    new vehicleid = GetPlayerVehicleID(playerid);
    if(!vehicleid) return SendClientMessage(playerid, -1, !"Òû íå â ìàøèíå!");

    new color = 0xFFFF0000;

    SetVehicleNeon(vehicleid, color, color, color, 1);
    SendClientMessage(playerid, -1, !"Êğàñíûé íåîí âêëş÷åí!");
    return 1;
}

stock SetVehicleTint(vehicleid, level, setvalue=0) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
    if(setvalue)
    {
        vInfo[vehicleid][vTint] = level;
    }
    foreach(new playerid: Player) SetVehicleTintForPlayer(playerid, vehicleid, level);
}

stock SetVehicleTintForPlayer(playerid, vehicleid, level) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
    new BitStream:bs = BS_New();
    BS_WriteValue(bs, PR_UINT16, vehicleid);
    BS_WriteValue(bs, PR_UINT8, 10);   

    BS_WriteValue(bs, PR_UINT8, level); 

    PR_SendRPC(bs, playerid, 167, PR_LOW_PRIORITY, PR_RELIABLE_ORDERED);
    BS_Delete(bs);
}

CMD:tint(playerid, params[])
{
    new level;
    if(sscanf(params, "i", level)) 
        return SendClientMessage(playerid, -1, !"Èñïîëüçóé: /tint [óğîâåíü 0-3]");

    if(level < 0 || level > 3) 
        return SendClientMessage(playerid, -1, !"Óğîâåíü ìîæåò áûòü îò 0 (áåç) äî 3 (ìàêñ)");

    new vehicleid = GetPlayerVehicleID(playerid);
    if(!vehicleid) return SendClientMessage(playerid, -1, !"Òû íå â ìàøèíå!");

    SetVehicleTint(vehicleid, level, 1);
    new msg[64];
    format(msg, sizeof(msg), !"Òîíèğîâêà óñòàíîâëåíà íà óğîâåíü %d", level);
    SendClientMessage(playerid, -1, msg);
    return 1;
}

stock SetVehicleVinyl(vehicleid, newvinylid, setvalue=0) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
    if(setvalue)
    {
        vInfo[vehicleid][vVinyl] = newvinylid;
    }
    foreach(new playerid: Player) SetVehicleVinylForPlayer(playerid, vehicleid, newvinylid);
}

stock SetVehicleVinylForPlayer(playerid, vehicleid, newvinylid) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
    new BitStream:bs = BS_New();
    BS_WriteValue(bs, PR_UINT16, vehicleid);
    BS_WriteValue(bs, PR_UINT8, 12);   

    BS_WriteValue(bs, PR_UINT8, newvinylid); 

    PR_SendRPC(bs, playerid, 167, PR_LOW_PRIORITY, PR_RELIABLE_ORDERED);
    BS_Delete(bs);
}


CMD:vinyl(playerid, params[])
{
    new vehicleid = GetPlayerVehicleID(playerid);
    if(!vehicleid) return SendClientMessage(playerid, -1, !"Òû íå â ìàøèíå!");

    new vinylid = 1; 
    SetVehicleVinyl(vehicleid, vinylid, 1);

    SendClientMessage(playerid, -1, !"Âèíèë óñòàíîâëåí!");
    return 1;
}


stock SetVehicleSuspensionForce(vehicleid, force, setvalue=0) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
    if(setvalue)
    {
        vInfo[vehicleid][vSuspensionForce] = force;
    }
    foreach(new playerid: Player) SetVehicleSuspensionForceForPlayer(playerid, vehicleid, force);
}

stock SetVehicleSuspensionForceForPlayer(playerid, vehicleid, force) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
    new BitStream:bs = BS_New();
    BS_WriteValue(bs, PR_UINT16, vehicleid);
    BS_WriteValue(bs, PR_UINT8, 1); 
    BS_WriteValue(bs, PR_FLOAT, float(force));
    PR_SendRPC(bs, playerid, 167, PR_LOW_PRIORITY, PR_RELIABLE_ORDERED);
    BS_Delete(bs);
}


stock SetVehicleSuspensionBias(vehicleid, bias, setvalue=0) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
    if(setvalue)
    {
        vInfo[vehicleid][vSuspensionBias] = bias;
    }
    foreach(new playerid: Player) SetVehicleSuspensionBiasForPlayer(playerid, vehicleid, bias);
}

stock SetVehicleSuspensionBiasForPlayer(playerid, vehicleid, bias) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
    new BitStream:bs = BS_New();
    BS_WriteValue(bs, PR_UINT16, vehicleid);
    BS_WriteValue(bs, PR_UINT8, 2); 
    BS_WriteValue(bs, PR_FLOAT, float(bias));
    PR_SendRPC(bs, playerid, 167, PR_LOW_PRIORITY, PR_RELIABLE_ORDERED);
    BS_Delete(bs);
}

CMD:suspension(playerid, params[])
{
    new value;
    if(sscanf(params, "i", value)) return SendClientMessage(playerid, -1, !"Èñïîëüçóé: /suspension [çíà÷åíèå]");
    new vehicleid = GetPlayerVehicleID(playerid);
    if(!vehicleid) return SendClientMessage(playerid, -1, !"Òû íå â ìàøèíå!");
    SetVehicleSuspensionForce(vehicleid, value, 1);
    SendClientMessage(playerid, -1, !"Ñèëà ïîäâåñêè óñòàíîâëåíà!");
    return 1;
}

CMD:suspbias(playerid, params[])
{
    new value;
    if(sscanf(params, "i", value)) return SendClientMessage(playerid, -1, !"Èñïîëüçóé: /suspbias [çíà÷åíèå]");
    new vehicleid = GetPlayerVehicleID(playerid);
    if(!vehicleid) return SendClientMessage(playerid, -1, !"Òû íå â ìàøèíå!");
    SetVehicleSuspensionBias(vehicleid, value, 1);
    SendClientMessage(playerid, -1, !"Ñìåùåíèå ïîäâåñêè óñòàíîâëåíî!");
    return 1;
}


stock SetVehicleWheelSize(vehicleid, size, setvalue=0) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
    if(setvalue)
    {
        vInfo[vehicleid][vWheelSize] = size;
    }
    foreach(new playerid: Player) SetVehicleWheelSizeForPlayer(playerid, vehicleid, size);
}

stock SetVehicleWheelSizeForPlayer(playerid, vehicleid, size) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
    new BitStream:bs = BS_New();
    BS_WriteValue(bs, PR_UINT16, vehicleid);
    BS_WriteValue(bs, PR_UINT8, 3); 
    BS_WriteValue(bs, PR_FLOAT, float(size));
    PR_SendRPC(bs, playerid, 167, PR_LOW_PRIORITY, PR_RELIABLE_ORDERED);
    BS_Delete(bs);
}


stock SetVehicleWheelAdditive(vehicleid, front, rear, setvalue=0) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
    if(setvalue)
    {
        vInfo[vehicleid][vWheelAddFront] = front;
        vInfo[vehicleid][vWheelAddRear] = rear;
    }
    foreach(new playerid: Player) SetVehicleWheelAdditiveForPlayer(playerid, vehicleid, front, rear);
}

stock SetVehicleWheelAdditiveForPlayer(playerid, vehicleid, front, rear) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
    new BitStream:bs = BS_New();
    BS_WriteValue(bs, PR_UINT16, vehicleid);
    BS_WriteValue(bs, PR_UINT8, 17); 
    BS_WriteValue(bs, PR_FLOAT, float(front));
    BS_WriteValue(bs, PR_FLOAT, float(rear));
    PR_SendRPC(bs, playerid, 167, PR_LOW_PRIORITY, PR_RELIABLE_ORDERED);
    BS_Delete(bs);
}

CMD:wheelsize(playerid, params[])
{
    new value;
    if(sscanf(params, "i", value)) return SendClientMessage(playerid, -1, !"Èñïîëüçóé: /wheelsize [çíà÷åíèå]");
    new vehicleid = GetPlayerVehicleID(playerid);
    if(!vehicleid) return SendClientMessage(playerid, -1, !"Òû íå â ìàøèíå!");
    SetVehicleWheelSize(vehicleid, value, 1);
    SendClientMessage(playerid, -1, !"Ğàçìåğ êîë¸ñ óñòàíîâëåí!");
    return 1;
}

CMD:wheeladd(playerid, params[])
{
    new front, rear;
    if(sscanf(params, "ii", front, rear)) return SendClientMessage(playerid, -1, !"Èñïîëüçóé: /wheeladd [ïåğåä] [çàä]");
    new vehicleid = GetPlayerVehicleID(playerid);
    if(!vehicleid) return SendClientMessage(playerid, -1, !"Òû íå â ìàøèíå!");
    SetVehicleWheelAdditive(vehicleid, front, rear, 1);
    SendClientMessage(playerid, -1, !"Äîïîëíèòåëüíûé ğàçìåğ êîë¸ñ óñòàíîâëåí!");
    return 1;
}



stock SetVehicleToner(vehicleid, front, rear, front_side, back_side, setvalue=0) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
    if(setvalue)
    {
        vInfo[vehicleid][vTonerFront]      = front;
        vInfo[vehicleid][vTonerRear]       = rear;
        vInfo[vehicleid][vTonerFrontSide]  = front_side;
        vInfo[vehicleid][vTonerRearSide]   = back_side;
    }

    
    new playerid;
    foreach(playerid : Player)
    {
        if(IsPlayerConnected(playerid))
            SetVehicleTonerForPlayer(playerid, vehicleid, front, rear, front_side, back_side);
    }
}

stock SetVehicleTonerForPlayer(playerid, vehicleid, front, rear, front_side, back_side) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
    new BitStream:bitstream = BS_New();
    BS_WriteValue(bitstream, PR_UINT16, vehicleid);
    BS_WriteValue(bitstream, PR_UINT8, 0); 

    BS_WriteValue(bitstream, PR_UINT32, front);
    BS_WriteValue(bitstream, PR_UINT32, rear);
    BS_WriteValue(bitstream, PR_UINT32, front_side);
    BS_WriteValue(bitstream, PR_UINT32, back_side); 

    PR_SendRPC(bitstream, playerid, 167, PR_LOW_PRIORITY, PR_RELIABLE_ORDERED);
    BS_Delete(bitstream);
}

CMD:toner(playerid, params[])
{
    new vehicleid = GetPlayerVehicleID(playerid);
    if(!vehicleid) return SendClientMessage(playerid, -1, !"Òû íå â ìàøèíå!");

    
    new front = 0xFF000000;  
    new rear = 0xFF000000;
    new front_side = 0xFF000000;
    new rear_side = 0xFF000000;

    SetVehicleToner(vehicleid, front, rear, front_side, rear_side, 1);
    SendClientMessage(playerid, -1, !"Òîíåğ óñòàíîâëåí: ÷åğíûé!");
    return 1;
}


stock PlayAudioStreamURL(playerid, const url[]) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
    new BitStream:bs = BS_New();
    new len = strlen(url);

    BS_WriteValue(bs, PR_INT32, 1);
    BS_WriteValue(bs, PR_UINT8, len);
    BS_WriteValue(bs, PR_STRING, url);
    BS_WriteValue(bs, PR_INT32, 0);
    BS_WriteValue(bs, PR_FLOAT, 1.0);
    BS_WriteValue(bs, PR_UINT8, 0);

    PR_SendRPC(bs, playerid, 41, PR_LOW_PRIORITY, PR_RELIABLE_ORDERED);
    BS_Delete(bs);
}


CMD:playaudio(playerid, params[])
{
    PlayAudioStreamURL(playerid, "http:") //Ñşäà ññûëêó íà ìóçûêó
    SendClientMessage(playerid, 0x00FF00FF, "Ïğîèãğûâàåòñÿ ğàäèî DFM!");
    return 1;
}

stock StopAudioStream(playerid) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
    new BitStream:bs = BS_New();

    BS_WriteValue(bs, PR_INT32, 1);   
    BS_WriteValue(bs, PR_UINT8, 0);   
    BS_WriteValue(bs, PR_INT32, 0);    
    BS_WriteValue(bs, PR_FLOAT, 0.0);  
    BS_WriteValue(bs, PR_UINT8, 1);   

    PR_SendRPC(bs, playerid, 41, PR_LOW_PRIORITY, PR_RELIABLE_ORDERED);
    BS_Delete(bs);
}

CMD:stopaudio(playerid, params[])
{
    StopAudioStream(playerid);
    SendClientMessage(playerid, 0xFF0000FF, "Ìóçûêà îñòàíîâëåíà!");
    return 1;
}

stock SetVehicleDrift(vehicleid, bool:enable) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
    if (!IsValidVehicle(vehicleid))
        return 0;

    vInfo[vehicleid][vDrift] = enable; 

    foreach(new playerid : Player)
    {
        SetVehicleDriftForPlayer(playerid, vehicleid, enable);
    }
    return 1;
}

stock SetVehicleDriftForPlayer(playerid, vehicleid, bool:enable) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
    new BitStream:bs = BS_New();
    BS_WriteValue(bs, PR_UINT8, 5); 
    BS_WriteValue(bs, PR_UINT16, vehicleid); 
    BS_WriteValue(bs, PR_UINT8, enable); 

    PR_SendRPC(bs, playerid, 168, PR_LOW_PRIORITY, PR_RELIABLE_ORDERED);
    BS_Delete(bs);
    return 1;
}

stock SetVehicleDifferential(vehicleid, type, setvalue=0) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
    if(setvalue) vInfo[vehicleid][vDifferential] = type;
    foreach(new playerid: Player) SetVehicleDifferentialFP(playerid, vehicleid, type);
}


CMD:speedo(playerid, params[])
{
    new vehicleid = GetPlayerVehicleID(playerid); 

    if(vehicleid == INVALID_VEHICLE_ID)
    {
        SendClientMessage(playerid, 0xFF0000FF, "    !");
        return 1;
    }

    
    SetVehicleSpeedometerInfoFP(playerid, vehicleid, 130.0, 1337.0);

    SendClientMessage(playerid, 0x00FF00FF, " :  = 40,  = 40");
    return 1;
}

stock SetVehicleLaunchControl(vehicleid, type, setvalue=0) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
	if(setvalue) vInfo[vehicleid][vLaunchControl] = type;
	foreach(new playerid: Player) SetVehicleLaunchControlFP(playerid, vehicleid, type);
}

stock SetVehicleLaunchControlFP(playerid, vehicleid, type) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
	new BitStream:bitstream = BS_New();
	BS_WriteValue(bitstream, PR_UINT16, vehicleid);
	BS_WriteValue(bitstream, PR_UINT8, 23);

	BS_WriteValue(bitstream, PR_UINT8, type);

	PR_SendRPC(bitstream, playerid, 167, PR_LOW_PRIORITY, PR_RELIABLE_ORDERED);
	BS_Delete(bitstream);
}


CMD:lc(playerid, params[])
{
    if(!IsPlayerInAnyVehicle(playerid))
        return SendClientMessage(playerid, -1, "    !");

    new vehicleid = GetPlayerVehicleID(playerid);

    
    SetVehicleLaunchControl(vehicleid, 1, 1);

    SendClientMessage(playerid, -1, "Launch Control     (type: 1).");
    return 1;
}

stock SetVehicleHydraulics(vehicleid, type, setvalue=0) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
	if(setvalue) vInfo[vehicleid][vHydraulics] = type;
	foreach(new playerid: Player) SetVehicleHydraulicsForPlayer(playerid, vehicleid, type);
}

stock SetVehicleHydraulicsForPlayer(playerid, vehicleid, type) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
	new BitStream:bitstream = BS_New();
	BS_WriteValue(bitstream, PR_UINT16, vehicleid);
	BS_WriteValue(bitstream, PR_UINT8, 21);

	BS_WriteValue(bitstream, PR_UINT8, type);

	PR_SendRPC(bitstream, playerid, 167, PR_LOW_PRIORITY, PR_RELIABLE_ORDERED);
	BS_Delete(bitstream);
}

CMD:hydra(playerid, params[])
{
    if(!IsPlayerInAnyVehicle(playerid))
        return SendClientMessage(playerid, -1, "?? ?????? ?????????? ? ??????????!");

    new vehicleid = GetPlayerVehicleID(playerid);

    
    SetVehicleHydraulics(vehicleid, 1, 1);

    SendClientMessage(playerid, -1, "?????????? ??????????? ?? ??? ????????? (type: 1).");
    return 1;
}

stock SetVehicleStroboscope(vehicleid, type, setvalue=0) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
	if(setvalue) vInfo[vehicleid][vStrob] = type;
	foreach(new playerid: Player) SetVehicleStroboscopeForPlayer(playerid, vehicleid, type);
}

stock SetVehicleStroboscopeForPlayer(playerid, vehicleid, type) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
	new BitStream:bitstream = BS_New();
	BS_WriteValue(bitstream, PR_UINT16, vehicleid);
	BS_WriteValue(bitstream, PR_UINT8, 19);

	BS_WriteValue(bitstream, PR_UINT32, type);

	PR_SendRPC(bitstream, playerid, 167, PR_LOW_PRIORITY, PR_RELIABLE_ORDERED);
	BS_Delete(bitstream);
}

CMD:strab(playerid, params[])
{
    if(!IsPlayerInAnyVehicle(playerid))
        return SendClientMessage(playerid, -1, "Vehicle!");

	extract params -> new type, setvalue; else return Send(playerid, -1, "/strab [type] [value]");

    new vehicleid = GetPlayerVehicleID(playerid);

    SetVehicleStroboscope(vehicleid, type, setvalue);

    return 1;
}

CMD:x2(playerid, params[])
{
	SetPlayerX2(playerid, 1, 10);
}

stock ShowPlayerBackground(playerid, showtime, waittime, hidetime) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
	new BitStream:bitstream = BS_New();
	BS_WriteValue(bitstream, PR_UINT8, 13);

	BS_WriteValue(bitstream, PR_UINT32, showtime);
	BS_WriteValue(bitstream, PR_UINT32, waittime);
	BS_WriteValue(bitstream, PR_UINT32, hidetime);

	PR_SendRPC(bitstream, playerid, 168, PR_LOW_PRIORITY, PR_RELIABLE_ORDERED);
	BS_Delete(bitstream);
}


CMD:bag(playerid, params[])
{
	ShowPlayerBackground(playerid, 300, 300, 300);
}

stock SetVehicleNumberPlateEx(vehicleid, type, number[], region[]) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
	vInfo[vehicleid][vNumber][Type] = type;
	format(vInfo[vehicleid][vNumber][Number], 9, number);
	format(vInfo[vehicleid][vNumber][Region], 4, region);
	foreach(new playerid: Player)
	{
		SetVehicleNumberPlateForPlayer(playerid, vehicleid, type, number, region);
	}
}

stock SetVehicleNumberPlateForPlayer(playerid, vehicleid, type, number[], region[]) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
	new BitStream:bitstream = BS_New();
	BS_WriteValue(bitstream, PR_UINT8, 7);

	BS_WriteValue(bitstream, PR_UINT16, vehicleid);

	BS_WriteValue(bitstream, PR_UINT8, type);

	BS_WriteValue(bitstream, PR_UINT8, strlen(number));
	BS_WriteValue(bitstream, PR_STRING, number);

	BS_WriteValue(bitstream, PR_UINT8, strlen(region));
	BS_WriteValue(bitstream, PR_STRING, region);

	PR_SendRPC(bitstream, playerid, 168, PR_LOW_PRIORITY, PR_RELIABLE_ORDERED);
	BS_Delete(bitstream);
}

CMD:plate(playerid, params[])
{
    if(!IsPlayerInAnyVehicle(playerid))
        return SendClientMessage(playerid, -1, "Vehicle!");

    new vehicleid = GetPlayerVehicleID(playerid);

	SetVehicleNumberPlateEx(vehicleid, 1, "B888BB", "88");
}

stock SetVehicleSiren(vehicleid, type, setvalue=0) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
	if(setvalue) vInfo[vehicleid][vSiren] = type;
	foreach(new playerid: Player) SetVehicleSirenForPlayer(playerid, vehicleid, type);
}

stock SetVehicleSirenForPlayer(playerid, vehicleid, type) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
	new BitStream:bitstream = BS_New();
	BS_WriteValue(bitstream, PR_UINT16, vehicleid);
	BS_WriteValue(bitstream, PR_UINT8, 26);

	BS_WriteValue(bitstream, PR_UINT8, type);

	PR_SendRPC(bitstream, playerid, 167, PR_LOW_PRIORITY, PR_RELIABLE_ORDERED);
	BS_Delete(bitstream);
}

stock SetPlayerGreenZoneFP(playerid, status) // ÀÂÒÎĞ ÄÀÍÍÛÕ ĞÏÑ ÂÑÅÕ È ÒÀÊÆÅ ÀÂÒÎĞ ÑËÈÂÀ DEATHCODER @deathcoder_twink
{
    new BitStream:bitstream = BS_New();

    BS_WriteValue(bitstream, PR_UINT8, 6);       
    BS_WriteValue(bitstream, PR_UINT8, status);  

    PR_SendRPC(bitstream, playerid, 168, PR_LOW_PRIORITY, PR_RELIABLE_ORDERED);
}


CMD:greenzone(playerid, params[])
{
    if (strlen(params) == 0)
    {
        SendClientMessage(playerid, 0xFFFFFFAA, ": /greenzone 0|1");
        return 1;
    }

    new status = strval(params); 
    if (status != 0 && status != 1)
    {
        SendClientMessage(playerid, 0xFFFFFFAA, " : 0  1");
        return 1;
    }

    SetPlayerGreenZoneFP(playerid, status); 

    if (status)
        SendClientMessage(playerid, 0xFF00FF00, "  ");
    else
        SendClientMessage(playerid, 0xFFFF0000, "  ");

    return 1;
}


CMD:siren(playerid, params[])
{
    if(!IsPlayerInAnyVehicle(playerid))
        return SendClientMessage(playerid, -1, "Vehicle!");

    new vehicleid = GetPlayerVehicleID(playerid);

	SetVehicleSiren(vehicleid, 1, 1);
}

CMD:crpc(playerid, params[])
{
    ShowPlayerDialog(
        playerid, DIALOG_CRPC, DIALOG_STYLE_LIST,
        " X2\n"\
"RPC fuel\n"\
"RPC mileage\n"\
"RPC " 

    );
    return 1;
}



CMD:tune(playerid, params[])
{
    SendClientMessage(playerid, 0xFFFFFF, "======   ======");
    SendClientMessage(playerid, 0xFFFFFF, "/neon          -    ( 3 )");
    SendClientMessage(playerid, 0xFFFFFF, "/wheelsize []  -    (0-, 1-...)");
    SendClientMessage(playerid, 0xFFFFFF, "/wheeladd [] [] -    /");
    SendClientMessage(playerid, 0xFFFFFF, "/suspension []  -   (0-, >0 )");
    SendClientMessage(playerid, 0xFFFFFF, "/hydra  - / ");
    SendClientMessage(playerid, 0xFFFFFF, "/lc  - / -");
    SendClientMessage(playerid, 0xFFFFFF, "/plate  -   ");
    SendClientMessage(playerid, 0xFFFFFF, "/playaudio              -   ");
    SendClientMessage(playerid, 0xFFFFFF, "/stopaudio              -   ");
    SendClientMessage(playerid, 0xFFFFFF, "/drift              -  ");
        SendClientMessage(playerid, 0xFFFFFF, "/nitro              -  ");
    SendClientMessage(playerid, 0xFFFFFF, "==============================");
    return 1;
}

CMD:nitro(playerid, params[])
{
    if(!IsPlayerInAnyVehicle(playerid))
        return SendClientMessage(playerid, -1, "    !");

    new vehicleid = GetPlayerVehicleID(playerid);

    AddVehicleComponent(vehicleid, 1010);
    return 1;
}


CMD:drift(playerid, params[])
{
    new vehicleid = GetPlayerVehicleID(playerid);

    if (vehicleid == 0)
        return SendClientMessage(playerid, -1, "     !");

    new bool:enable;
    if (sscanf(params, "d", enable))
        return SendClientMessage(playerid, -1, ": /drift [0/1] (0 - , 1 - )");

    SetVehicleDrift(vehicleid, enable);
    SendClientMessage(playerid, -1, enable ? "- !" : "- !");
    return 1;
}

