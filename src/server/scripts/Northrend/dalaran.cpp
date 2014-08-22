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

/* Script Data Start
SDName: Dalaran
SDAuthor: WarHead, MaXiMiUS
SD%Complete: 99%
SDComment: For what is 63990+63991? Same function but don't work correct...
SDCategory: Dalaran
Script Data End */

#include "ScriptPCH.h"
//#include "SharedDefines.h"

/*******************************************************
 * npc_mageguard_dalaran
 *******************************************************/

enum Spells
{
    SPELL_TRESPASSER_A = 54028,
    SPELL_TRESPASSER_H = 54029
};

enum NPCs // All outdoor guards are within 35.0f of these NPCs
{
    NPC_APPLEBOUGH_A = 29547,
    NPC_SWEETBERRY_H = 29715,
};

class npc_mageguard_dalaran : public CreatureScript
{
public:
    npc_mageguard_dalaran() : CreatureScript("npc_mageguard_dalaran") { }

    struct npc_mageguard_dalaranAI : public Scripted_NoMovementAI
    {
        npc_mageguard_dalaranAI(Creature* creature) : Scripted_NoMovementAI(creature)
        {
            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_NORMAL, true);
            creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, true);
        }

        void Reset(){}

        void EnterCombat(Unit* /*who*/){}

        void AttackStart(Unit* /*who*/){}

        void MoveInLineOfSight(Unit* who)
        {
            if (!who || !who->IsInWorld() || who->GetZoneId() != 4395)
                return;

            if (!me->IsWithinDist(who, 65.0f, false))
                return;

            Player* player = who->GetCharmerOrOwnerPlayerOrPlayerItself();
            
            // If player has Disguise aura for quest A Meeting With The Magister or An Audience With The Arcanist, do not teleport it away but let it pass
            if (!player || player->isGameMaster() || player->IsBeingTeleported() || player->HasAura(70973) || player->HasAura(70974) || player->HasAura(70971) || player->HasAura(70972))
                return;

            switch (me->GetEntry())
            {
                case 29254:
                    if (player->GetTeam() == HORDE)              // Horde unit found in Alliance area
                    {
                        if (GetClosestCreatureWithEntry(me, NPC_APPLEBOUGH_A, 32.0f))
                        {
                            if (me->isInBackInMap(who, 12.0f))   // In my line of sight, "outdoors", and behind me
                                DoCast(who, SPELL_TRESPASSER_A); // Teleport the Horde unit out
                        }
                        else                                      // In my line of sight, and "indoors"
                            DoCast(who, SPELL_TRESPASSER_A);     // Teleport the Horde unit out
                    }
                    break;
                case 29255:
                    if (player->GetTeam() == ALLIANCE)           // Alliance unit found in Horde area
                    {
                        if (GetClosestCreatureWithEntry(me, NPC_SWEETBERRY_H, 32.0f))
                        {
                            if (me->isInBackInMap(who, 12.0f))   // In my line of sight, "outdoors", and behind me
                                DoCast(who, SPELL_TRESPASSER_H); // Teleport the Alliance unit out
                        }
                        else                                      // In my line of sight, and "indoors"
                            DoCast(who, SPELL_TRESPASSER_H);     // Teleport the Alliance unit out
                    }
                    break;
            }
            me->SetOrientation(me->GetHomePosition().GetOrientation());
            return;
        }

        void UpdateAI(const uint32 /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mageguard_dalaranAI(creature);
    }
};

/*######
## npc_hira_snowdawn
######*/

enum eHiraSnowdawn
{
    SPELL_COLD_WEATHER_FLYING                   = 54197
};

#define GOSSIP_TEXT_TRAIN_HIRA "I seek training to ride a steed."

class npc_hira_snowdawn : public CreatureScript
{
public:
    npc_hira_snowdawn() : CreatureScript("npc_hira_snowdawn") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (!creature->isVendor() || !creature->isTrainer())
            return false;

        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_TRAIN_HIRA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);

        if (player->getLevel() >= 80 && player->HasSpell(SPELL_COLD_WEATHER_FLYING))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        if (uiAction == GOSSIP_ACTION_TRAIN)
            player->GetSession()->SendTrainerList(creature->GetGUID());

        if (uiAction == GOSSIP_ACTION_TRADE)
            player->GetSession()->SendListInventory(creature->GetGUID());

        return true;
    }
};

class npc_arcmage_pentarus : public CreatureScript
{
public:
    npc_arcmage_pentarus() : CreatureScript("npc_arcmage_pentarus") { }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*uiSender*/, uint32 /*uiAction*/)
    {
        player->CastSpell(player, 50987);
        return true;
    }
};

enum DalaranSquirl
{
	ITEM_SQUIRREL_NUT = 60002,
	SPELL_FEED_SQUIRREL = 61857,
};

class npc_squirrel : public CreatureScript
{
public:
    npc_squirrel() : CreatureScript("npc_squirrel") { }

    struct npc_squirrelAI : public ScriptedAI
    {
		npc_squirrelAI(Creature* creature) : ScriptedAI(creature), CD(urand(0, 15000)), follow(0), followTalk(0) { }

