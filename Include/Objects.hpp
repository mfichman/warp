/******************************************************************************
 * Warp: CS248 Final Project                                                  *
 * Francesco Georg, Matt Fichman                                              *
 ******************************************************************************/
#pragma once

#include <Game.hpp>
#include <memory>
#include <string>

namespace Warp {

class Game;

class Objects {
public:
	struct Impl;

    enum Type {
        TYPE_PLANE = 0x00000001,
        TYPE_BALL = 0x00000002,
        TYPE_RAY = 0x00000003,
        TYPE_TERRAIN = 0x00000004,
        TYPE_ANY = 0xFFFFFFFF
    };

	/** Creates a new object factory */
    Objects(Game* game);

    /** Destructor */
    ~Objects();

    /** Creates a new ball using parameters in the given table */
    void createBall(const std::string& name, int table);

    /** Creates a new cube using parameters in the given table */
    void createPlane(const std::string& name, int table);

    /** Creates a new ship using parameters in the given table */
    void createShip(const std::string& name, int table);

private:
	std::auto_ptr<Impl> impl_;
};

}