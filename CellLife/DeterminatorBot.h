#pragma once
#include <array>
#include <memory>
#include "CellBot.h"
#include "CellWorldPhysics.h"

#define DETERMINATOR_BRAIN_SIZE (256)
#define DETERMINATOR_MAX_COMMANDS (256)

#define DETERMINATOR_MAX_ENERGY (1000.0f)
#define DETERMINATOR_MAX_HP (100.0f)

/* Every action costs energy, even cmd interpreter step */
//Idle cost = "basic cost of living", drains once every turn
#define ENERGY_IDLE_COST (1.0f)
#define ENERGY_INTERPRETATION_COST (0.1f)
#define ENERGY_DIVISION_COST (5.0f)
#define ENERGY_MOVEMENT_COST (1.0f)

/* When energy is 0, bot will spend HP instead of energy */
#define ENERGY_HUNGRY_THRESHOLD (100.0f)
#define HP_HUNGRY_DRAIN (1.0f)


/*
	Determinator bot with simple determenistic program in its brain.
	Properties:
	HP - Health Points - draining from low energy. If becomes 0 - bot is dead.
	Energy - "adenosine triphosphate"; every movement and every tick drains energy

	Each command is 1 byte [0-255]
	Program length: DETERMINATOR_BRAIN_SIZE
	Each "non-condition" command is terminating program and make a corresponding turn
	Each "condition" command moves command pointer, and maximum number of hopes is DETERMINATOR_MAX_COMMANDS (but there is loop detection)
	Command list:

	0 -  nothing to do, null command
	
	1 -  cell division (available empty cell, starting from N side)
	2 -  cell division (available empty cell, starting from E side)
	3 -  cell division (available empty cell, starting from S side)
	4 -  cell division (available empty cell, starting from W side)
	
	5 -  consume sun energy (photosynthesis)
	
	6 -  move N direction
	7 -  move E direction
	8 -  move S direction
	9 -  move W direction
	10 - attack N direction (eat if success)
	11 - attack E direction (eat if success)
	12 - attack S direction (eat if success)
	13 - attack W direction (eat if success)
	14 - defend from N direction
	15 - defend from E direction
	16 - defend from S direction
	17 - defend from W direction
	18 - heal to N direction (recover other bot's HP for energy)
	19 - heal to E direction
	20 - heal to S direction
	21 - heal to W direction
	22 - feed to N direction (give energy to bot)
	23 - feed to E direction
	24 - feed to S direction
	25 - feed to W direction

	26 - heal itself for energy
	27 - grow HP (increase maximum HP for energy)
	28 - grow leaves (increase photosynthesis power for energy)
	29 - grow claws (increase attack power for energy)
	30 - grow armor (increase defense power for energy)
	31 - repeat last successful command
	
	32 - (condition) depth is lower than
	33 - (condition) depth is higher than
	34 - (condition) depth is between 2 values
	35 - (condition) HP is low
	36 - (condition) HP is falling down
	37 - (condition) HP is raising up
	38 - (condition) Energy is low
	39 - (condition) Energy is falling down
	40 - (condition) Energy is raising up
	
	41 - (condition) look at N direction (options: empty, wall, organic (dead body), bot, bot relative (the same species))
	42 - (condition) look at E direction
	43 - (condition) look at S direction
	44 - (condition) look at W direction
	
	45 - (condition) look for empty cell (options: N, E, S, W, not found) - first found direction will be executed
	46 - (condition) look for wall
	47 - (condition) look for organic
	48 - (condition) look for bot
	49 - (condition) look for bot relative

	50 - (condition) goto absolute address
	51 - (condition) goto relative address
*/

enum BotCommand {
	CMD_NOTHING = 0,  //nothing to do, null command

	CMD_DIVISION_N, // cell division(available empty cell, starting from N side)
	CMD_DIVISION_E, //cell division(available empty cell, starting from E side)
	CMD_DIVISION_S, //cell division(available empty cell, starting from S side)
	CMD_DIVISION_W, //cell division(available empty cell, starting from W side)

	CMD_PHOTOSYNTHESIS, //consume sun energy(photosynthesis)

	CMD_MOVE_N, //move N direction
	CMD_MOVE_E, //move E direction
	CMD_MOVE_S, //move S direction
	CMD_MOVE_W, //move W direction
	CMD_ATTACK_N, //attack N direction(eat if success)
	CMD_ATTACK_E, //attack E direction(eat if success)
	CMD_ATTACK_S, //attack S direction(eat if success)
	CMD_ATTACK_W, //attack W direction(eat if success)
	CMD_DEFEND_N, //defend from N direction
	CMD_DEFEND_E, //defend from E direction
	CMD_DEFEND_S, //defend from S direction
	CMD_DEFEND_W, //defend from W direction
	CMD_HEAL_N, //heal to N direction(recover other bot's HP for energy)
	CMD_HEAL_E, //heal to E direction
	CMD_HEAL_S, //heal to S direction
	CMD_HEAL_W, //heal to W direction
	CMD_FEED_N, //feed to N direction(give energy to bot)
	CMD_FEED_E, //feed to E direction
	CMD_FEED_S, //feed to S direction
	CMD_FEED_W, //feed to W direction

