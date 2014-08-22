/*
 * Copyright (C) 2008-2011 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: Zangarmarsh
SD%Complete: 100
SDComment: Quest support: 9752, 9785, 9803, 10009. Mark Of ... buffs.
SDCategory: Zangarmarsh
EndScriptData */

/* ContentData
npcs_ashyen_and_keleth
npc_cooshcoosh
npc_elder_kuruti
npc_mortog_steamhead
npc_kayra_longmane
npc_timothy_daniels
EndContentData */

#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"

/*######
## npcs_ashyen_and_keleth
######*/

#define GOSSIP_ITEM_BLESS_ASH     "Grant me your mark, wise ancient."
#define GOSSIP_ITEM_BLESS_KEL     "Grant me your mark, mighty ancient."
//signed for 17900 but used by 17900, 17901
#define GOSSIP_REWARD_BLESS       -1000359
//#define TEXT_BLESSINGS        "<You need higher standing with Cenarion Expedition to recive a blessing.>"

class npcs_ashyen_and_keleth : public CreatureScript
{
public:
    npcs_ashyen_and_keleth() : CreatureScript("npcs_ashyen_and_keleth") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetReputationRank(942) > REP_NEUTRAL)
        {
            if (creature->GetEntry() == 17900)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BLESS_ASH, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            if (creature->GetEntry() == 17901)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BLESS_KEL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        }
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
        {
            creature->setPowerType(POWER_MANA);
            creature->SetMaxPower(POWER_MANA, 200);             //set a "fake" mana value, we can't depend on database doing it in this case
            creature->SetPower(POWER_MANA, 200);

            if (creature->GetEntry() == 17900)                //check which Creature we are dealing with
            {
                switch (player->GetReputationRank(942))
                {                                               //mark of lore
                    case REP_FRIENDLY:
                        creature->CastSpell(player, 31808, true);
                        DoScriptText(GOSSIP_REWARD_BLESS, creature);
                        break;
                    case REP_HONORED:
                        creature->CastSpell(player, 31810, true);
                        DoScriptText(GOSSIP_REWARD_BLESS, creature);
                        break;
                    case REP_REVERED:
                        creature->CastSpell(player, 31811, true);
                        DoScriptText(GOSSIP_REWARD_BLESS, creature);
                        break;
                    case REP_EXALTED:
                        creature->CastSpell(player, 31815, true);
                        DoScriptText(GOSSIP_REWARD_BLESS, creature);
                        break;
                    default:
                        break;
                }
            }

            if (creature->GetEntry() == 17901)
            {
                switch (player->GetReputationRank(942))         //mark of war
                {
                    case REP_FRIENDLY:
                        creature->CastSpell(player, 31807, true);
                        DoScriptText(GOSSIP_REWARD_BLESS, creature);
                        break;
                    case REP_HONORED:
                        creature->CastSpell(player, 31812, true);
                        DoScriptText(GOSSIP_REWARD_BLESS, creature);
                        break;
                    case REP_REVERED:
                        creature->CastSpell(player, 31813, true);
                        DoScriptText(GOSSIP_REWARD_BLESS, creature);
                        break;
                    case REP_EXALTED:
                        creature->CastSpell(player, 31814, true);
                        DoScriptText(GOSSIP_REWARD_BLESS, creature);
                        break;
                    default:
                        break;
                }
            }
            player->CLOSE_GOSSIP_MENU();
            player->TalkedToCreature(creature->GetEntry(), creature->GetGUID());
        }
        return true;
    }
};

/*######
## npc_cooshcoosh
######*/

#define GOSSIP_COOSH            "You owe Sim'salabim money. Hand them over or die!"

enum eCooshhooosh
{
    SPELL_LIGHTNING_BOLT    = 9532,
    QUEST_CRACK_SKULLS      = 10009,
    FACTION_HOSTILE_CO      = 45
};

class npc_cooshcoosh : public CreatureScript
{
public:
    npc_cooshcoosh() : CreatureScript("npc_cooshcoosh") { }

    struct npc_cooshcooshAI : public ScriptedAI
    {
        npc_cooshcooshAI(Creature* c) : ScriptedAI(c)
        {
            m_uiNormFaction = c->getFaction();
        }

        uint32 m_uiNormFaction;
        uint32 LightningBolt_Timer;

