-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Shark = Object:new()

-- Test enemy for the game.  The init() function gets
-- called when a new sentinel is created
function Shark:init()

    -- Add the base mesh to the object.  Each mesh gets
    -- added to its own scene node (this will be important
    -- later for explosions)
    self:addEntity{name="Hull", mesh="SharkHull.mesh"}
    self:addEntity{name="MainEngine", mesh="SharkMainEngine.mesh"}
    self:addEntity{name="LeftEngine", mesh="SharkLeftEngine.mesh"}
    self:addEntity{name="RightEngine", mesh="SharkRightEngine.mesh"}
    self.destroyed = false
    self.hitPoints = 1
end

-- This function gets called once per timestep by the
-- C++ peer class connected to this Lua class
function Shark:onTimeStep()
end

-- This function is called when the object is selected by
-- the player
function Shark:onSelect()
    self:target()
end

-- This function is called when a projectile hits the object
function Shark:onProjectileHit()

end

function Shark:onDestroy()
    if (self.destroyed) then return end
    self.destroyed = true
    Level:createTask(function()
        Level:playSFX{id=1, gain=2} -- queue chuck sound effect
        Level:sleep(.5); -- grow animation happens here
        Level:playSFX{id=2, gain=2} -- queue chuck sound effect
        local explosion = Level:createObject("Explosion")  
        explosion:set{position=self:getPosition()}
        self:explode()
    end)
end

-- Called when an object hits a player
function Shark:onPlayerHit()
    if (self.destroyed) then return end
    self.destroyed = true
    local explosion = Level:createObject("Explosion")  
    explosion:set{position=self:getPosition()}
    self:explode()
end
