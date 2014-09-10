/*
 * Copyright (C) 2008-2011 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"
#include "Vehicle.h"

/*######
## npc_agnetta_tyrsdottar
######*/

#define SAY_AGGRO                  -1571003
#define GOSSIP_AGNETTA             "Skip the warmup, sister... or are you too scared to face soemeone your own size?"

enum eAgnetta
{
    QUEST_ITS_THAT_YOUR_GOBLIN      = 12969,
    FACTION_HOSTILE_AT1             = 45
};

class npc_agnetta_tyrsdottar : public CreatureScript
{
public:
    npc_agnetta_tyrsdottar() : CreatureScript("npc_agnetta_tyrsdottar") { }

    struct npc_agnetta_tyrsdottarAI : public ScriptedAI
    {
        npc_agnetta_tyrsdottarAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            me->RestoreFaction();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_agnetta_tyrsdottarAI(creature);
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_ITS_THAT_YOUR_GOBLIN) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_AGNETTA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(13691, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
        {
            DoScriptText(SAY_AGGRO, creature);
            player->CLOSE_GOSSIP_MENU();
            creature->setFaction(FACTION_HOSTILE_AT1);
            creature->AI()->AttackStart(player);
        }

        return true;
    }
};

/*######
## npc_frostborn_scout
######*/

#define GOSSIP_ITEM1    "Are you okay? I've come to take you back to Frosthold if you can stand."
#define GOSSIP_ITEM2    "I'm sorry that I didn't get here sooner. What happened?"
#define GOSSIP_ITEM3    "I'll go get some help. Hang in there."

enum eFrostbornScout
{
    QUEST_MISSING_SCOUTS  =  12864
};

class npc_frostborn_scout : public CreatureScript
{
public:
    npc_frostborn_scout() : CreatureScript("npc_frostborn_scout") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {

        if (player->GetQuestStatus(QUEST_MISSING_SCOUTS) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->PlayerTalkClass->SendGossipMenu(13611, creature->GetGUID());
        }

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (uiAction)
        {
        case GOSSIP_ACTION_INFO_DEF+1:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->PlayerTalkClass->SendGossipMenu(13612, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            player->PlayerTalkClass->SendGossipMenu(13613, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            player->PlayerTalkClass->SendGossipMenu(13614, creature->GetGUID());
            player->AreaExploredOrEventHappens(QUEST_MISSING_SCOUTS);
            break;
        }

        return true;
    }
};

/*######
## npc_thorim
######*/

#define GOSSIP_HN "Thorim?"
#define GOSSIP_SN1 "Can you tell me what became of Sif?"
#define GOSSIP_SN2 "He did more than that, Thorim. He controls Ulduar now."
#define GOSSIP_SN3 "It needn't end this way."

enum eThorim
{
    QUEST_SIBLING_RIVALRY = 13064,
    NPC_THORIM = 29445,
    GOSSIP_TEXTID_THORIM1 = 13799,
    GOSSIP_TEXTID_THORIM2 = 13801,
    GOSSIP_TEXTID_THORIM3 = 13802,
    GOSSIP_TEXTID_THORIM4 = 13803
};

class npc_thorim : public CreatureScript
{
public:
    npc_thorim() : CreatureScript("npc_thorim") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->isQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_SIBLING_RIVALRY) == QUEST_STATUS_INCOMPLETE) {
            player->ADD_GOSSIP_ITEM(0, GOSSIP_HN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_THORIM1, creature->GetGUID());
            return true;
        }
        return false;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(0, GOSSIP_SN1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_THORIM2, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->ADD_GOSSIP_ITEM(0, GOSSIP_SN2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_THORIM3, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->ADD_GOSSIP_ITEM(0, GOSSIP_SN3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_THORIM4, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+4:
                player->CLOSE_GOSSIP_MENU();
                player->CompleteQuest(QUEST_SIBLING_RIVALRY);
                break;
        }
        return true;
    }
};

/*######
## npc_goblin_prisoner
######*/

enum eGoblinPrisoner
{
    GO_RUSTY_CAGE = 191544
};

class npc_goblin_prisoner : public CreatureScript
{
public:
    npc_goblin_prisoner() : CreatureScript("npc_goblin_prisoner") { }

    struct npc_goblin_prisonerAI : public ScriptedAI
    {
        npc_goblin_prisonerAI(Creature* creature) : ScriptedAI (creature){}

