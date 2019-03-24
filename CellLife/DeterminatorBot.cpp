#include "stdafx.h"
#include "CellWorld.h"
#include "DeterminatorBot.h"

using namespace std;

//int DeterminatorBot::getColorR() const 
//{
//	return 0;
//}
//
//int DeterminatorBot::getColorG() const
//{
//	return 0x80 + (energy / DETERMINATOR_MAX_ENERGY * 0x80);
//}
//
//int DeterminatorBot::getColorB() const
//{
//	return 0;
//}

void DeterminatorBot::Render(HDC hdc, int x, int y, int cellSize, int levelOfDetails = 0) const
{
	HBRUSH hCellBrush = CreateSolidBrush(RGB(
		0, 
		0x7F + (energy / DETERMINATOR_MAX_ENERGY * 0x80), 
		0));

	SelectObject(hdc, hCellBrush);
	Rectangle(hdc, x, y, x + cellSize + 1, y + cellSize + 1);
	DeleteObject(hCellBrush);

	if (levelOfDetails > 0)
	{
		HFONT newFont = CreateFont((unsigned int)round(cellSize / 3.0f),
			0, 0, 0, 500, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Courier New");
		HFONT oldFont = (HFONT)SelectObject(hdc, newFont);

		SetBkMode(hdc, TRANSPARENT);

		RECT rc = { x + 1, y + 1, x + cellSize, y + cellSize };
		int delta = (int)round(energy_delta);
		wstring txt = to_wstring((int)energy) +
			((delta >= 0) ? L"+" : L"") + 
			to_wstring(delta);
		
		SetTextColor(hdc, RGB(0xff, 0xff, 0));
		DrawText(hdc, txt.c_str(), txt.length(), &rc, DT_TOP | DT_LEFT | DT_SINGLELINE);

		delta = (int)round(hp_delta);
		txt = to_wstring((int)hp) +
			((delta >= 0) ? L"+" : L"") +
			to_wstring(delta);

		SetTextColor(hdc, RGB(0xff, 0, 0));
		DrawText(hdc, txt.c_str(), txt.length(), &rc, DT_BOTTOM | DT_SINGLELINE);

		SelectObject(hdc, oldFont);
		DeleteObject(newFont);
	}
}

void DeterminatorBot::RenderBotInfo(HDC hdc, int x, int y) const
{
	
}

void DeterminatorBot::photosynthesis(float sunEnergy)
{
	energy += sunEnergy * photoEfficiency / 100.0f;
}

void DeterminatorBot::updateStatus(CellWorld *world)
{	
	if (energy < 0.0f) {
		hp += energy;
		energy = 0.0f;
	}

	if (hp < 0.0f)
	{
		hp = 0.0f;
		status = BOT_DEAD;
	}
}

void DeterminatorBot::randomDivision(CellWorld *world) //Division in random direction if possible
{
	vector<unsigned int> new_x, new_y;

	if (CELL_EMPTY == world->getStatus(x, y - 1)) //N
	{
		new_x.push_back(x);
		new_y.push_back(y - 1);
	}
	if (CELL_EMPTY == world->getStatus(x + 1, y)) //E
	{
		new_x.push_back(x + 1);
		new_y.push_back(y);
	}
	if (CELL_EMPTY == world->getStatus(x, y + 1)) //S
	{
		new_x.push_back(x);
		new_y.push_back(y + 1);
	}
	if (CELL_EMPTY == world->getStatus(x - 1, y)) //W
	{
		new_x.push_back(x - 1);
		new_y.push_back(y);
	}

	if (new_x.size() > 0) //Non-zero places to division
	{
		unsigned int rnd_index = world->getRandomNumber() % new_x.size();
		division(world, new_x[rnd_index], new_y[rnd_index]);
	}		
}

#define MUTATION_PROBABILITY_SIMPLE (0.05f)

void DeterminatorBot::division(CellWorld *world, unsigned int new_x, unsigned int new_y)
{
	DeterminatorBrain new_brain = brain;
	float child_energy = 0.0f;

	//simple mutation (1 random command -> new value)
	unsigned int chances = (unsigned int)(1.0f / MUTATION_PROBABILITY_SIMPLE); //just 1 chance from how many?
	if (0 == world->getRandomNumber() % chances) //implements this probability for mutation
	{
		unsigned int mutation_index = world->getRandomNumber() % DETERMINATOR_BRAIN_SIZE;
		unsigned int new_cmd = world->getRandomNumber() % DETERMINATOR_BRAIN_SIZE;
		new_brain[mutation_index] = new_cmd;
	}

	child_energy = energy / 2.0f; //Simpliest case: descendant gains half og the parent's energy
	
	sharedDeterminatorBot new_bot = make_shared<DeterminatorBot>(new_brain);
	new_bot->setEnergy(child_energy); 
	world->AddBot(new_x, new_y, new_bot);

	energy -= child_energy;
	energy -= ENERGY_DIVISION_COST;
}

void DeterminatorBot::nextMove(CellWorld *world)
{
	float initial_hp = hp;
	float initial_energy = energy;

	for (int i = 0; i < DETERMINATOR_MAX_COMMANDS; ++i)
	{
		bool terminal_cmd = false; //Termination flag
		unsigned int new_cmd_ptr = cmd_ptr + 1; //next cmd_ptr; can be changed by goto and "if" commands

		switch (brain[cmd_ptr]) {
		case BotCommand::CMD_NOTHING:			
			break;

		case BotCommand::CMD_PHOTOSYNTHESIS:			
			photosynthesis(world->getWorldPhysics()->getSunPower(y));
			terminal_cmd = true;
			break;

		case BotCommand::CMD_DIVISION_N:			
			if (CELL_EMPTY == world->getStatus(x, y - 1))
				division(world, x, y - 1);
			else if (CELL_EMPTY == world->getStatus(x + 1, y))
				division(world, x + 1, y);
			else if (CELL_EMPTY == world->getStatus(x, y + 1))
				division(world, x, y + 1);
			else if (CELL_EMPTY == world->getStatus(x - 1, y))
				division(world, x - 1, y);

			terminal_cmd = true;
			break;

		case BotCommand::CMD_DIVISION_E:
			if (CELL_EMPTY == world->getStatus(x + 1, y))
				division(world, x + 1, y);
			else if (CELL_EMPTY == world->getStatus(x, y + 1))
				division(world, x, y + 1);
			else if (CELL_EMPTY == world->getStatus(x - 1, y))
				division(world, x - 1, y);
			else if (CELL_EMPTY == world->getStatus(x, y - 1))
				division(world, x, y - 1);

			terminal_cmd = true;
			break;

		case BotCommand::CMD_DIVISION_S:
			if (CELL_EMPTY == world->getStatus(x, y + 1))
				division(world, x, y + 1);
			else if (CELL_EMPTY == world->getStatus(x - 1, y))
				division(world, x - 1, y);
			else if (CELL_EMPTY == world->getStatus(x, y - 1))
				division(world, x, y - 1);
			else if (CELL_EMPTY == world->getStatus(x + 1, y))
				division(world, x + 1, y);

			terminal_cmd = true;
			break;

		case BotCommand::CMD_DIVISION_W:
			if (CELL_EMPTY == world->getStatus(x - 1, y))
				division(world, x - 1, y);
			else if (CELL_EMPTY == world->getStatus(x, y - 1))
				division(world, x, y -1);
			else if (CELL_EMPTY == world->getStatus(x + 1, y))
				division(world, x + 1, y);
			else if (CELL_EMPTY == world->getStatus(x, y + 1))
				division(world, x, y + 1);

			terminal_cmd = true;
			break;

		case BotCommand::CMD_GOTO_ABSOLUTE:
			new_cmd_ptr = brain[(cmd_ptr + 1) % DETERMINATOR_BRAIN_SIZE]; //new address of cmd pointer			
			break;

		case BotCommand::CMD_GOTO_RELATIVE:
			new_cmd_ptr = cmd_ptr + brain[(cmd_ptr + 1) % DETERMINATOR_BRAIN_SIZE]; //relative shift of cmd pointer			
			break;
		}

		cmd_ptr = new_cmd_ptr % DETERMINATOR_BRAIN_SIZE; //move cmd pointer
		energy -= ENERGY_INTERPRETATION_COST;
		updateStatus(world);

		if (terminal_cmd)
			break;
	}

	if (energy < ENERGY_HUNGRY_THRESHOLD)
	{
		hp -= HP_HUNGRY_DRAIN;
		updateStatus(world);
	}

	hp_delta = hp - initial_hp;
	energy_delta = energy - initial_energy;

	if (energy > DETERMINATOR_MAX_ENERGY) //Division if the energy is on maximum
	{
		randomDivision(world);
		if (energy > DETERMINATOR_MAX_ENERGY) //Division failed, but energy can't be highier than maximum
			energy = DETERMINATOR_MAX_ENERGY;
	}
}

/*
0 - nothing to do, null command

1 - cell division(available empty cell, starting from N side)
2 - cell division(available empty cell, starting from E side)
3 - cell division(available empty cell, starting from S side)
4 - cell division(available empty cell, starting from W side)

5 - consume sun energy(photosynthesis)

6 - move N direction
7 - move E direction
8 - move S direction
9 - move W direction
10 - attack N direction(eat if success)
11 - attack E direction(eat if success)
12 - attack S direction(eat if success)
13 - attack W direction(eat if success)
14 - defend from N direction
15 - defend from E direction
16 - defend from S direction
17 - defend from W direction
18 - heal to N direction(recover other bot's HP for energy)
	19 - heal to E direction
	20 - heal to S direction
	21 - heal to W direction
	22 - feed to N direction(give energy to bot)
	23 - feed to E direction
	24 - feed to S direction
	25 - feed to W direction

	26 - heal itself for energy
	27 - grow HP(increase maximum HP for energy)
	28 - grow leaves(increase photosynthesis power for energy)
	29 - grow claws(increase attack power for energy)
	30 - grow armor(increase defense power for energy)
	31 - repeat last successful command

	32 - (condition)depth is lower than
	33 - (condition)depth is higher than
	34 - (condition)depth is between 2 values
	35 - (condition)HP is low
	36 - (condition)HP is falling down
	37 - (condition)HP is raising up
	38 - (condition)Energy is low
	39 - (condition)Energy is falling down
	40 - (condition)Energy is raising up

	41 - (condition)look at N direction(options: empty, wall, organic(dead body), bot, bot relative(the same species))
	42 - (condition)look at E direction
	43 - (condition)look at S direction
	44 - (condition)look at W direction

	45 - (condition)look for empty cell(options: N, E, S, W, not found) - first found direction will be executed
	46 - (condition)look for wall
	47 - (condition)look for organic
	48 - (condition)look for bot
	49 - (condition)look for bot relative

	50 - (condition) goto absolute address
	51 - (condition) goto relative address
*/