        void Reset()
        {
            LightningBolt_Timer = 2000;
            if (me->getFaction() != m_uiNormFaction)
                me->setFaction(m_uiNormFaction);
        }

        void EnterCombat(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (LightningBolt_Timer <= diff)
            {
                DoCast(me->getVictim(), SPELL_LIGHTNING_BOLT);
                LightningBolt_Timer = 5000;
            } else LightningBolt_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cooshcooshAI (creature);
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_CRACK_SKULLS) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_COOSH, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        player->SEND_GOSSIP_MENU(9441, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        if (uiAction == GOSSIP_ACTION_INFO_DEF)
        {
            player->CLOSE_GOSSIP_MENU();
            creature->setFaction(FACTION_HOSTILE_CO);
            creature->AI()->AttackStart(player);
        }
        return true;
    }
};

/*######
## npc_elder_kuruti
######*/

#define GOSSIP_ITEM_KUR1 "Greetings, elder. It is time for your people to end their hostility towards the draenei and their allies."
#define GOSSIP_ITEM_KUR2 "I did not mean to deceive you, elder. The draenei of Telredor thought to approach you in a way that would seem familiar to you."
#define GOSSIP_ITEM_KUR3 "I will tell them. Farewell, elder."

class npc_elder_kuruti : public CreatureScript
{
public:
    npc_elder_kuruti() : CreatureScript("npc_elder_kuruti") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(9803) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KUR1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        player->SEND_GOSSIP_MENU(9226, creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KUR2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(9227, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KUR3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->SEND_GOSSIP_MENU(9229, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
            {
                if (!player->HasItemCount(24573, 1))
                {
                    ItemPosCountVec dest;
                    uint32 itemId = 24573;
                    InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemId, 1, NULL);
                    if (msg == EQUIP_ERR_OK)
                    {
                        player->StoreNewItem(dest, itemId, true);
                    }
                    else
                        player->SendEquipError(msg, NULL, NULL, itemId);
                }
                player->SEND_GOSSIP_MENU(9231, creature->GetGUID());
                break;
            }
        }
        return true;
    }
};

/*######
## npc_mortog_steamhead
######*/
class npc_mortog_steamhead : public CreatureScript
{
public:
    npc_mortog_steamhead() : CreatureScript("npc_mortog_steamhead") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->isVendor() && player->GetReputationRank(942) == REP_EXALTED)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        if (uiAction == GOSSIP_ACTION_TRADE)
            player->GetSession()->SendListInventory(creature->GetGUID());
        return true;
    }
};

/*######
## npc_kayra_longmane
######*/

enum eKayra
{
    SAY_START           = -1000343,
    SAY_AMBUSH1         = -1000344,
    SAY_PROGRESS        = -1000345,
    SAY_AMBUSH2         = -1000346,
    SAY_NEAR_END        = -1000347,
    SAY_END             = -1000348, //this is signed for 10646

    QUEST_ESCAPE_FROM   = 9752,
    NPC_SLAVEBINDER     = 18042
};

class npc_kayra_longmane : public CreatureScript
{
public:
    npc_kayra_longmane() : CreatureScript("npc_kayra_longmane") { }

    struct npc_kayra_longmaneAI : public npc_escortAI
    {
        npc_kayra_longmaneAI(Creature* c) : npc_escortAI(c) {}

        void Reset() { }