		uint32 CD;
		uint32 follow;
		uint32 followTalk;
		bool boomed;

		void Reset()
		{
			boomed = false;
		}

		void JustDied(Unit* who)
		{
			Talk(2);
		}

		void DamageTaken(Unit* attacker, uint32& damage)
		{
			if (damage >= me->GetHealth())
			{
				if (!boomed)
				{
					boomed = true;
					
					DoCastAOE(26027, true);
					DoCastAOE(60523, true);
				}
			}	
		}

		void DamageDealt(Unit* who, uint32& damage, DamageEffectType type)
		{
			damage = 0;
		}

		void SpellHit(Unit* caster, const SpellInfo* spell)
		{
			if (spell->Id == SPELL_FEED_SQUIRREL)
			{
				me->HandleEmoteCommand(EMOTE_ONESHOT_JUMPEND);
				me->RemoveAurasDueToSpell(SPELL_FEED_SQUIRREL);
				me->GetMotionMaster()->MoveFollow(caster, -0.5f, 0.0f);

				follow = 10000;
				followTalk = 6000;
				if (caster->ToPlayer())
					caster->ToPlayer()->DestroyItemCount(ITEM_SQUIRREL_NUT, 1, true);
			}
		}

		void UpdateAI(uint32 const diff)
		{
			if (followTalk)
			{
				if (followTalk <= diff)
				{
					Talk(1);

					followTalk = 0;
				}
				else
					followTalk -= diff;
			}
			
			if (follow)
			{
				if (follow <= diff)
				{
					me->GetMotionMaster()->MoveTargetedHome();

					follow = 0;
				}
				else
					follow -= diff;
				return;
			}

			if (CD)
			{
				if (CD <= diff)
					CD = 0;
				else
					CD -= diff;
			}
			if (!CD)
			{
				if (me->FindNearestPlayer(10.0f))
				{
					Talk(0);

					CD = urand(70000, 90000);
				}
			}
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_squirrelAI(creature);
    }
};

enum Dalaran_Laundry
{
	LAUNDRY_TALK_START_1 = 0,
	LAUNDRY_TALK_START_2,
	LAUNDRY_TALK_WATER,
	LAUNDRY_TALK_SHIRT,
	LAUNDRY_TALK_PANTS,
	LAUNDRY_TALK_UNDIES,
	LAUNDRY_TALK_CORRECT,
	LAUNDRY_TALK_INCORRECT,
	LAUNDRY_TALK_END_1,
	LAUNDRY_TALK_END_2,

	EVENT_LAUNDRY_TALK_START_2 = 1,
	EVENT_LAUNDRY_NEXT,
	EVENT_LAUNDRY_INCORRECT,
	EVENT_LAUNDRY_END_2,

	ACTION_START_LAUNDRY = 1,
	ACTION_END_LAUNDRY,
	
	GO_TABARD = 201384,
	
	GO_WATER_A = 201298,
	GO_SHIRT_A = 201296,
	GO_PANTS_A = 201295,
	GO_UNDIES_A = 201297,

	GO_WATER_H = 201855,
	GO_SHIRT_H = 201300,
	GO_PANTS_H = 201301,
	GO_UNDIES_H = 201299,

	SPELL_WATER = 69548,
	SPELL_SHIRT = 69543,
	SPELL_PANTS = 69542,
	SPELL_UNDIES = 69544,

	NPC_SHANDY_GLOSSBLEAM = 36856,
	NPC_AQUANOS = 36851,
};

static const uint32 laundryIDs [8] = 
{
	GO_WATER_A,
	GO_SHIRT_A,
	GO_PANTS_A,
	GO_UNDIES_A,

	GO_WATER_H,
	GO_SHIRT_H,
	GO_PANTS_H,
	GO_UNDIES_H
};

static const Position laundryPositions [4] = 
{
	{5806.797852f, 690.300903f, 659.149963f, 3.1f},
	{5804.944336f, 692.579529f, 657.948975f, 3.1f},
	{5804.622070f, 694.774780f, 657.948975f, 3.1f},
	{5804.517578f, 697.229553f, 657.948975f, 3.1f},
};

static const uint32 laundrySpells [4] =
{
	SPELL_WATER,
	SPELL_SHIRT,
	SPELL_PANTS,
	SPELL_UNDIES
};

class npc_shandy_glossgleam : public CreatureScript
{
public:
    npc_shandy_glossgleam() : CreatureScript("npc_shandy_glossgleam") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
		player->CLOSE_GOSSIP_MENU();
		creature->AI()->DoAction(ACTION_START_LAUNDRY);
		creature->AI()->SetGUID(player->GetGUID());

        return true;
    }

    struct npc_shandy_glossgleamAI : public ScriptedAI
    {
		npc_shandy_glossgleamAI(Creature* creature) : ScriptedAI(creature)
		{
			DoAction(ACTION_END_LAUNDRY);
		}
		