        void Reset()
        {
            me->SetReactState(REACT_PASSIVE);

            if (GameObject* pGO = me->FindNearestGameObject(GO_RUSTY_CAGE, 5.0f))
            {
                if (pGO->GetGoState() == GO_STATE_ACTIVE)
                    pGO->SetGoState(GO_STATE_READY);
            }
        }

    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_goblin_prisonerAI(creature);
    }
};

/*######
## npc_victorious_challenger
######*/

#define GOSSIP_CHALLENGER            "Let's do this, sister."

enum eVictoriousChallenger
{
    QUEST_TAKING_ALL_CHALLENGERS    = 12971,
    QUEST_DEFENDING_YOUR_TITLE      = 13423,

    SPELL_SUNDER_ARMOR              = 11971,
    SPELL_REND_VC                   = 11977
};

class npc_victorious_challenger : public CreatureScript
{
public:
    npc_victorious_challenger() : CreatureScript("npc_victorious_challenger") { }

    struct npc_victorious_challengerAI : public ScriptedAI
    {
        npc_victorious_challengerAI(Creature* creature) : ScriptedAI(creature) {}

        uint32 SunderArmorTimer;
        uint32 RendTimer;

        void Reset()
        {
            me->RestoreFaction();

            SunderArmorTimer = 10000;
            RendTimer        = 15000;
        }

        void UpdateAI(const uint32 diff)
        {
            //Return since we have no target
            if (!UpdateVictim())
                return;

            if (RendTimer < diff)
            {
                DoCast(me->getVictim(), SPELL_REND_VC, true);
                RendTimer = 15000;
            }else RendTimer -= diff;

            if (SunderArmorTimer < diff)
            {
                DoCast(me->getVictim(), SPELL_SUNDER_ARMOR, true);
                SunderArmorTimer = 10000;
            }else SunderArmorTimer -= diff;

            DoMeleeAttackIfReady();
        }

        void KilledUnit(Unit* /*victim*/)
        {
            me->RestoreFaction();
        }

    };

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->isQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_TAKING_ALL_CHALLENGERS) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_DEFENDING_YOUR_TITLE) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_CHALLENGER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        return false;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
        {
            player->CLOSE_GOSSIP_MENU();
            creature->setFaction(14);
            creature->AI()->AttackStart(player);
        }

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_victorious_challengerAI(creature);
    }
};

/*######
## npc_loklira_crone
######*/

#define GOSSIP_LOKLIRACRONE     "Tell me about this proposal"
#define GOSSIP_LOKLIRACRONE1    "What happened then?"
#define GOSSIP_LOKLIRACRONE2    "You want me to take part in the Hyldsmeet to end the war?"
#define GOSSIP_LOKLIRACRONE3    "Very well. I'll take part in this competition."

enum eLokliraCrone
{
    QUEST_HYLDSMEET     = 12970,

    GOSSIP_TEXTID_LOK1  = 13778,
    GOSSIP_TEXTID_LOK2  = 13779,
    GOSSIP_TEXTID_LOK3  = 13780
};

class npc_loklira_crone : public CreatureScript
{
public:
    npc_loklira_crone() : CreatureScript("npc_loklira_crone") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->isQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_HYLDSMEET) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOKLIRACRONE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }
        return false;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOKLIRACRONE1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_LOK1, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOKLIRACRONE2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_LOK2, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOKLIRACRONE3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_LOK3, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+4:
                player->CLOSE_GOSSIP_MENU();
                player->CompleteQuest(QUEST_HYLDSMEET);
                break;
        }
        return true;
    }
};

/////////////////////
///npc_injured_goblin
/////////////////////

enum eInjuredGoblin
{
    QUEST_BITTER_DEPARTURE     = 12832,
    SAY_QUEST_ACCEPT           =  -1800042,
    SAY_END_WP_REACHED         =  -1800043
};

#define GOSSIP_ITEM_1       "I am ready, lets get you out of here"

class npc_injured_goblin : public CreatureScript
{
public:
    npc_injured_goblin() : CreatureScript("npc_injured_goblin") { }

    struct npc_injured_goblinAI : public npc_escortAI
    {
        npc_injured_goblinAI(Creature* creature) : npc_escortAI(creature) { }

        void WaypointReached(uint32 i)
        {
            Player* player = GetPlayerForEscort();
            switch (i)
            {
            case 26:
                DoScriptText(SAY_END_WP_REACHED, me, player);
                break;
            case 27:
                if (player)
                    player->GroupEventHappens(QUEST_BITTER_DEPARTURE, me);
                break;
            }
        }

        void EnterCombat(Unit* /*who*/) {}

        void Reset() {}

        void JustDied(Unit* /*killer*/)
        {
            Player* player = GetPlayerForEscort();
            if (HasEscortState(STATE_ESCORT_ESCORTING) && player)
                player->FailQuest(QUEST_BITTER_DEPARTURE);
        }