        void WaypointReached(uint32 i)
        {
            Player* player = GetPlayerForEscort();

            if (!player)
                return;

            switch (i)
            {
                case 4:
                    DoScriptText(SAY_AMBUSH1, me, player);
                    DoSpawnCreature(NPC_SLAVEBINDER, -10.0f, -5.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    DoSpawnCreature(NPC_SLAVEBINDER, -8.0f, 5.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    break;
                case 5:
                    DoScriptText(SAY_PROGRESS, me, player);
                    SetRun();
                    break;
                case 16:
                    DoScriptText(SAY_AMBUSH2, me, player);
                    DoSpawnCreature(NPC_SLAVEBINDER, -10.0f, -5.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    DoSpawnCreature(NPC_SLAVEBINDER, -8.0f, 5.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    break;
                case 17:
                    SetRun(false);
                    DoScriptText(SAY_NEAR_END, me, player);
                    break;
                case 25:
                    DoScriptText(SAY_END, me, player);
                    player->GroupEventHappens(QUEST_ESCAPE_FROM, me);
                    break;
            }
        }
    };

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_ESCAPE_FROM)
        {
            DoScriptText(SAY_START, creature, player);

            if (npc_escortAI* pEscortAI = CAST_AI(npc_kayra_longmane::npc_kayra_longmaneAI, creature->AI()))
                pEscortAI->Start(false, false, player->GetGUID());
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_kayra_longmaneAI(creature);
    }
};

/*######
## npc_timothy_daniels
######*/

#define GOSSIP_TIMOTHY_DANIELS_ITEM1    "Specialist, eh? Just what kind of specialist are you, anyway?"
#define GOSSIP_TEXT_BROWSE_POISONS      "Let me browse your reagents and poison supplies."

enum eTimothy
{
    GOSSIP_TEXTID_TIMOTHY_DANIELS1      = 9239
};

class npc_timothy_daniels : public CreatureScript
{
public:
    npc_timothy_daniels() : CreatureScript("npc_timothy_daniels") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->isQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (creature->isVendor())
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_POISONS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TIMOTHY_DANIELS_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_TIMOTHY_DANIELS1, creature->GetGUID());
                break;
            case GOSSIP_ACTION_TRADE:
                player->GetSession()->SendListInventory(creature->GetGUID());
                break;
        }

        return true;
    }
};

/*######
## SLAVE PENS BOSS AHUNE
######*/

enum Creatures
{
    NPC_AHUNE                   = 25740,
    NPC_AHUNE_HC                = 26338,
    NPC_FROZEN_CORE             = 25865,
    NPC_FROZEN_CORE_HC          = 26339,
    NPC_AHUNITE_COLDWEAVE       = 25756,
    NPC_AHUNITE_COLDWEAVE_HC    = 26340,
    NPC_AHUNITE_FROSTWIND       = 25757,
    NPC_AHUNITE_FROSTWIND_HC    = 26341,
    NPC_AHUNITE_HAILSTONE       = 25755,
    NPC_AHUNITE_HAILSTONE_HC    = 26342,
	NPC_ICE_SPEAR_BUNNY         = 25985,
};

enum Spells
{
    // Ahune
    SPELL_AHUNES_SHIELD         = 45954,
    SPELL_COLD_SLAP             = 30056,
	SPELL_SUMMON_ICE_SPEAR      = 46359,

    SPELL_MAKE_BONFIRE          = 45930,
    SPELL_SUMMONING_VISUAL1     = 45937, // below Ahune (?)
    SPELL_SUMMONING_VISUAL2     = 45938, // below the Summoning Stone (?)
    SPELL_SUMMON_MINION_VISUAL  = 46103, // Midsummer - Ahune - Summon Minion, Lower
    SPELL_GHOST_VISUAL          = 46786,
    SPELL_RESURFACE             = 46402, // Ahune Resurfaces

    SPELL_LOOT_CHEST            = 45939,
    SPELL_LOOT_CHEST_HC         = 46622,

    SPELL_AHUNE_ACHIEVEMENT     = 62043, // Midsummer - Ahune - DIES, Achievement

    // Coldweave
    SPELL_BITTER_BLAST          = 46406,

    // Frostwind
    SPELL_LIGHTNING_SHIELD      = 12550,
    SPELL_WIND_BUFFET           = 46568,

    // Hailstone
    SPELL_CHILLING_AURA         = 46542,
    SPELL_PULVERIZE             = 2676,

	//Ice Spear
	SPELL_ICE_SPEAR_VISUAL      = 75498,
	SPELL_SUMMON_GO             = 46369,
	SPELL_ICE_SPEAR_EXPLOSION   = 46360,

};

enum Events
{
    // Ahune
    EVENT_SWITCH_PHASE = 1,
	EVENT_HIDE_AHUNE,
    EVENT_SUMMON_HAILSTONE,
    EVENT_SUMMON_COLDWEAVE,
    EVENT_SUMMON_FROSTWIND,
    EVENT_ICE_SPEAR,
    EVENT_COLD_SLAP,

    // Frozen Core
    EVENT_GHOST_VISUAL,
    EVENT_RESURFACE_SOON,

    // Coldweave
    EVENT_BITTER_BLAST,

    // Frostwind
    EVENT_WIND_BUFFET,

    // Hailstone
    EVENT_CHILLING_AURA,
    EVENT_PULVERIZE,