	CMD_SELF_HEAL, //heal itself for energy
	CMD_GROW_HP, //grow HP(increase maximum HP for energy)
	CMD_GROW_LEAVES, //grow leaves(increase photosynthesis power for energy)
	CMD_GROW_CLAWS, //grow claws(increase attack power for energy)
	CMD_GROW_ARMOR, //grow armor(increase defense power for energy)
	CMD_REPEAT_LAST, //repeat last successful command

	CMD_COND_DEPTH_LOWER_THAN, //(condition)depth is lower than
	CMD_COND_DEPTH_HIGHER_THAN, //(condition)depth is higher than
	CMD_COND_DEPTH_BETWEEN, //(condition)depth is between 2 values
	CMD_COND_HP_LOW, //(condition)HP is low
	CMD_COND_HP_FALLING_DOWN, //(condition)HP is falling down
	CMD_COND_HP_RAISING_UP, //(condition)HP is raising up
	CMD_COND_ENERGY_LOW, //(condition)Energy is low
	CMD_COND_ENERGY_FALLING_DOWN, //(condition)Energy is falling down
	CMD_COND_ENERGY_RAISING_UP, //(condition)Energy is raising up

	CMD_COND_LOOK_AT_N, //(condition)look at N direction(options: empty, wall, organic(dead body), bot, bot relative(the same species))
	CMD_COND_LOOK_AT_E, //(condition)look at E direction
	CMD_COND_LOOK_AT_S, //(condition)look at S direction
	CMD_COND_LOOK_AT_W, //(condition)look at W direction

	CMD_COND_LOOK_FOR_EMPTY, //(condition)look for empty cell(options: N, E, S, W, not found) - first found direction will be executed
	CMD_COND_LOOK_FOR_WALL, //(condition)look for wall
	CMD_COND_LOOK_FOR_ORGANIC, //(condition)look for organic
	CMD_COND_LOOK_FOR_BOT, //(condition)look for bot
	CMD_COND_LOOK_FOR_RELATIVE, //(condition)look for bot relative

	CMD_GOTO_ABSOLUTE, //goto absolute address
	CMD_GOTO_RELATIVE //goto relative address
};

using namespace std;

typedef array<unsigned char, DETERMINATOR_BRAIN_SIZE> DeterminatorBrain;

struct DeterminatorConstants
{
public:
	//int SunEnergyEffectiveness;
};

class DeterminatorBot :
	public CellBot
{
protected:
	DeterminatorBrain brain;
	unsigned int cmd_ptr;
	unsigned int agressiveMovesCount;
	unsigned int passiveMovesCount;

	float hp, hp_delta;
	float energy, energy_delta;

	float photoEfficiency;
	float attackPower;
	float defensePower;

	virtual void photosynthesis(float sunEnergy);
	virtual void division(CellWorld *world, unsigned int new_x, unsigned int new_y);
	void randomDivision(CellWorld *world);

public:
	using CellBot::CellBot;

	DeterminatorBot() : cmd_ptr(0), agressiveMovesCount(0), passiveMovesCount(0),
						hp(100.0f), hp_delta(0.0f), energy(DETERMINATOR_MAX_ENERGY), energy_delta(0.0f),
						photoEfficiency(50.0f), attackPower(50.0f), defensePower(45.0f)
	{
		brain.fill(CMD_PHOTOSYNTHESIS); //Simpliest initial stupid bot: just photosynthesis to avoid dying
	}
	
	DeterminatorBot(DeterminatorBrain &Brain) : cmd_ptr(0), agressiveMovesCount(0), passiveMovesCount(0),
		hp(100.0f), hp_delta(0.0f), energy(DETERMINATOR_MAX_ENERGY), energy_delta(0.0f), 
		photoEfficiency(50.0f), attackPower(50.0f), defensePower(45.0f)
	{
		brain = Brain;
	}

	virtual void Render(HDC hdc, int x, int y, int cellSize, int levelOfDetails) const;
	virtual void RenderBotInfo(HDC hdc, int x, int y) const;

	virtual void nextMove(CellWorld *world);
	virtual void updateStatus(CellWorld *world); //check HP, energy, etc

	virtual DeterminatorBrain getBrain() { return brain; }
	virtual void setBrain(DeterminatorBrain &Brain) { brain = Brain; }

	unsigned int getCmdPtr() { return cmd_ptr; }
	void setCmdPtr(unsigned int in_cmd_ptr) { cmd_ptr = in_cmd_ptr % DETERMINATOR_BRAIN_SIZE; }

	float getHP() { return hp; }
	float getHPDelta() { return hp_delta; }
	void setHP(float HP) { hp = HP; }

	float getEnergy() { return energy; }
	float getEnergyDelta() { return energy_delta; }
	void setEnergy(float Energy) { energy = Energy; }

	float getPhotoEfficiency() { return photoEfficiency; }
	void setPhotoEfficiency(float PhotoEfficiency) { photoEfficiency = PhotoEfficiency; }
	float getAttackPower() { return attackPower; }
	void setAttackPower(float AttackPower) { attackPower = AttackPower; }
	float getDefensePower() { return defensePower; }
	void setDefensePower(float DefensePower) { defensePower = DefensePower; }
};

typedef shared_ptr<DeterminatorBot> sharedDeterminatorBot;
typedef weak_ptr<DeterminatorBot> weakDeterminatorBot;