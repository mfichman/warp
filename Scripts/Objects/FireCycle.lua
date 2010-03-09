-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

FireCycle = Object:new()

-- Test enemy for the game.  The init() function gets
-- called when a new sentinel is created
function FireCycle:init()

    -- Add the base mesh to the object.  Each mesh gets
    -- added to its own scene node (this will be important
    -- later for explosions)
    self:addEntity{name="Body", mesh="FireCycleBody.mesh"}
    self:addEntity{name="LAxle", mesh="FireCycleLAxle.mesh"}
    self:addEntity{name="RAxle", mesh="FireCycleRAxle.mesh"}
    self:addEntity{name="LWheel", mesh="FireCycleLWheel.mesh"}
    self:addEntity{name="RWheel", mesh="FireCycleRWheel.mesh"}
    self.destroyed = false
    self.hitPoints = 3
end

-- This function gets called once per timestep by the
-- C++ peer class connected to this Lua class
function FireCycle:onTimeStep()
end

-- This function is called when the object is selected by
-- the player
function FireCycle:onSelect()
    self:target()
end

-- This function is called when a projectile hits the object
function FireCycle:onProjectileHit()

end

function FireCycle:onDestroy()
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
function FireCycle:onPlayerHit()
    if (self.destroyed) then return end
    self.destroyed = true
    local explosion = Level:createObject("Explosion")  
    explosion:set{position=self:getPosition()}
    self:explode()
end