	//Ice Spear
	EVENT_ICE_SPEAR_EXPLOSION,
	EVENT_ICE_SPEAR_DESPAWN,
};

enum Achievements
{
	ACHIEVEMENT_SLAY_ANUHE = 263
};

#define GOSSIP_SUMMON "Disturb the Stone and summon Lord Ahune.";

// After GOSSIP_SUMMON was clicked
// Player says: The Ice stone has melted.
// Player says: Ahune, your strength grows no more!
// Player says: Your frozen reign will not come to pass!

Position const SummonPositions[4] =
{
        {-88.495071f, -254.462997f, -1.077302f, 5.0f}, // Ahune / Frozen-Core
        {-90.891891f, -243.488068f, -1.116222f}, // Hailstone
        {-97.389175f, -239.780701f, -1.264044f}, // Coldweave #1
        {-85.160637f, -236.127808f, -1.572418f}, // Coldweave #2
};

class boss_ahune : public CreatureScript
{
    public:
        boss_ahune() : CreatureScript("boss_ahune") { }

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_ahuneAI(creature);
        }

        struct boss_ahuneAI : public Scripted_NoMovementAI
        {
            boss_ahuneAI(Creature* creature) : Scripted_NoMovementAI(creature), summons(me) { }
			
            EventMap events;
            SummonList summons;
			bool secondPhase;
            uint64 frozenCoreGUID;

            void Reset()
            {
                summons.DespawnAll();

                events.Reset();
				
				secondPhase = false;
                frozenCoreGUID = 0;

                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
				me->SetVisible(true);
            }
			
			void IsSummonedBy(Unit* summoner)
			{
				me->HandleEmoteCommand(EMOTE_ONESHOT_EMERGE);
			}

            void EnterCombat(Unit* who)
            {
                DoZoneInCombat();

				DoCast(SPELL_AHUNES_SHIELD);

                events.Reset();
				events.ScheduleEvent(EVENT_SWITCH_PHASE, 90000); // phase 2 after 90 seconds
                events.ScheduleEvent(EVENT_COLD_SLAP, 800); // every 800ms in melee range in phase 1
                events.ScheduleEvent(EVENT_SUMMON_HAILSTONE, 1000); // once in phase 1
                events.ScheduleEvent(EVENT_SUMMON_COLDWEAVE, 6000); // every 5 seconds in phase 1
                events.ScheduleEvent(EVENT_ICE_SPEAR, 9500); // every 7 seconds in phase 1, first after 9.5 seconds
				
            }

            void JustDied(Unit* killer)
            {
				summons.DespawnAll();

				if (me->GetInstanceScript())
					me->GetInstanceScript()->DoCompleteAchievement(ACHIEVEMENT_SLAY_ANUHE);
				
				if (Creature* bunny = me->SummonCreature(25745, me->GetPositionX(), me->GetPositionY() + 15.0f, me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 604800000))
					if (GameObject* go = bunny->SummonGameObject(me->GetMap()->IsHeroic() ? 188192 : 187892, me->GetPositionX(), me->GetPositionY() + 15.0f, me->GetPositionZ(), me->GetOrientation(), 0, 0, 0, 0, 604800000))
					{
						go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
						go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
						go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
						go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
					}

				//me->DespawnOrUnsummon(12000);
            }

            void JustSummoned(Creature* summoned)
            {
				if (summoned->GetEntry() == NPC_ICE_SPEAR_BUNNY)
					return;
                DoZoneInCombat(summoned);
                summons.Summon(summoned);
            }

			void SummonedCreatureDies(Creature* summon, Unit* killer)
			{
				if (summon->GetEntry() == DUNGEON_MODE(NPC_FROZEN_CORE, NPC_FROZEN_CORE_HC))
				{
					killer->Kill(me);
				}
			}

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
						case EVENT_HIDE_AHUNE:
							me->SetVisible(false);
							break;
                        case EVENT_SWITCH_PHASE:
                            if (!secondPhase)
                            {
								events.Reset();
                                events.ScheduleEvent(EVENT_SWITCH_PHASE, 30000);
                                events.ScheduleEvent(EVENT_HIDE_AHUNE, 1500);
								events.ScheduleEvent(EVENT_ICE_SPEAR, 9500);
								secondPhase = true;

                                me->SetReactState(REACT_PASSIVE);
                                me->AttackStop();
                                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                                me->HandleEmoteCommand(EMOTE_ONESHOT_SUBMERGE);

                                // Emote: Ahune retreats. His defenses diminish.

                                // spawn core
                                if (Unit* frozenCore = me->SummonCreature(DUNGEON_MODE(NPC_FROZEN_CORE, NPC_FROZEN_CORE_HC), *me, TEMPSUMMON_CORPSE_DESPAWN))
                                {
                                    frozenCoreGUID = frozenCore->GetGUID();
									frozenCore->SetMaxHealth(me->GetMaxHealth());
                                    frozenCore->SetHealth(me->GetHealth()); // sync health on phase change

									frozenCore->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
									frozenCore->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
                                }
                            }
                            else
                            {
								events.Reset();
								events.ScheduleEvent(EVENT_SWITCH_PHASE, 90000); // phase 2 after 90 seconds
								events.ScheduleEvent(EVENT_COLD_SLAP, 800); // every 800ms in melee range in phase 1
								events.ScheduleEvent(EVENT_SUMMON_HAILSTONE, 1000); // once in phase 1
								events.ScheduleEvent(EVENT_SUMMON_COLDWEAVE, 6000); // every 5 seconds in phase 1
								events.ScheduleEvent(EVENT_ICE_SPEAR, 9500);
								
								secondPhase = false;

                                me->SetReactState(REACT_AGGRESSIVE);
                                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
								me->SetVisible(true);
                                me->HandleEmoteCommand(EMOTE_ONESHOT_EMERGE);

                                // despawn core
                                if (Creature* frozenCore = me->GetCreature(*me, frozenCoreGUID))
								{
									me->SetHealth(frozenCore->GetHealth());
                                    frozenCore->DespawnOrUnsummon(0);
								}
                            }
                            break;
                        case EVENT_COLD_SLAP:
                            if (Unit* target = SelectTarget(SELECT_TARGET_NEAREST, 0, 8.0f, true))
                                DoCast(target, SPELL_COLD_SLAP);
                            events.ScheduleEvent(EVENT_COLD_SLAP, 500);
                            break;
                        case EVENT_ICE_SPEAR:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
								DoCast(target, SPELL_SUMMON_ICE_SPEAR);
							events.ScheduleEvent(EVENT_ICE_SPEAR, 9000);
                            break;
                        case EVENT_SUMMON_HAILSTONE:
                            me->SummonCreature(DUNGEON_MODE(NPC_AHUNITE_HAILSTONE, NPC_AHUNITE_HAILSTONE_HC), SummonPositions[1], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000);
							events.ScheduleEvent(EVENT_SUMMON_HAILSTONE, 40000);
                            break;
                        case EVENT_SUMMON_COLDWEAVE: // they always come in pairs of two
                            me->SummonCreature(DUNGEON_MODE(NPC_AHUNITE_COLDWEAVE, NPC_AHUNITE_COLDWEAVE_HC), SummonPositions[2], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000);
                            me->SummonCreature(DUNGEON_MODE(NPC_AHUNITE_COLDWEAVE, NPC_AHUNITE_COLDWEAVE_HC), SummonPositions[3], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000);
                            events.ScheduleEvent(EVENT_SUMMON_COLDWEAVE, 5000);
                            break;
                    }
                }
            }
        };
};

