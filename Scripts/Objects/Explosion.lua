-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Explosion = Object:new()

-- Test enemy for the game.  The init() function gets
-- called when a new sentinel is created
function Explosion:init()
    self:addParticleSystem{name="Explosion", template="Explosion"}
    --self:addParticleSystem{name="Smoke", template="Smoke"}
end

-- This function gets called once per timestep by the
-- C++ peer class connected to this Lua class
function Explosion:onTimeStep()

end
