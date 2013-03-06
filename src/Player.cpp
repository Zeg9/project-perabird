/*
    Copyright (C) 2012 Zeg9
    
    This file is part of Project Perabird.
    Project Perabird is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Project Perabird is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Project Perabird.  If not, see <http://www.gnu.org/licenses/>
*/

#include <string>
#include "Tools.h"
#include "Player.h"

Player::Player() : name("") {}
Player::~Player() {}
std::string Player::getName() { return name; }
void Player::setName(std::string _name) { name = _name; }

vec3 Player::getPosition() { return pos; }
void Player::setPosition(int x, int y, int z) { pos.x = x; pos.y = y; pos.z = z; }
void Player::setPosition(vec3 _pos) { pos = _pos; }

