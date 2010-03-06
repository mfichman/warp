-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Photon = Object:new()

-- Test enemy for the game.  The init() function gets
-- called when a new sentinel is created
function Photon:init()
    Level:playSFX{id=1, gain=2} -- queue chuck sound effect

    --self:addParticleSystem{name="Photon", template="Photon"}
end

-- This function gets called once per timestep by the
-- C++ peer class connected to this Lua class
function Photon:onTimeStep()
end
