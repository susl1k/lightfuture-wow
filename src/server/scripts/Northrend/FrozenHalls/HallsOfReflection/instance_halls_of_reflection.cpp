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

/* ScriptData
SDName: instance_halls_of_reflection
SD%Complete: 70%
SDComment:
SDErrors:
SDCategory: instance script
SDAuthor: /dev/rsa, modified by MaxXx2021 aka Mioka
EndScriptData */

#include "ScriptPCH.h"
#include "halls_of_reflection.h"
#include "World.h"
#include "Map.h"
#include "MapManager.h"
#include "Transport.h"

class instance_halls_of_reflection : public InstanceMapScript
{
public:
    instance_halls_of_reflection() : InstanceMapScript("instance_halls_of_reflection", 668) { }

    struct instance_halls_of_reflection_InstanceMapScript : public InstanceScript
    {
        instance_halls_of_reflection_InstanceMapScript(Map* pMap) : InstanceScript(pMap) 
        {
            Difficulty = pMap->GetDifficulty();
            Initialize();
           _teamInInstance = 0;
        }

        uint32 m_auiEncounter[MAX_ENCOUNTERS+1];
        uint32 m_auiLider;
        std::string strSaveData;

        uint8 Difficulty;

        uint64 m_uiFalricGUID;
        uint64 m_uiMarwynGUID;  
        uint64 m_uiLichKingGUID;
        uint64 m_uiLiderGUID;
		uint64 m_uiEscapeLiderGUID;

        uint64 m_uiMainGateGUID;
        uint64 m_uiExitGateGUID;
        uint64 m_uiDoor2GUID;
        uint64 m_uiDoor3GUID;

        uint64 m_uiFrostGeneralGUID;
        uint64 m_uiCaptainsChestHordeGUID;
        uint64 m_uiCaptainsChestAllianceGUID;
        uint64 m_uiFrostmourneGUID;
        uint64 m_uiFrostmourneAltarGUID;
        uint64 m_uiPortalGUID;
        uint64 m_uiIceWall1GUID;
        uint64 m_uiIceWall2GUID;
        uint64 m_uiIceWall3GUID;
        uint64 m_uiIceWall4GUID;
        uint64 m_uiGoCaveGUID;
        uint64 m_uiteamInInstance;
        uint32 _teamInInstance;

		bool queldallarInProgress;
		uint64 captainGUID;
		Transport* gunship;
		
        std::set<uint64> GunshipCannonGUIDs;
        std::set<uint64> GunshipStairGUIDs;

        void Initialize()
        {
            for (uint8 i = 0; i < MAX_ENCOUNTERS; ++i)
                m_auiEncounter[i] = NOT_STARTED;
            m_uiMainGateGUID = 0;
            m_uiFrostmourneGUID = 0;
            m_uiFalricGUID = 0;
            m_uiLiderGUID = 0;
			m_uiEscapeLiderGUID = 0;
            m_uiLichKingGUID = 0;
            m_uiExitGateGUID = 0;
            m_uiIceWall1GUID = 0;
            m_uiIceWall2GUID = 0;
            m_uiIceWall3GUID = 0;
            m_uiIceWall4GUID = 0;
            m_uiGoCaveGUID = 0;
			queldallarInProgress = false;
			captainGUID = 0;
			gunship = NULL;
        }

        void OpenDoor(uint64 guid)
        {
            if(!guid) return;
            GameObject* go = instance->GetGameObject(guid);
            if(go) go->SetGoState(GO_STATE_ACTIVE);
        }

        void CloseDoor(uint64 guid)
        {
            if(!guid) return;
            GameObject* go = instance->GetGameObject(guid);
            if(go) go->SetGoState(GO_STATE_READY);
        }