		EventMap events;
		uint64 playerGUID;
		uint8 successCount;
		uint8 next;
		uint8 team;
		bool waiting;

		void DoAction(int32 const action)
		{
			switch (action)
			{
				case ACTION_START_LAUNDRY:
					me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
					successCount = 0;
					waiting = false;
					Talk(LAUNDRY_TALK_START_1);
					events.Reset();
					events.ScheduleEvent(EVENT_LAUNDRY_TALK_START_2, 4500);
					break;
				case ACTION_END_LAUNDRY:
					me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
					for (uint8 i = 0; i < 8; i++)
						if (GameObject* go = me->FindNearestGameObject(laundryIDs[i], 15.0f))
							go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
					events.Reset();
					playerGUID = 0;
					playerGUID = 0;
					successCount = 0;
					waiting = false;
					break;
			}
		}

		void SetGUID(uint64 guid, int32 = 0)
		{
			playerGUID = guid;
			team = Unit::GetPlayer(*me, guid)->GetTeam() == TEAM_ALLIANCE ? 0 : 4;
		}

		void SetData(uint32 data, uint32 value)
		{
			if (!waiting)
				return;

			if (data == next)
			{
				Talk(LAUNDRY_TALK_CORRECT);
				events.CancelEvent(EVENT_LAUNDRY_INCORRECT);
				events.RescheduleEvent(EVENT_LAUNDRY_NEXT, 1000);
				successCount++;
				waiting = false;
			}
			else
				events.RescheduleEvent(EVENT_LAUNDRY_INCORRECT, 500);
		}

		void UpdateAI(uint32 const diff)
		{
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
					case EVENT_LAUNDRY_TALK_START_2:
						Talk(LAUNDRY_TALK_START_2);
						for (uint8 i = 0; i < 4; i++)
							if (GameObject* go = me->FindNearestGameObject(laundryIDs[i+team], 15.0f))
								go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
						events.ScheduleEvent(EVENT_LAUNDRY_NEXT, 10000);
						break;
					case EVENT_LAUNDRY_NEXT:
						if (successCount == 6)
						{
							Talk(LAUNDRY_TALK_END_1);
							events.ScheduleEvent(EVENT_LAUNDRY_END_2, 10000);
							break;
						}

						next = urand(0,3);
						switch (next)
						{
							case 0:
								Talk(LAUNDRY_TALK_WATER);
								break;
							case 1:
								Talk(LAUNDRY_TALK_SHIRT);
								break;
							case 2:
								Talk(LAUNDRY_TALK_PANTS);
								break;
							case 3:
								Talk(LAUNDRY_TALK_UNDIES);
								break;
						}
						waiting = true;
						events.ScheduleEvent(EVENT_LAUNDRY_NEXT, 10000);
						events.ScheduleEvent(EVENT_LAUNDRY_INCORRECT, 8000);
						break;
					case EVENT_LAUNDRY_INCORRECT:
						Talk(LAUNDRY_TALK_INCORRECT);
						successCount = 0;
						waiting = false;
						events.RescheduleEvent(EVENT_LAUNDRY_NEXT, 4000);
						break;
					case EVENT_LAUNDRY_END_2:
						Talk(LAUNDRY_TALK_END_2);
						me->SummonGameObject(GO_TABARD, 5802.22f, 691.556f, 657.949f, 3.50801f, 0.0f, 0.0f, 0.0f, 0.0f, 60);
						DoAction(ACTION_END_LAUNDRY);
						break;
				}
			}

			if (Player* player = Unit::GetPlayer(*me, playerGUID))
				if (player->GetPositionZ() < 657.5f || me->GetExactDist2d(player) > 15.0f)
					DoAction(ACTION_END_LAUNDRY);
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shandy_glossgleamAI(creature);
    }
};

class go_laundry : public GameObjectScript
{
	public:
		go_laundry() : GameObjectScript("go_laundry") { }

	bool OnGossipHello(Player *pPlayer, GameObject *pGO)
	{
		uint32 data = 0;
		switch (pGO->GetEntry())
		{
			case GO_WATER_A:
			case GO_WATER_H:
				break;//data = 0;
			case GO_SHIRT_A:
			case GO_SHIRT_H:
				data = 1;
				break;
			case GO_PANTS_A:
			case GO_PANTS_H:
				data = 2;
				break;
			case GO_UNDIES_A:
			case GO_UNDIES_H:
				data = 3;
				break;
			default:
				return false;
		}

		if (Creature* aquanos = pGO->FindNearestCreature(NPC_AQUANOS, 20.0f))
			if (Creature* shandy = pGO->FindNearestCreature(NPC_SHANDY_GLOSSBLEAM, 20.0f))
			{
				pPlayer->CastSpell(aquanos, laundrySpells[data], false);
				shandy->AI()->SetData(data, 0);
			}

		return true;
	};
};

void AddSC_dalaran()
{
    new npc_mageguard_dalaran;
    new npc_hira_snowdawn;
    new npc_arcmage_pentarus;
	new npc_squirrel;
	new npc_shandy_glossgleam;
	new go_laundry;
}