class npc_ahunite_hailstone : public CreatureScript
{
    public:
        npc_ahunite_hailstone() : CreatureScript("npc_ahunite_hailstone") { }

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_ahunite_hailstoneAI(creature);
        }

        struct npc_ahunite_hailstoneAI : public ScriptedAI
        {
            npc_ahunite_hailstoneAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_PULVERIZE, urand(6000, 8000));
                events.ScheduleEvent(EVENT_CHILLING_AURA, 10000); // TODO: get correct timing

                me->AddAura(SPELL_CHILLING_AURA, me);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_PULVERIZE:
                            DoCastVictim(SPELL_PULVERIZE);
                            events.ScheduleEvent(EVENT_PULVERIZE, urand(6000, 8000));
                            break;
                        case EVENT_CHILLING_AURA:
                            if (Aura* chillingAura = me->GetAura(SPELL_CHILLING_AURA))
                                if (chillingAura->GetStackAmount() < 10)
                                    chillingAura->SetStackAmount(chillingAura->GetStackAmount() + 1);
                            events.ScheduleEvent(EVENT_CHILLING_AURA, 10000);
                            break;
                    }
                }
            }
        };
};

class npc_ahunite_coldweave : public CreatureScript
{
    public:
        npc_ahunite_coldweave() : CreatureScript("npc_ahunite_coldweave") { }

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_ahunite_coldweaveAI(creature);
        }

        struct npc_ahunite_coldweaveAI : public ScriptedAI
        {
            npc_ahunite_coldweaveAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_BITTER_BLAST, 500);
            }

            void AttackStart(Unit* who)
            {
                AttackStartCaster(who, 10.0f); // keep distance to target, because we only do ranged attacks
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BITTER_BLAST:
                            DoCastVictim(SPELL_BITTER_BLAST);
                            events.ScheduleEvent(EVENT_BITTER_BLAST, urand(5000, 7000));
                            break;
                    }
                }
            }
        };
};

