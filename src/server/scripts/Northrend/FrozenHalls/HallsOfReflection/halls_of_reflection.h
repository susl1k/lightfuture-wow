/*
 * Copyright (C) 2008 - 2010 Trinity <http://www.trinitycore.org/>
 *
 * Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef DEF_HALLS_OF_REFLECTION_H
#define DEF_HALLS_OF_REFLECTION_H

enum Data
{
    TYPE_PHASE              = 0,
    TYPE_EVENT              = 1,
    TYPE_FALRIC             = 2,
    TYPE_MARWYN             = 3,
    TYPE_FROST_GENERAL      = 4,
    TYPE_LICH_KING          = 5,
    TYPE_ICE_WALL_01        = 6,
    TYPE_ICE_WALL_02        = 7,
    TYPE_ICE_WALL_03        = 8,
    TYPE_ICE_WALL_04        = 9,
    TYPE_HALLS              = 10,
    MAX_ENCOUNTERS,
	TYPE_QUEL_DALLAR,
	TYPE_GUNSHIP,

    DATA_ESCAPE_LIDER       = 101,
    DATA_LIDER              = 102,
    DATA_SUMMONS            = 103,
    DATA_TEAM_IN_INSTANCE   = 104,
	DATA_ESCAPE_LIDER_2     = 105,

    /*UNITS*/
    NPC_DARK_1                   = 38177, //Shadowy Mercenary
    NPC_DARK_2                   = 38176, //Tortured Rifleman
    NPC_DARK_3                   = 38173, //Spectral Footman
    NPC_DARK_4                   = 38172, //Phantom Mage
    NPC_DARK_5                   = 38567, //Phantom Hallucination
    NPC_DARK_6                   = 38175, //Ghostly Priest
    NPC_JAINA                    = 37221,
    NPC_DARK_RANGER             = 37779,
    NPC_ARCHMAGE                 = 37774,
    NPC_SYLVANA                  = 37223,
    NPC_JAINA_OUTRO              = 36955,
    NPC_SYLVANA_OUTRO            = 37554,
    NPC_ALTAR_TARGET             = 37704,
    NPC_UTHER                    = 37225,
    NPC_LICH_KING                = 36954,
    BOSS_LICH_KING               = 37226,
    NPC_ICE_WALL                 = 37014,
    NPC_FALRIC                   = 38112,
    NPC_MARWYN                   = 38113,
    NPC_GHOSTLY_ROGUE            = 38177,
    NPC_GHOSTLY_PRIEST           = 38175,
    NPC_GHOSTLY_MAGE             = 38172,
    NPC_GHOSTLY_FOOTMAN          = 38173,
    NPC_GHOSTLY_RIFLEMAN         = 38176,
    NPC_GLUK                     = 38567,

    NPC_RAGING_GNOUL             = 36940,
    NPC_RISEN_WITCH_DOCTOR       = 36941,
    NPC_ABON                     = 37069,

    NPC_FROST_GENERAL            = 36723,
    NPC_REFLECTION                                = 37068, // 37107 for tank only?

    GO_ICECROWN_DOOR             = 201976, //72802
    GO_ICECROWN_DOOR_2           = 197342,
    GO_ICECROWN_DOOR_3           = 197343,
    GO_IMPENETRABLE_DOOR         = 197341, //72801
    GO_FROSTMOURNE_ALTAR         = 202236, //3551
    GO_FROSTMOURNE               = 202302, //364

    GO_ICE_WALL_1                = 201385,
    GO_ICE_WALL_2                = 201885,
    GO_ICE_WALL_3                = 202396,
    GO_ICE_WALL_4                = 500001,
    GO_CAVE                      = 201596,
    GO_PORTAL                    = 202079,

    GO_CAPTAIN_CHEST_1           = 202212, //3145
    GO_CAPTAIN_CHEST_2           = 201710, //30357
    GO_CAPTAIN_CHEST_3           = 202337, //3246
    GO_CAPTAIN_CHEST_4           = 202336, //3333
};

struct Locations
{
    float x, y, z, o;
    uint32 id;
};

