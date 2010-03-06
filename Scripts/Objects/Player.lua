-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Player = Object:new()

-- Test enemy for the game.  The init() function gets
-- called when a new sentinel is created
function Player:init()

    -- Add the base mesh to the object.  Each mesh gets
    -- added to its own scene node (this will be important
    -- later for explosions)
    self:addEntity{name="Hull", mesh="DaggerHull.mesh"}
    self:addEntity{name="LeftWing", mesh="DaggerLeftWing.mesh"}
    self:addEntity{name="RightWing", mesh="DaggerRightWing.mesh"}
    self:addEntity{name="Engine", mesh="DaggerEngine.mesh"}
end

-- This function gets called once per timestep by the
-- C++ peer class connected to this Lua class
function Player:onTimeStep()
end
