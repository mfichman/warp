-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Sentinel = Enemy:new()

-- Test enemy for the game.  The init() function gets
-- called when a new sentinel is created
function Sentinel:init()

    -- Add the base mesh to the sentinel.  Each mesh gets
    -- added to its own scene node (this will be important
    -- later for explosions)
   -- self:addEntity{name="Body", mesh="Sentinel.mesh"}
end

-- This function gets called once per timestep by the
-- C++ peer class connected to this Lua class
function Sentinel:onTimeStep()
end