       void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);
            if (!UpdateVictim())
                return;
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_injured_goblinAI(creature);
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->isQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_BITTER_DEPARTURE) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->PlayerTalkClass->SendGossipMenu(9999999, creature->GetGUID());
        }
        else
            player->SEND_GOSSIP_MENU(999999, creature->GetGUID());
        return true;
    }

    bool OnQuestAccept(Player* /*player*/, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_BITTER_DEPARTURE)
            DoScriptText(SAY_QUEST_ACCEPT, creature);

        return false;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        npc_escortAI* pEscortAI = CAST_AI(npc_injured_goblin::npc_injured_goblinAI, creature->AI());

        if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
        {
            pEscortAI->Start(true, true, player->GetGUID());
            creature->setFaction(113);
        }
        return true;
    }
};

/*######
## npc_roxi_ramrocket
######*/

#define SPELL_MECHANO_HOG           60866
#define SPELL_MEKGINEERS_CHOPPER    60867

class npc_roxi_ramrocket : public CreatureScript
{
public:
    npc_roxi_ramrocket() : CreatureScript("npc_roxi_ramrocket") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        //Quest Menu
        if (creature->isQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        //Trainer Menu
        if ( creature->isTrainer() )
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_TEXT_TRAIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);

        //Vendor Menu
        if ( creature->isVendor() )
            if (player->HasSpell(SPELL_MECHANO_HOG) || player->HasSpell(SPELL_MEKGINEERS_CHOPPER))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
        case GOSSIP_ACTION_TRAIN:
            player->GetSession()->SendTrainerList(creature->GetGUID());
            break;
        case GOSSIP_ACTION_TRADE:
            player->GetSession()->SendListInventory(creature->GetGUID());
            break;
        }
        return true;
    }
};

/*######
## npc_brunnhildar_prisoner
######*/

enum BrunnhildarPrisoner {
    SPELL_ICE_PRISON           = 54894,
    SPELL_ICE_LANCE            = 55046,
    SPELL_FREE_PRISONER        = 55048,
    SPELL_RIDE_DRAKE           = 55074,
    SPELL_SHARD_IMPACT         = 55047
};

class npc_brunnhildar_prisoner : public CreatureScript
{
public:
    npc_brunnhildar_prisoner() : CreatureScript("npc_brunnhildar_prisoner") { }

    struct npc_brunnhildar_prisonerAI : public ScriptedAI
    {
        npc_brunnhildar_prisonerAI(Creature* creature) : ScriptedAI(creature) {}

        bool freed;

        void Reset()
        {
            freed = false;
            me->CastSpell(me, SPELL_ICE_PRISON, true);
        }

        void JustRespawned()
        {
            Reset();
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!freed)
                return;

            if (!me->HasUnitState(UNIT_STATE_ONVEHICLE))
            {
                me->DespawnOrUnsummon();
            }
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id != SPELL_ICE_LANCE)
                return;

            if (caster->GetVehicleKit()->GetAvailableSeatCount() != 0)
            {
                me->CastSpell(me, SPELL_FREE_PRISONER, true);
                me->CastSpell(caster, SPELL_RIDE_DRAKE, true);
                me->CastSpell(me, SPELL_SHARD_IMPACT, true);
                freed = true;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_brunnhildar_prisonerAI(creature);
    }
};

class npc_icefang : public CreatureScript
{
public:
    npc_icefang() : CreatureScript("npc_icefang") { }

    struct npc_icefangAI : public npc_escortAI
    {
        npc_icefangAI(Creature* creature) : npc_escortAI(creature) {}

        void AttackStart(Unit* /*who*/) {}
        void EnterCombat(Unit* /*who*/) {}
        void EnterEvadeMode() {}

        void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply)
        {
            if (who->GetTypeId() == TYPEID_PLAYER)
            {
                if (apply)
                    Start(false, true, who->GetGUID());
            }
        }

        void WaypointReached(uint32 /*wp*/)
        {
        }

        void JustDied(Unit* /*killer*/)
        {
        }

        void OnCharmed(bool /*apply*/)
        {
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_icefangAI (creature);
    }
};

class npc_hyldsmeet_protodrake : public CreatureScript
{
    enum NPCs
    {
        NPC_HYLDSMEET_DRAKERIDER = 29694
    };

    public:
        npc_hyldsmeet_protodrake() : CreatureScript("npc_hyldsmeet_protodrake") { }

        class npc_hyldsmeet_protodrakeAI : public CreatureAI
        {
            public:
                npc_hyldsmeet_protodrakeAI(Creature* creature) : CreatureAI(creature), _accessoryRespawnTimer(0), _vehicleKit(creature->GetVehicleKit())
		{
			me->SetCanFly(true);
			me->SetDisableGravity(true);
			me->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_HOVER);
		}

