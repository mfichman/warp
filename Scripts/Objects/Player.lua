-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Player = Object:new()

-- Test enemy for the game.  The init() function gets
-- called when a new sentinel is created
function Player:init()

    -- Add the base mesh to the object.  Each mesh gets
    -- added to its own scene node (this will be important
    -- later for explosions)
    --self:addEntity{name="Body", mesh="LightBikeBody.mesh"}
    --self:addEntity{name="LeftWing", mesh="LightBikeLeftWing.mesh"}
    --self:addEntity{name="RightWing", mesh="LightBikeRightWing.mesh"}
    --self:addEntity{name="TopEngine", mesh="LightBikeTopEngine.mesh"}
    --self:addEntity{name="BottomEngine", mesh="LightBikeBottomEngine.mesh"}
    self:addEntity{name="Hull", mesh="SharkHull.mesh"}
    self:addEntity{name="MainEngine", mesh="SharkMainEngine.mesh"}
    self:addEntity{name="LeftEngine", mesh="SharkLeftEngine.mesh"}
    self:addEntity{name="RightEngine", mesh="SharkRightEngine.mesh"}
--    self:addEntity{name="Hull", mesh="DaggerHull.mesh"}
--    self:addEntity{name="LeftWing", mesh="DaggerLeftWing.mesh"}
--    self:addEntity{name="RightWing", mesh="DaggerRightWing.mesh"}
--    self:addEntity{name="Engine", mesh="DaggerEngine.mesh"}
end

-- This function gets called once per timestep by the
-- C++ peer class connected to this Lua class
function Player:onTimeStep()
end

function Player:onEnemyHit()
    --self:explode();
end