        void OnCreatureCreate(Creature* creature)
        {
            if (!_teamInInstance)
            {
                Map::PlayerList const &players = instance->GetPlayers();
                if (!players.isEmpty())
                    if (Player* player = players.begin()->getSource())
                        _teamInInstance = player->GetTeam();
            }

            switch(creature->GetEntry())
            {
                case NPC_FALRIC: 
                       m_uiFalricGUID = creature->GetGUID(); 
                       break;
                case NPC_MARWYN: 
                       m_uiMarwynGUID = creature->GetGUID();  
                       break;
                case BOSS_LICH_KING: 
                       m_uiLichKingGUID = creature->GetGUID();
                       break;
                case NPC_FROST_GENERAL:
                       m_uiFrostGeneralGUID = creature->GetGUID();
                       break;
                case NPC_SYLVANA: 
                        if (_teamInInstance == ALLIANCE)
                           creature->UpdateEntry(NPC_JAINA, ALLIANCE);
						creature->SetPhaseMask(65535, true);
                       break;
                case NPC_DARK_RANGER: 
                        if (_teamInInstance == ALLIANCE)
                           creature->UpdateEntry(NPC_ARCHMAGE, ALLIANCE);
                       break;
                case NPC_SYLVANA_OUTRO: 
                        if (_teamInInstance == ALLIANCE)
                           creature->UpdateEntry(NPC_JAINA_OUTRO, ALLIANCE);
                        creature->SetPhaseMask(65535, true);
						m_uiEscapeLiderGUID = creature->GetGUID();
                       break;
                case NPC_JUSTIN_BARTLETT:
                case NPC_KORM_BLACKSCAR:
                    captainGUID = creature->GetGUID();
                    break;
                case NPC_WORLD_TRIGGER:
                    if (!creature->GetTransport())
                        break;
                    // no break
                case NPC_GUNSHIP_CANNON_HORDE:
                    GunshipCannonGUIDs.insert(creature->GetGUID());
                    break;
            }
        }

        void OnPlayerEnter(Player *pPlayer)
        {
			if (!GetData(TYPE_QUEL_DALLAR))
				if (pPlayer->GetQuestStatus(QUEST_QUEL_DELLAR_A) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(QUEST_QUEL_DELLAR_H) == QUEST_STATUS_INCOMPLETE)
				{
					SetData(TYPE_QUEL_DALLAR, 1);
					instance->SummonCreature(NPC_UTHER, UtherQueldellarPosition)->AI()->DoAction(ACTION_START_FIGHT);
				}

            if (!_teamInInstance)
                _teamInInstance = pPlayer->GetTeam();

			//if (!gunship)
			//	SpawnGunship();

			sMapMgr->LoadTransportForPlayers(pPlayer);

			enum PhaseControl
			{
				HORDE_CONTROL_PHASE_SHIFT_1    = 55773,
				HORDE_CONTROL_PHASE_SHIFT_2    = 60028,
				ALLIANCE_CONTROL_PHASE_SHIFT_1 = 55774,
				ALLIANCE_CONTROL_PHASE_SHIFT_2 = 60027,
			};
        
			if (!sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GROUP)) return;

            switch (pPlayer->GetTeam())
            {
                case ALLIANCE:
                      if (pPlayer && pPlayer->IsInWorld() && pPlayer->HasAura(HORDE_CONTROL_PHASE_SHIFT_1))
                          pPlayer->RemoveAurasDueToSpell(HORDE_CONTROL_PHASE_SHIFT_1);
                      pPlayer->CastSpell(pPlayer, HORDE_CONTROL_PHASE_SHIFT_2, false);
                      break;
                case HORDE:
                      if (pPlayer && pPlayer->IsInWorld() && pPlayer->HasAura(ALLIANCE_CONTROL_PHASE_SHIFT_1)) 
                          pPlayer->RemoveAurasDueToSpell(ALLIANCE_CONTROL_PHASE_SHIFT_1);
                      pPlayer->CastSpell(pPlayer, ALLIANCE_CONTROL_PHASE_SHIFT_2, false);
                      break;
            }

        };