                void PassengerBoarded(Unit* who, int8 /*seat*/, bool apply)
                {
                    if (apply)
                        return;

                    if (who->GetEntry() == NPC_HYLDSMEET_DRAKERIDER)
                        _accessoryRespawnTimer = 5 * MINUTE * IN_MILLISECONDS;
                }

                void UpdateAI(uint32 const diff)
                {
                    //! We need to manually reinstall accessories because the vehicle itself is friendly to players,
                    //! so EnterEvadeMode is never triggered. The accessory on the other hand is hostile and killable.
                    if (_accessoryRespawnTimer && _accessoryRespawnTimer <= diff && _vehicleKit)
                    {
                        _vehicleKit->InstallAllAccessories(true);
                        _accessoryRespawnTimer = 0;
                    }
                    else
                        _accessoryRespawnTimer -= diff;
                }

            private:
                uint32 _accessoryRespawnTimer;
                Vehicle* _vehicleKit;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_hyldsmeet_protodrakeAI (creature);
        }
};

/*######
## npc_freed_protodrake
######*/

enum FreedProtoDrake
{
    AREA_VALLEY_OF_ANCIENT_WINTERS      = 4437,
    TEXT_EMOTE                          = 0,
    SPELL_KILL_CREDIT_PRISONER          = 55144,
    SPELL_SUMMON_LIBERATED              = 55073,
    SPELL_KILL_CREDIT_DRAKE             = 55143
};

const Position FreedDrakeWaypoints[16] =
{
    {7294.96f, -2418.733f, 823.869f, 0.0f},
    {7315.984f, -2331.46f, 826.3972f, 0.0f},
    {7271.826f, -2271.479f, 833.5917f, 0.0f},
    {7186.253f, -2218.475f, 847.5632f, 0.0f},
    {7113.195f, -2164.288f, 850.2301f, 0.0f},
    {7078.018f, -2063.106f, 854.7581f, 0.0f},
    {7073.221f, -1983.382f, 861.9246f, 0.0f},
    {7061.455f, -1885.899f, 865.119f, 0.0f},
    {7033.32f, -1826.775f, 876.2578f, 0.0f},
    {6999.902f, -1784.012f, 897.4521f, 0.0f},
    {6954.913f, -1747.043f, 897.4521f, 0.0f},
    {6933.856f, -1720.698f, 882.2022f, 0.0f},
    {6932.729f, -1687.306f, 866.1189f, 0.0f},
    {6952.458f, -1663.802f, 849.8133f, 0.0f},
    {7002.819f, -1651.681f, 831.397f, 0.0f},
    {7026.531f, -1649.239f, 828.8406f, 0.0f}
};


class npc_freed_protodrake : public CreatureScript
{
public:
    npc_freed_protodrake() : CreatureScript("npc_freed_protodrake") { }

    struct npc_freed_protodrakeAI : public VehicleAI
    {
        npc_freed_protodrakeAI(Creature* creature) : VehicleAI(creature) {}

        bool autoMove;
        bool wpReached;
        uint16 CheckTimer;
        uint16 countWP;

        void Reset()
        {
            autoMove = false;
            wpReached = false;
            CheckTimer = 5000;
            countWP = 0;
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id < 15)
            {
                ++countWP;
                wpReached = true;
            }
            else
            // drake reached village
            {
                // get player that rides drake (from seat 0)
                Unit* player = me->GetVehicleKit()->GetPassenger(0);
                if (player && player->GetTypeId() == TYPEID_PLAYER)
                {
                    // for each prisoner on drake, give credit
                    for (uint8 i = 1; i < 4; ++i)
                        if (Unit* prisoner = me->GetVehicleKit()->GetPassenger(i))
                        {
                            if (prisoner->GetTypeId() != TYPEID_UNIT)
                                return;
                            prisoner->CastSpell(player, SPELL_KILL_CREDIT_PRISONER, true);
                            prisoner->CastSpell(prisoner, SPELL_SUMMON_LIBERATED, true);
                            prisoner->ExitVehicle();
                        }
                    me->CastSpell(me, SPELL_KILL_CREDIT_DRAKE, true);
                    player->ExitVehicle();
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!autoMove)
            {
                if (CheckTimer < diff)
                {
                    CheckTimer = 5000;
                    if (me->GetAreaId() == AREA_VALLEY_OF_ANCIENT_WINTERS)
                    {
                        Talk(TEXT_EMOTE, me->GetVehicleKit()->GetPassenger(0)->GetGUID());
                        autoMove = true;
                        wpReached = true;
                    }
                }
                else
                    CheckTimer -= diff;
            }

            if (wpReached && autoMove)
            {
                wpReached = false;
                me->GetMotionMaster()->MovePoint(countWP, FreedDrakeWaypoints[countWP]);
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_freed_protodrakeAI(creature);
    }
};

enum CloseRift
{
    SPELL_DESPAWN_RIFT          = 61665
};

class spell_close_rift : public SpellScriptLoader
{
    public:
        spell_close_rift() : SpellScriptLoader("spell_close_rift") { }