static Locations SpawnLoc[]=
{
    { 5309.577f, 2042.668f, 707.7781f, 4.694936f },
    { 5295.885f, 2040.342f, 707.7781f, 5.078908f },
    { 5340.836f, 1992.458f, 707.7781f, 2.757620f },
    { 5325.072f, 1977.597f, 707.7781f, 2.076942f },
    { 5277.365f, 1993.229f, 707.7781f, 0.401426f },
    { 5275.479f, 2001.135f, 707.7781f, 0.174533f },
    { 5302.448f, 2042.222f, 707.7781f, 4.904375f },
    { 5343.293f, 1999.384f, 707.7781f, 2.914700f },
    { 5295.635f, 1973.757f, 707.7781f, 1.186824f },
    { 5311.031f, 1972.229f, 707.7781f, 1.640610f },
    { 5275.076f, 2008.724f, 707.7781f, 6.213372f },
    { 5316.701f, 2041.550f, 707.7781f, 4.502949f },
    { 5344.150f, 2007.168f, 707.7781f, 3.159046f },
    { 5319.158f, 1973.998f, 707.7781f, 1.919862f },
    { 5302.247f, 1972.415f, 707.7781f, 1.378810f },
    { 5277.739f, 2016.882f, 707.7781f, 5.969026f },
    { 5322.964f, 2040.288f, 707.7781f, 4.345870f },
    { 5343.467f, 2015.951f, 707.7781f, 3.490659f },
    { 5313.820f, 1978.146f, 707.7781f, 1.745329f },
    { 5279.649f, 2004.656f, 707.7781f, 0.069814f },
    { 5306.057f, 2037.002f, 707.7781f, 4.817109f },
    { 5337.865f, 2003.403f, 707.7781f, 2.984513f },
    { 5299.434f, 1979.009f, 707.7781f, 1.239184f },
    { 5312.752f, 2037.122f, 707.7781f, 4.590216f },
    { 5335.724f, 1996.859f, 707.7781f, 2.740167f },
    { 5280.632f, 2012.156f, 707.7781f, 6.056293f },
    { 5320.369f, 1980.125f, 707.7781f, 2.007129f },
    { 5306.572f, 1977.474f, 707.7781f, 1.500983f },
    { 5336.599f, 2017.278f, 707.7781f, 3.473205f },
    { 5282.897f, 2019.597f, 707.7781f, 5.881760f },
    { 5318.704f, 2036.108f, 707.7781f, 4.223697f },
    { 5280.513f, 1997.842f, 707.7781f, 0.296706f },
    { 5337.833f, 2010.057f, 707.7781f, 3.228859f },
    { 5299.250f, 2035.998f, 707.7781f, 5.026548f }
};

static Position UtherQueldellarPosition = {5315.82f, 2001.03f, 709.34f, 3.8f};
static Position QuelDellarPosition = {5294.62f, 1991.19f, 707.6f, 4.02f};

enum
{
	EVENT_PREFIGHT_TALK_LK = 1,
	EVENT_PREFIGHT_TALK_ME,
	EVENT_STOP_PREFIGHT,
	EVENT_ICE_WALL_1_1,
	EVENT_ICE_WALL_1_2,
	EVENT_ICE_WALL_2_1,
	EVENT_ICE_WALL_3_1,
	EVENT_ICE_WALL_4_1,
	EVENT_ICE_WALL_4_2,
	EVENT_RESET_FIGHT_1,
	EVENT_RESET_FIGHT_2,
	EVENT_RESET_FIGHT_3,
	EVENT_OUTRO_1,
	EVENT_OUTRO_2,
	EVENT_OUTRO_3,
	EVENT_OUTRO_4,

	ACTION_START_PREFIGHT = 100,
	ACTION_TALK_PREFIGHT,
	ACTION_STOP_PREFIGHT,
	ACTION_START_FIGHT,
	ACTION_ICE_WALL_DESTROYED,
	ACTION_RESET_FIGHT,
	ACTION_ICE_WALL_1,
	ACTION_ICE_WALL_2,
	ACTION_ICE_WALL_3,
	ACTION_ICE_WALL_4,
	ACTION_OUTRO,
	
	//queldellar
	NPC_QUEL_DELLAR = 37158,

	QUEST_QUEL_DELLAR_A = 24480,
	QUEST_QUEL_DELLAR_H = 24561,

	EVENT_START_QUELDELLAR,
	EVENT_OUTRO_5,
	EVENT_START_ATTACK,
	EVENT_QUELDELLAR_BLADESTORM,
	EVENT_QUELDELLAR_HEROIC_STRIKE,
	EVENT_QUELDELLAR_MORTAL_STRIKE,

	SPELL_QUELDELLAR_BLADESTORM = 67541,
	SPELL_QUELDELLAR_HEROIC_STRIKE = 29426,
	SPELL_QUELDELLAR_MORTAL_STRIKE = 16856,

	ACTION_OUTRO_UTHER,
};

enum Gunship
{
	GO_GUNSHIP_SKYBREAKER						= 201598,
	GO_GUNSHIP_ORGRIMS_HAMMER					= 201599,
    GO_THE_SKYBREAKER_STAIRS                    = 201709,
    GO_ORGRIMS_HAMMER_STAIRS                    = 202211,
	
    NPC_WORLD_TRIGGER                           = 22515,//alliance cannon
    NPC_GUNSHIP_CANNON_HORDE                    = 37593,//horde canon
    NPC_JUSTIN_BARTLETT                         = 30344,
    NPC_KORM_BLACKSCAR                          = 30824,

	EVENT_GUNSHIP_ARRIVAL                       = 22709,
    EVENT_GUNSHIP_ARRIVAL_2                     = 22714,

    SPELL_GUNSHIP_CANNON_FIRE                   = 70017,
    SPELL_GUNSHIP_CANNON_FIRE_MISSILE_ALLIANCE  = 70021,
    SPELL_GUNSHIP_CANNON_FIRE_MISSILE_HORDE     = 70246,
};

#endif
