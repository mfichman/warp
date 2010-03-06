-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Banshee = Object:new()

-- Test enemy for the game.  The init() function gets
-- called when a new sentinel is created
function Banshee:init()

    -- Add the base mesh to the object.  Each mesh gets
    -- added to its own scene node (this will be important
    -- later for explosions)
    self:addEntity{name="Hull", mesh="BansheeBody.mesh"}
    self:addEntity{name="Wing", mesh="BansheeWings.mesh"}
    self:addEntity{name="Pylon", mesh="BansheePylon.mesh"}
    self.destroyed = false
end

-- This function gets called once per timestep by the
-- C++ peer class connected to this Lua class
function Banshee:onTimeStep()
end

-- This function is called when the object is selected by
-- the player
function Banshee:onSelect()
    self:target()
end

-- This function is called when a projectile hits the object
function Banshee:onProjectileHit()
    if (self.destroyed) then return end
    self.destroyed = true
    Level:createTask(function()
        Level:playSFX{id=0, gain=2} -- queue chuck sound effect
        local explosion = Level:createObject("Explosion")  
        explosion:set{position=self:getPosition()}
        self:explode()
    end)
end