        class spell_close_rift_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_close_rift_AuraScript);

            bool Load()
            {
                _counter = 0;
                return true;
            }

            bool Validate(SpellInfo const* /*spell*/)
            {
                return sSpellMgr->GetSpellInfo(SPELL_DESPAWN_RIFT);
            }

            void HandlePeriodic(AuraEffect const* /* aurEff */)
            {
                if (++_counter == 5)
                    GetTarget()->CastSpell((Unit*)NULL, SPELL_DESPAWN_RIFT, true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_close_rift_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }

        private:
            uint8 _counter;

        };

        AuraScript* GetAuraScript() const
        {
            return new spell_close_rift_AuraScript();
        }
};

/*######
## Quest: Forging a Head (12985)
######*/

enum DeadIrongiant
{
    ENTRY_AMBUSHER              = 30208,
    SPELL_SALVAGE_CORPSE        = 56227,
    SPELL_CREATE_EYES           = 56230
};

class npc_dead_irongiant : public CreatureScript
{
public:
    npc_dead_irongiant() : CreatureScript("npc_dead_irongiant") {}

    struct npc_dead_irongiantAI : public ScriptedAI
    {
        npc_dead_irongiantAI(Creature* creature) : ScriptedAI(creature) {}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_SALVAGE_CORPSE)
            {
                if (!urand(0,2))
                {
                    for (uint8 i = 0; i < 3; ++i)
                        if (Creature *temp = me->SummonCreature(ENTRY_AMBUSHER, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60*IN_MILLISECONDS))
                            temp->AI()->AttackStart(caster);
                }
                else
                    me->CastSpell(caster, SPELL_CREATE_EYES, true);

                me->DespawnOrUnsummon(500);
            }
        }
    };

    CreatureAI *GetAI(Creature *creature) const
    {
        return new npc_dead_irongiantAI(creature);
    }
};

enum Brothers_Bronzebeard
{
	PATH_WAYPOINTS = 64,

	NPC_FLYING_MACHINE = 30134,
	NPC_BRANN = 30107,
	NPC_MURADIN = 30408,
	NPC_MAGNI = 30411,

	ACTION_START_TALK = 917,
	DATA_PLAYER_GUID = 5432,
	QUEST_BROTHERS_BRONZEBEARD = 12973,


	EVENT_BRANN_SPAWN = 1,
	EVENT_MURADIN_SPAWN,
	EVENT_MAGNI_SPAWN,
	EVENT_ALL_WALK,
	EVENT_DESPAWN_ALL,
	
	TALK_BRANN_01 = 128,
	TALK_BRANN_02,
	TALK_BRANN_03,
	TALK_BRANN_04,
	TALK_BRANN_05,
	TALK_BRANN_06,
	TALK_BRANN_07,
	TALK_BRANN_08,

	TALK_MURADIN_01 = 256,
	TALK_MURADIN_02,
	TALK_MURADIN_03,
	TALK_MURADIN_04,
	TALK_MURADIN_05,
	TALK_MURADIN_06,
	TALK_MURADIN_07,
	TALK_MURADIN_08,
	TALK_MURADIN_09,
	TALK_MURADIN_10,
	TALK_MURADIN_11,
	TALK_MURADIN_12,
	TALK_MURADIN_13,

	TALK_MAGNI_01 = 512,
	TALK_MAGNI_02,
	TALK_MAGNI_03,
	TALK_MAGNI_04,
	TALK_MAGNI_05,
	TALK_MAGNI_06,
	TALK_MAGNI_07,
	TALK_MAGNI_08,
};