        void OnGameObjectCreate(GameObject* go)
        {
            switch(go->GetEntry())
            {
                case GO_IMPENETRABLE_DOOR: m_uiMainGateGUID = go->GetGUID(); break;
                case GO_FROSTMOURNE: m_uiFrostmourneGUID = go->GetGUID(); break;
                case GO_ICECROWN_DOOR:     m_uiExitGateGUID = go->GetGUID(); break;
                case GO_ICECROWN_DOOR_2:   m_uiDoor2GUID = go->GetGUID(); break;
                case GO_ICECROWN_DOOR_3:   m_uiDoor3GUID = go->GetGUID(); break;
                case GO_PORTAL:            m_uiPortalGUID = go->GetGUID(); break;
                case GO_CAPTAIN_CHEST_1:
                                      if (Difficulty == RAID_DIFFICULTY_10MAN_NORMAL)
                                      m_uiCaptainsChestHordeGUID = go->GetGUID(); 
                                      break;
                case GO_CAPTAIN_CHEST_3:
                                      if (Difficulty == RAID_DIFFICULTY_25MAN_NORMAL)
                                      m_uiCaptainsChestHordeGUID = go->GetGUID(); 
                                      break;
                case GO_CAPTAIN_CHEST_2:
                                      if (Difficulty == RAID_DIFFICULTY_10MAN_NORMAL)
                                      m_uiCaptainsChestAllianceGUID = go->GetGUID(); 
                                      break;
                case GO_CAPTAIN_CHEST_4:
                                      if (Difficulty == RAID_DIFFICULTY_25MAN_NORMAL)
                                      m_uiCaptainsChestAllianceGUID = go->GetGUID(); 
                                      break;
                case GO_ICE_WALL_1:
                    m_uiIceWall1GUID = go->GetGUID();
                    go->SetGoState(GO_STATE_ACTIVE);
                    break;
                case GO_ICE_WALL_2:
                    m_uiIceWall2GUID = go->GetGUID();
                    go->SetGoState(GO_STATE_ACTIVE);
                    break;
                case GO_ICE_WALL_3:
                    m_uiIceWall3GUID = go->GetGUID();
                    go->SetGoState(GO_STATE_ACTIVE);
                    break;
                case GO_ICE_WALL_4:
                    m_uiIceWall4GUID = go->GetGUID();
                    go->SetGoState(GO_STATE_ACTIVE);
                    break;
                case GO_CAVE:
                    m_uiGoCaveGUID = go->GetGUID();
                    go->SetGoState(GO_STATE_ACTIVE);
                    break;
                case GO_THE_SKYBREAKER_STAIRS:
                case GO_ORGRIMS_HAMMER_STAIRS:
					if (GetData(TYPE_LICH_KING) == DONE)
                        go->SetRespawnTime(DAY);
                    GunshipStairGUIDs.insert(go->GetGUID());
                    break;
            }
        }

