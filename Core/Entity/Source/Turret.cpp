#include <fstream>
#include "Turret.h"
#include "LoadEnums.h"
#include "ObjectIds.h"
#include "Display.h"
#include "TileChangeManager.h"
#include "SoundManager.h"
#include "Common.h"
#include "TileEnums.h"
#include "Player.h"

namespace game
{
	extern Display game;
	extern TileChangeManager TileHandler;
	extern SoundManager m_sounds;
}


Turret::Turret()
{
	isDestroyed_ = false;
	graphic = '+';
	owner = "None";
	health.setMaxHealth(1000);
	repair_sound_.SetSound("Repair");
}

Turret::Turret(Position position, std::string owner)
{
	this->owner = owner;
	ai.setPosition(position);
	isDestroyed_ = false;
	graphic = '+';
	owner = "None";
	health.setMaxHealth(1000);
}


Turret::~Turret()
{
}

HealthComponent& Turret::getHealthRef()
{
	return health;
}

NoteUI Turret::getNote()
{
	NoteUI note;
	std::stringstream line;
	line << "Owner: " << owner;
	note.AddLine(line.str());
	line.str(string());
	line << "Health: " << health.getHealth() << "/" << health.getMaxHealth();
	note.AddLine(line.str());
	return note;
}

void Turret::setGraphic(char g)
{
	graphic = g;
	Tile& tile = game::game.getTileRefAt(ai.getPosition());
	tile.updateOverlay(true, g);
}

void Turret::setOwner(std::string owner)
{
	this->owner = owner;
	ai.setOwner(owner);
}

void Turret::setPosition(Position pos)
{
	ai.setPosition(pos);
}

void Turret::setRange(int range)
{
	ai.setVisionRange(range);
}

void Turret::setShootCoolDown(int seconds)
{
	ai.setShootCoolDownTime(seconds);
}

char Turret::getGraphic()
{
	return graphic;
}

std::string Turret::getOwner()
{
	return owner;
}

void Turret::serialize(std::stringstream& file)
{
	file << LOAD::L_Turret << std::endl
		<< owner << std::endl // String
		<< (int)graphic << std::endl // Int
		<< isDestroyed_ << std::endl // Bool
		<< getID() << std::endl; // Int
	ai.serialize(file);
	health.serialize(file);
}

void Turret::deserialize(std::stringstream& file)
{
	int graphic_;
	int id;
	file >> owner
		>> graphic_
		>> isDestroyed_
		>> id;
	setID(id);
	ai.deserialize(file);
	health.deserialize(file);
	graphic = graphic_;
}

void Turret::serialize(fstream & file)
{
	file << LOAD::L_Turret << std::endl
		<< owner << std::endl
		<< (int)graphic << std::endl
		<< isDestroyed_ << std::endl
		<< getID() << std::endl;
	ai.serialize(file);
	health.serialize(file);
}

void Turret::deserialize(fstream & file)
{
	int graphic_;
	int id;
	file >> owner
		>> graphic_
		>> isDestroyed_
		>> id;
	setID(id);
	ai.deserialize(file);
	health.deserialize(file);
	graphic = graphic_;
}

void Turret::update()
{
	repair_sound_.Update();
	if (isDestroyed_ == true) return;
	ai.update();
}

bool Turret::hasComponent(int component)
{
	switch (component)
	{
	case TURRET_ID: return true;
	default: return false;
	}
}

bool Turret::isKilled()
{
	return isDestroyed_;
}

void Turret::kill()
{
	isDestroyed_ = true;
	clean();
}

void Turret::clean()
{
	Tile &tile = game::game.getTileRefAt(ai.getPosition());
	tile.removeOverlay();
	game::TileHandler.push_back(ai.getPosition());
}

bool Turret::damage(int amount, string name, bool server)
{
	if (owner == name) return false;
	if (server == false)
	{
		std::stringstream msg;
		if (isSetToUpdate() == true)
		{
			health.damage(amount);
			msg << SendDefault << EndLine << EntityDamage << EndLine << ai.getPosition().serializeR() << amount << EndLine << name << EndLine;
			SendServerLiteral(msg.str());
			if (health.isDead())
			{
				game::m_sounds.PlaySoundR("Destroy");
				kill();
			}
		}
		else
		{
			health.damage(amount);
			msg << SendDefault << EndLine << EntityDamage << EndLine << ai.getPosition().serializeR() << amount << EndLine << name << EndLine;
			SendServerLiteral(msg.str());
			if (health.isDead())
			{
				game::m_sounds.PlaySoundR("Destroy");
			}
		}
	}
	else
	{
		health.damage(amount);
		if (isSetToUpdate() == true)
		{
			if (health.isDead())
			{
				kill();
				game::m_sounds.PlaySoundR("Destroy");
			}
		}
	}
	return true;
}

void Turret::setPos(Position pos)
{
	ai.setPosition(pos);
}

void Turret::updateOverlay()
{
	game::game.getTileRefAt(ai.getPosition()).updateOverlay(true, graphic);
}

void Turret::updateID()
{
	return;
}

void Turret::send()
{
	std::stringstream msg;
	msg << SendDefault << EndLine << EntityAdd << EndLine << ETurret << EndLine;
	serialize(msg);
	SendServerLiteral(msg.str());
}

void Turret::render()
{
	game::game.getTileRefAt(ai.getPosition()).updateOverlay(true, graphic);
	game::game.getTileRefAt(ai.getPosition()).setOverlayColor(C_Black);
}

void Turret::activate(Player* player)
{
	if (player->getName() == owner)
	{
		health.heal(2);
		if (health.isFull() == false)
		{
			repair_sound_.SetTimer(0.2);
		}
	}
}

Position Turret::getPos()
{
	return ai.getPosition();
}