const Position points [PATH_WAYPOINTS] = 
{
	{7523.397461f, -971.936890f, 478.741913f, 0.0f},
	{7518.943848f, -973.407349f, 475.441284f, 0.0f},
	{7467.104980f, -999.575317f, 494.572937f, 0.0f},
	{7458.075684f, -1031.964600f, 514.338379f, 0.0f},
	{7468.719238f, -1062.681396f, 534.103455f, 0.0f},
	{7496.499023f, -1080.123779f, 552.983704f, 0.0f},
	{7536.130859f, -1071.417358f, 576.334473f, 0.0f},
	{7554.211426f, -1038.723755f, 596.886169f, 0.0f},
	{7537.178223f, -1000.335693f, 613.765259f, 0.0f},
	{7501.943848f, -992.758789f, 628.372253f, 0.0f},
	{7465.884766f, -1010.102783f, 644.022583f, 0.0f},
	{7448.760254f, -1038.971191f, 656.693665f, 0.0f},
	{7447.369629f, -1066.415527f, 668.386230f, 0.0f},
	{7474.199707f, -1099.170898f, 695.609863f, 0.0f},
	{7502.819824f, -1101.662598f, 717.271118f, 0.0f},
	{7538.560059f, -1089.950195f, 742.631348f, 0.0f},
	{7565.559082f, -1053.397949f, 763.125244f, 0.0f},
	{7564.488770f, -1022.301147f, 775.507263f, 0.0f},
	{7545.917480f, -991.489258f, 788.753784f, 0.0f},
	{7512.195313f, -972.129333f, 799.849670f, 0.0f},
	{7466.723145f, -975.968079f, 811.653259f, 0.0f},
	{7434.344727f, -999.193420f, 823.712952f, 0.0f},
	{7420.422852f, -1026.400879f, 834.623840f, 0.0f},
	{7422.325684f, -1054.944824f, 847.695801f, 0.0f},
	{7439.555176f, -1095.433960f, 868.722656f, 0.0f},
	{7467.803223f, -1118.728394f, 889.030396f, 0.0f},
	{7508.827637f, -1129.564087f, 912.890747f, 0.0f},
	{7545.782227f, -1119.897461f, 929.444336f, 0.0f},
	{7576.872070f, -1089.682617f, 941.986694f, 0.0f},
	{7588.408691f, -1058.593018f, 948.158264f, 0.0f},
	{7588.583496f, -1012.504150f, 953.340088f, 0.0f},
	{7570.491699f, -970.927856f, 956.575500f, 0.0f},
	{7549.674805f, -942.945740f, 958.656921f, 0.0f},
	{7525.700684f, -919.002563f, 960.538391f, 0.0f},
	{7502.805176f, -900.966736f, 961.905945f, 0.0f},
	{7442.785645f, -865.201477f, 965.264221f, 0.0f},
	{7381.229492f, -832.049561f, 968.701355f, 0.0f},
	{7350.224121f, -817.766785f, 969.459229f, 0.0f},
	{7317.675293f, -804.903748f, 969.285828f, 0.0f},
	{7252.815918f, -779.085754f, 964.769897f, 0.0f},
	{7221.525391f, -764.584106f, 958.867920f, 0.0f},
	{7182.653320f, -740.483521f, 948.233276f, 0.0f},
	{7157.730957f, -716.556030f, 937.673889f, 0.0f},
	{7137.936035f, -686.174622f, 924.710754f, 0.0f},
	{7127.196777f, -661.935425f, 915.520020f, 0.0f},
	{7107.217773f, -598.135620f, 894.952026f, 0.0f},
	{7098.825195f, -565.253662f, 886.399353f, 0.0f},
	{7090.391602f, -531.970947f, 879.641174f, 0.0f},
	{7081.550781f, -498.434998f, 874.979858f, 0.0f},
	{7071.466797f, -467.029175f, 873.499451f, 0.0f},
	{7043.198730f, -403.374908f, 879.268188f, 0.0f},
	{7025.937500f, -373.390778f, 884.490967f, 0.0f},
	{7005.911133f, -345.370789f, 890.642700f, 0.0f},
	{6974.771973f, -311.512787f, 899.698486f, 0.0f},
	{6947.843262f, -290.435944f, 907.100952f, 0.0f},
	{6919.657715f, -271.776611f, 916.120300f, 0.0f},
	{6871.432617f, -249.042419f, 936.988159f, 0.0f},
	{6832.978516f, -239.880966f, 959.651428f, 0.0f},
	{6803.871094f, -239.013077f, 979.119934f, 0.0f},
	{6760.672363f, -248.490173f, 1004.487000f, 0.0f},
	{6742.339355f, -258.831696f, 1004.511108f, 0.0f},
	{6732.791504f, -267.286316f, 1002.709717f, 0.0f},
	{6727.387207f, -278.369812f, 999.239075f, 0.0f},
	{6720.798340f, -297.991608f, 992.280396f, 0.0f}
};

const Position BrannSpawnPosition = {6720.798340f, -297.991608f, 992.280396f, 0.0f};
const Position BrannTalkPosition = {6666.520020f, -298.764313f, 988.996338f, 0.0f};