        void SetData(uint32 uiType, uint32 uiData)
        {
            switch(uiType)
            {
                case TYPE_PHASE:                m_auiEncounter[uiType] = uiData;
												uiData = NOT_STARTED;
					break;
                case TYPE_EVENT:                m_auiEncounter[uiType] = uiData;
                                                uiData = NOT_STARTED;
                    break;
                case TYPE_FALRIC:               m_auiEncounter[uiType] = uiData;
                    if(uiData == SPECIAL)
                                                    CloseDoor(m_uiExitGateGUID);
                    break;
                case TYPE_MARWYN:               m_auiEncounter[uiType] = uiData;
                    if(uiData == DONE)
                    {
                                                   OpenDoor(m_uiMainGateGUID);
                                                   OpenDoor(m_uiExitGateGUID);
                    }
					else if (uiData == FAIL)
					                               OpenDoor(m_uiExitGateGUID);
                    break;
                case TYPE_FROST_GENERAL:        m_auiEncounter[uiType] = uiData; 
                                                if(uiData == DONE)
                                                   OpenDoor(m_uiDoor2GUID);
                    break;
                case TYPE_LICH_KING:            m_auiEncounter[uiType] = uiData;
                                                if(uiData == IN_PROGRESS)
                                                   OpenDoor(m_uiDoor3GUID);
                                                else if(uiData == DONE)
                                                {
													if (m_auiLider == 1)
													{
														if (GameObject* pChest = instance->GetGameObject(m_uiCaptainsChestAllianceGUID))
															if (pChest && !pChest->isSpawned())
																pChest->SetRespawnTime(DAY);
													}
													else if (GameObject* pChest = instance->GetGameObject(m_uiCaptainsChestHordeGUID))
													{
														if (pChest && !pChest->isSpawned())
															pChest->SetRespawnTime(DAY);
													}

													if (GameObject* pPortal = instance->GetGameObject(m_uiPortalGUID))
														if (pPortal && !pPortal->isSpawned())
															pPortal->SetRespawnTime(DAY);
                                                }
												else if(uiData == FAIL)
                                                   CloseDoor(m_uiDoor3GUID);
                    break;
				case TYPE_ICE_WALL_01:          m_auiEncounter[uiType] = uiData; instance->GetGameObject(m_uiIceWall1GUID)->SetGoState(uiData == IN_PROGRESS ? GO_STATE_READY : GO_STATE_ACTIVE); break;
                case TYPE_ICE_WALL_02:          m_auiEncounter[uiType] = uiData; instance->GetGameObject(m_uiIceWall2GUID)->SetGoState(uiData == IN_PROGRESS ? GO_STATE_READY : GO_STATE_ACTIVE); break;
                case TYPE_ICE_WALL_03:          m_auiEncounter[uiType] = uiData; instance->GetGameObject(m_uiIceWall3GUID)->SetGoState(uiData == IN_PROGRESS ? GO_STATE_READY : GO_STATE_ACTIVE); break;
                case TYPE_ICE_WALL_04:          m_auiEncounter[uiType] = uiData; instance->GetGameObject(m_uiIceWall4GUID)->SetGoState(uiData == IN_PROGRESS ? GO_STATE_READY : GO_STATE_ACTIVE); break;
                case TYPE_HALLS:                m_auiEncounter[uiType] = uiData; break;
                case DATA_LIDER:                m_auiLider = uiData;
                                                uiData = NOT_STARTED;
                    break;
				case TYPE_QUEL_DALLAR:
					queldallarInProgress = uiData;
					uiData = NOT_STARTED;
					break;
				case TYPE_GUNSHIP:
					switch (uiData)
					{
						case IN_PROGRESS:
							//if (!gunship)
								//SpawnGunship();
							if (gunship)
							{
								gunship->BuildStartMovePacket(gunship->GetMap());
								gunship->SetUInt32Value(GAMEOBJECT_DYNAMIC, 0x10830010);
								gunship->SetFloatValue(GAMEOBJECT_PARENTROTATION + 3, 1.0f);
								
								std::set<uint32> mapsUsed;
								GameObjectTemplate const* goinfo = gunship->GetGOInfo();

								gunship->GenerateWaypoints(goinfo->moTransport.taxiPathId, mapsUsed);

								UpdateTransportMotionInMap(gunship);
							}
							break;
						case DONE:
							if (gunship)
							{
								gunship->m_WayPoints.clear();
								gunship->BuildStopMovePacket(gunship->GetMap());

								UpdateTransportMotionInMap(gunship);
								// Actualizando sus estados antes de bajarse.
								gunship->UpdateNPCPositions();
								gunship->UpdatePlayerPositions();
							}
							break;
					}
					uiData = NOT_STARTED;
					break;
            }

            if (uiData == DONE)
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;

                for(uint8 i = 0; i < MAX_ENCOUNTERS; ++i)
                    saveStream << m_auiEncounter[i] << " ";

                strSaveData = saveStream.str();

                SaveToDB();
                OUT_SAVE_INST_DATA_COMPLETE;
            }
        }

		void UpdateTransportMotionInMap(Transport* t)
		{
			Map* map = t->GetMap();

			for (Map::PlayerList::const_iterator itr = map->GetPlayers().begin(); itr != map->GetPlayers().end(); ++itr)
			{
				if (Player* pPlayer = itr->getSource())
				{
					UpdateData transData;
					t->BuildCreateUpdateBlockForPlayer(&transData, pPlayer);
					WorldPacket packet;
					transData.BuildPacket(&packet);
					pPlayer->SendDirectMessage(&packet);
				}
			}
		}

        const char* Save()
        {
            return strSaveData.c_str();
        }

        uint32 GetData(uint32 uiType)
        {
            switch(uiType)
            {
                case TYPE_PHASE:                return m_auiEncounter[uiType];
                case TYPE_EVENT:                return m_auiEncounter[uiType];
                case TYPE_FALRIC:               return m_auiEncounter[uiType];
                case TYPE_MARWYN:               return m_auiEncounter[uiType];
                case TYPE_LICH_KING:            return m_auiEncounter[uiType];
                case TYPE_FROST_GENERAL:        return m_auiEncounter[uiType];
                case TYPE_ICE_WALL_01:          return m_auiEncounter[uiType];
                case TYPE_ICE_WALL_02:          return m_auiEncounter[uiType];
                case TYPE_ICE_WALL_03:          return m_auiEncounter[uiType];
                case TYPE_ICE_WALL_04:          return m_auiEncounter[uiType];
                case TYPE_HALLS:                return m_auiEncounter[uiType];
                case DATA_LIDER:                return m_auiLider;
                case DATA_TEAM_IN_INSTANCE:     return _teamInInstance;
				case TYPE_QUEL_DALLAR:          return queldallarInProgress;
                default:                        return 0;
            }
            return 0;
        }

        void SetData64(uint32 uiData, uint64 uiGuid)
        {
            switch(uiData)
            {
                case DATA_ESCAPE_LIDER:
                    m_uiLiderGUID = uiGuid;
                    break;
            }
        }

        uint64 GetData64(uint32 uiData)
        {
            switch(uiData)
            {
                case GO_IMPENETRABLE_DOOR: return m_uiMainGateGUID;
                case GO_FROSTMOURNE:       return m_uiFrostmourneGUID;
                case NPC_FALRIC:           return m_uiFalricGUID;
                case NPC_MARWYN:           return m_uiMarwynGUID;
                case BOSS_LICH_KING:       return m_uiLichKingGUID;
                case DATA_ESCAPE_LIDER:    return m_uiLiderGUID;
                case DATA_ESCAPE_LIDER_2:  return m_uiEscapeLiderGUID;
                case NPC_FROST_GENERAL:    return m_uiFrostGeneralGUID;
                case GO_ICECROWN_DOOR:     return m_uiExitGateGUID;
                case GO_ICECROWN_DOOR_2:   return m_uiDoor2GUID;
                case GO_ICECROWN_DOOR_3:   return m_uiDoor3GUID;
                case GO_ICE_WALL_1:        return m_uiIceWall1GUID;
                case GO_ICE_WALL_2:        return m_uiIceWall2GUID;
                case GO_ICE_WALL_3:        return m_uiIceWall3GUID;
                case GO_ICE_WALL_4:        return m_uiIceWall4GUID;
                case GO_CAVE:              return m_uiGoCaveGUID;
            }
            return 0;
        }

        void Load(const char* chrIn)
        {
            if (!chrIn)
            {
                OUT_LOAD_INST_DATA_FAIL;
                return;
            }

            OUT_LOAD_INST_DATA(chrIn);

            std::istringstream loadStream(chrIn);

            for(uint8 i = 0; i < MAX_ENCOUNTERS; ++i)
            {
                loadStream >> m_auiEncounter[i];

                if (m_auiEncounter[i] == IN_PROGRESS)
                    m_auiEncounter[i] = NOT_STARTED;
            }

            OUT_LOAD_INST_DATA_COMPLETE;
        }


		void SpawnGunship()
		{
			switch (_teamInInstance)
			{
				case ALLIANCE:
					gunship = sMapMgr->LoadTransportInMap(instance, GO_GUNSHIP_SKYBREAKER, 10800000);

					gunship->AddNPCPassengerInInstance(30351, 40.85356f, 44.65979f, 25.11708f, 2.617994f);
					gunship->AddNPCPassengerInInstance(30867, -32.58828f, 22.11204f, 21.78542f, 1.762783f);
					gunship->AddNPCPassengerInInstance(30867, -11.11922f, 23.02313f, 21.71026f, 1.727876f);
					gunship->AddNPCPassengerInInstance(30867, 36.80393f, 45.60984f, 25.11626f, 1.43117f);
					gunship->AddNPCPassengerInInstance(30351, 1.432831f, 22.45517f, 21.75367f, 3.193953f);
					gunship->AddNPCPassengerInInstance(30394, -57.66117f, -6.004808f, 23.56313f, 4.956735f);
					gunship->AddNPCPassengerInInstance(30351, 1.032896f, 9.635975f, 20.53982f, 3.211406f);
					gunship->AddNPCPassengerInInstance(30351, -36.27486f, -6.711545f, 20.53283f, 1.53589f);
					gunship->AddNPCPassengerInInstance(30351, -36.26366f, 6.612509f, 20.5329f, 4.642576f);
					gunship->AddNPCPassengerInInstance(30352, -16.93313f, 2.497342f, 20.87589f, 3.106686f);
					gunship->AddNPCPassengerInInstance(30352, -49.00542f, 0.003014f, 20.75066f, 0.01745329f);
					gunship->AddNPCPassengerInInstance(30352, 16.73826f, 2.378118f, 20.50117f, 3.159046f);
					gunship->AddNPCPassengerInInstance(30352, 48.81408f, 8.76864f, 40.16452f, 1.675516f);
					gunship->AddNPCPassengerInInstance(30352, -16.85414f, -2.518523f, 20.87587f, 3.263766f);
					gunship->AddNPCPassengerInInstance(30351, 4.010166f, -22.42914f, 21.77942f, 0.2617994f);
					gunship->AddNPCPassengerInInstance(30351, 0.778628f, -9.484917f, 20.5411f, 3.036873f);
					gunship->AddNPCPassengerInInstance(30867, -9.599308f, -23.15501f, 21.71576f, 4.782202f);
					gunship->AddNPCPassengerInInstance(30352, 48.82674f, -8.803922f, 40.16443f, 4.712389f);
					gunship->AddNPCPassengerInInstance(30867, -32.99351f, -22.17393f, 21.7879f, 4.502949f);
					gunship->AddNPCPassengerInInstance(30867, 37.14339f, -45.94594f, 25.11639f, 4.014257f);
					gunship->AddNPCPassengerInInstance(30344, -2.700737f, 12.2316f, 20.52945f, 1.727876f);
					gunship->AddNPCPassengerInInstance(22515, -27.09398f, 38.85326f, 1.366914f, 1.256637f);
					gunship->AddNPCPassengerInInstance(22515, -6.396934f, 39.80114f, 1.470398f, 1.256637f);
					gunship->AddNPCPassengerInInstance(22515, 4.017809f, 38.32001f, 1.533938f, 1.256637f);
					gunship->AddNPCPassengerInInstance(30392, 28.1948f, 7.542603f, 23.37183f, 5.8294f);
					gunship->AddNPCPassengerInInstance(30833, 6.518055f, 0.003965f, 20.66434f, 0.0f);
					gunship->AddNPCPassengerInInstance(30350, 16.36582f, -2.323581f, 20.49201f, 3.141593f);
					gunship->AddNPCPassengerInInstance(30347, 28.24817f, -7.667989f, 23.37183f, 0.4363323f);
					break; 
				case HORDE:
					gunship = sMapMgr->LoadTransportInMap(instance, GO_GUNSHIP_ORGRIMS_HAMMER, 10800000);

					gunship->AddNPCPassengerInInstance(37593, 2.015905f, 34.44526f, 10.09305f, 1.64061f);
					gunship->AddNPCPassengerInInstance(37593, 14.85607f, 33.80163f, 9.849781f, 1.64061f);
					gunship->AddNPCPassengerInInstance(37593, -10.84229f, 34.34502f, 10.6434f, 1.64061f);
					gunship->AddNPCPassengerInInstance(30824, 17.28272f, 21.73325f, 35.37741f, 1.623156f);

					break;
			}
			gunship->SetZoneScript();
		}

        void ProcessEvent(WorldObject* /*gameObject*/, uint32 eventId)
        {
			if (Creature* captain = instance->GetCreature(captainGUID))
				captain->MonsterSay("zpracovava se event", LANG_UNIVERSAL, 0);

            switch (eventId)
            {
				case EVENT_GUNSHIP_ARRIVAL:
					if (gunship)
						gunship->BuildStopMovePacket(instance);
					
                    if (Creature* captain = instance->GetCreature(captainGUID))
                        captain->AI()->Talk(0);
					
					for (std::set<uint64>::iterator i = GunshipCannonGUIDs.begin(); i != GunshipCannonGUIDs.end(); i++)
                        if (Creature* cannon = instance->GetCreature(*i))
							if ((cannon->GetEntry() == NPC_WORLD_TRIGGER) == (_teamInInstance == ALLIANCE))
                                cannon->CastSpell(cannon, SPELL_GUNSHIP_CANNON_FIRE, true);
					break;
				case EVENT_GUNSHIP_ARRIVAL_2:
					SetData(TYPE_GUNSHIP, DONE);
					/*if (gunship)
						gunship->BuildWaitMovePacket(instance);*/
					
                    if (Creature* captain = instance->GetCreature(captainGUID))
                        captain->AI()->Talk(1);
					
					for (std::set<uint64>::iterator i = GunshipStairGUIDs.begin(); i != GunshipStairGUIDs.end(); i++)
                        if (GameObject* stairs = instance->GetGameObject(*i))
							if ((stairs->GetEntry() == GO_THE_SKYBREAKER_STAIRS) == (_teamInInstance == ALLIANCE))
								stairs->SetRespawnTime(DAY);
					break;
			}
		}
    };

    InstanceScript* GetInstanceScript (InstanceMap* pMap) const
    {
        return new instance_halls_of_reflection_InstanceMapScript(pMap);
    }
};


void AddSC_instance_halls_of_reflection()
{
    new instance_halls_of_reflection();
}