class npc_ahunite_frostwind : public CreatureScript
{
    public:
        npc_ahunite_frostwind() : CreatureScript("npc_ahunite_frostwind") { }

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_ahunite_frostwindAI(creature);
        }

        struct npc_ahunite_frostwindAI : public ScriptedAI
        {
            npc_ahunite_frostwindAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_WIND_BUFFET, 2000); // TODO: get correct timing for wind buffet

                DoCast(me, SPELL_LIGHTNING_SHIELD);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_WIND_BUFFET:
                            if (Unit* target = SelectTarget(SELECT_TARGET_FARTHEST, 0, 120.0f))
                                DoCast(target, SPELL_WIND_BUFFET);
                            events.ScheduleEvent(EVENT_WIND_BUFFET, urand(5000, 7000));
                            break;
                    }
                }
            }
        };
};

class go_ice_stone : public GameObjectScript
{
public:
	go_ice_stone() : GameObjectScript("go_ice_stone") { }

	bool OnQuestReward(Player* player, GameObject* object, const Quest* quest, uint32 nevim)
	{
		player->SummonCreature(player->GetMap()->IsHeroic() ? NPC_AHUNE_HC : NPC_AHUNE, SummonPositions[0]);
		object->Delete();
		return true;
	}
};

class npc_ice_spear_bunny : public CreatureScript
{
    public:
        npc_ice_spear_bunny() : CreatureScript("npc_ice_spear_bunny") { }

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_ice_spear_bunnyAI(creature);
        }

		struct npc_ice_spear_bunnyAI : public NullCreatureAI
        {
            npc_ice_spear_bunnyAI(Creature* creature) : NullCreatureAI(creature) { }

            EventMap events;

            void Reset()
            {
                events.Reset();
				events.ScheduleEvent(EVENT_ICE_SPEAR_EXPLOSION, 5000);
				DoCast(me, SPELL_SUMMON_GO, true);
				if (GameObject* spear = me->FindNearestGameObject(188077, 5.0f))
				{
					spear->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
				}
				DoCast(me, SPELL_ICE_SPEAR_VISUAL, true);
            }

            void UpdateAI(const uint32 diff)
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
						case EVENT_ICE_SPEAR_EXPLOSION:
							if (GameObject* spear = me->FindNearestGameObject(188077, 5.0f))
							{
								DoCastAOE(SPELL_ICE_SPEAR_EXPLOSION);
								spear->Use(me);
							}
							events.ScheduleEvent(EVENT_ICE_SPEAR_DESPAWN, 3000);
							break;
						case EVENT_ICE_SPEAR_DESPAWN:
							if (GameObject* spear = me->FindNearestGameObject(188077, 5.0f))
							{
								spear->Delete();
								me->DespawnOrUnsummon();
							}
							break;
                    }
                }
            }
        };
};

/*######
## AddSC
######*/

void AddSC_zangarmarsh()
{
    new npcs_ashyen_and_keleth();
    new npc_cooshcoosh();
    new npc_elder_kuruti();
    new npc_mortog_steamhead();
    new npc_kayra_longmane();
    new npc_timothy_daniels();
    // AHUNE
    new boss_ahune();
    new npc_ahunite_hailstone();
    new npc_ahunite_coldweave();
    new npc_ahunite_frostwind();
	new go_ice_stone();
	new npc_ice_spear_bunny();
}