const Position MuradinSpawnPosition = {6653.249512f, -310.646973f, 989.384521f, 0.0f};
const Position MuradinTalkPosition = {6663.880859f, -300.809082f, 988.763550f, 0.0f};

const Position MagniSpawnPosition = {6649.638184f, -296.596069f, 985.170654f, 0.0f};
const Position MagniTalkPosition = {6662.093750f, -296.852142f, 988.097961f, 0.0f};

class npc_brann_q12973 : public CreatureScript
{
public:
    npc_brann_q12973() : CreatureScript("npc_brann_q12973") { }

	bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest)
	{
		Creature* plane = creature->SummonCreature(NPC_FLYING_MACHINE, *creature);
		Creature* brann = creature->SummonCreature(NPC_BRANN, *creature);
		if (plane && brann)
		{
			brann->AI()->SetGUID(player->GetGUID(), DATA_PLAYER_GUID);
			brann->EnterVehicle(plane, 0);
			player->Dismount();
			player->EnterVehicle(plane, 1);

			plane->GetMotionMaster()->MoveSmoothPath(1, points, PATH_WAYPOINTS, 50.0f, true);
		}

		return true;
	}
};

class npc_plane_q12973 : public CreatureScript
{
public:
    npc_plane_q12973() : CreatureScript("npc_plane_q12973") { }

    struct npc_plane_q12973AI : public NullCreatureAI
    {
        npc_plane_q12973AI(Creature* creature) : NullCreatureAI(creature) { }

		void MovementInform(uint32 type, uint32 id)
		{
			if (Unit* brann = me->GetVehicleKit()->GetPassenger(0))
			{
				brann->ExitVehicle();
				if (Unit* player = me->GetVehicleKit()->GetPassenger(1))
					player->ExitVehicle();
				else
					brann->ToCreature()->DespawnOrUnsummon();
				brann->GetAI()->DoAction(ACTION_START_TALK);
				me->DespawnOrUnsummon(1);
			}
		}
    };

    CreatureAI *GetAI(Creature *creature) const
    {
        return new npc_plane_q12973AI(creature);
    }
};

class npc_talk_brann_q12973 : public CreatureScript
{
public:
    npc_talk_brann_q12973() : CreatureScript("npc_talk_brann_q12973") { }

	struct npc_talk_brann_q12973AI : public ScriptedAI
    {
        npc_talk_brann_q12973AI(Creature* creature) : ScriptedAI(creature) { }

		void DoAction(const int32 action)
		{
			switch (action)
			{
				case ACTION_START_TALK:
					events.Reset();
					events.ScheduleEvent(EVENT_BRANN_SPAWN, 500);
					events.ScheduleEvent(EVENT_MURADIN_SPAWN, 6000);
					events.ScheduleEvent(TALK_BRANN_01, 6500);
					break;
			}
		}

		void SetGUID(uint64 guid, int32 data)
		{
			if (data == DATA_PLAYER_GUID)
				player = Unit::GetPlayer(*me, guid);
		}

		void UpdateAI(uint32 const diff)
		{
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
					case EVENT_BRANN_SPAWN:
						me->GetMotionMaster()->MovePoint(1, BrannTalkPosition);
						break;
					case EVENT_MURADIN_SPAWN:
						muradin = me->SummonCreature(NPC_MURADIN, MuradinSpawnPosition);
						muradin->GetMotionMaster()->MovePoint(1, MuradinTalkPosition);
						break;
					case EVENT_MAGNI_SPAWN:
						magni = me->SummonCreature(NPC_MAGNI, MagniSpawnPosition);
						break;
					case EVENT_ALL_WALK:
						me->GetMotionMaster()->MovePoint(2, BrannSpawnPosition);
						muradin->GetMotionMaster()->MovePoint(2, MuradinSpawnPosition);
						magni->GetMotionMaster()->MovePoint(2, MagniSpawnPosition);
						events.ScheduleEvent(EVENT_DESPAWN_ALL, 1500);
						break;
					case EVENT_DESPAWN_ALL:
						muradin->DespawnOrUnsummon();
						magni->DespawnOrUnsummon();
						player->AreaExploredOrEventHappens(QUEST_BROTHERS_BRONZEBEARD);
						me->DespawnOrUnsummon();
						break;

					case TALK_BRANN_01:
						Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_MURADIN_01, 5000);
						break;
					case TALK_BRANN_02:
						Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_MURADIN_02, 10000);
						break;
					case TALK_BRANN_03:
						Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_MURADIN_04, 10000);
						break;
					case TALK_BRANN_04:
						Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_BRANN_05, 13000);
						break;
					case TALK_BRANN_05:
						Talk(eventId & 0xF);
						events.ScheduleEvent(EVENT_MAGNI_SPAWN, 10000);
						events.ScheduleEvent(TALK_MAGNI_01, 15000);
						break;
					case TALK_BRANN_06:
						Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_MAGNI_02, 3000);
						break;
					case TALK_BRANN_07:
						Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_MURADIN_11, 8000);
						break;
					case TALK_BRANN_08:
						Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_MURADIN_13, 12000);
						break;

					case TALK_MURADIN_01:
						muradin->AI()->Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_BRANN_02, 5000);
						break;
					case TALK_MURADIN_02:
						muradin->AI()->Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_MURADIN_03, 5000);
						break;
					case TALK_MURADIN_03:
						muradin->AI()->Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_BRANN_03, 2000);
						break;
					case TALK_MURADIN_04:
						muradin->AI()->Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_MURADIN_05, 10000);
						break;
					case TALK_MURADIN_05:
						muradin->AI()->Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_BRANN_04, 7000);
						break;
					case TALK_MURADIN_06:
						magni->GetMotionMaster()->MovePoint(1, MagniTalkPosition);
						muradin->AI()->Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_MAGNI_03, 8000);
						break;
					case TALK_MURADIN_07:
						muradin->AI()->Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_MAGNI_04, 8000);
						break;
					case TALK_MURADIN_08:
						muradin->AI()->Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_MURADIN_09, 12000);
						break;
					case TALK_MURADIN_09:
						muradin->AI()->Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_MAGNI_05, 4000);
						break;
					case TALK_MURADIN_10:
						muradin->AI()->Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_BRANN_07, 13000);
						break;
					case TALK_MURADIN_11:
						muradin->AI()->Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_MAGNI_06, 11000);
						break;
					case TALK_MURADIN_12:
						muradin->AI()->Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_MAGNI_07, 6000);
						break;
					case TALK_MURADIN_13:
						muradin->AI()->Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_MAGNI_08, 9000);
						break;

					case TALK_MAGNI_01:
						magni->AI()->Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_BRANN_06, 13000);
						break;
					case TALK_MAGNI_02:
						magni->AI()->Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_MURADIN_06, 8000);
						break;
					case TALK_MAGNI_03:
						magni->AI()->Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_MURADIN_07, 15000);
						break;
					case TALK_MAGNI_04:
						magni->AI()->Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_MURADIN_08, 14000);
						break;
					case TALK_MAGNI_05:
						magni->AI()->Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_MURADIN_10, 10000);
						break;
					case TALK_MAGNI_06:
						magni->AI()->Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_MURADIN_12, 11000);
						break;
					case TALK_MAGNI_07:
						magni->AI()->Talk(eventId & 0xF);
						events.ScheduleEvent(TALK_BRANN_08, 9000);
						break;
					case TALK_MAGNI_08:
						magni->AI()->Talk(eventId & 0xF);
						events.ScheduleEvent(EVENT_ALL_WALK, 3000);
						break;
				}
			}
		}

		EventMap events;
		Creature* muradin;
		Creature* magni;
		Player* player;
    };

    CreatureAI *GetAI(Creature *creature) const
    {
        return new npc_talk_brann_q12973AI(creature);
    }
};

enum Stormpeak_Wyrm
{
	SPELL_FROST_SPIT = 57833,
};

class npc_stormpeak_wyrm : public CreatureScript
{
public:
    npc_stormpeak_wyrm() : CreatureScript("npc_stormpeak_wyrm") { }

    struct npc_stormpeak_wyrmAI : public LandingAI
    {
        npc_stormpeak_wyrmAI(Creature* creature) : LandingAI(creature) { }

		void Reset()
		{
			_Reset();
			spellTimer = urand(5000, 6000);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (spellTimer < diff)
			{
				DoCastVictim(SPELL_FROST_SPIT);
				spellTimer = urand(11000, 12000);
			}
			else
				spellTimer -= diff;

			DoMeleeAttackIfReady();
		}

		uint32 spellTimer;
    };

    CreatureAI *GetAI(Creature *creature) const
    {
        return new npc_stormpeak_wyrmAI(creature);
    }
};

void AddSC_storm_peaks()
{
    new npc_agnetta_tyrsdottar;
    new npc_frostborn_scout;
    new npc_thorim;
    new npc_goblin_prisoner;
    new npc_victorious_challenger;
    new npc_loklira_crone;
    new npc_injured_goblin;
    new npc_roxi_ramrocket;
    new npc_brunnhildar_prisoner;
    new npc_freed_protodrake();
    new npc_icefang;
    new npc_hyldsmeet_protodrake;
    new spell_close_rift();
    new npc_dead_irongiant();
	new npc_brann_q12973();
	new npc_plane_q12973();
	new npc_talk_brann_q12973();
	new npc_stormpeak_wyrm();
}
