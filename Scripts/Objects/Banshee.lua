-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Banshee = Enemy:new()

-- Test enemy for the game.  The init() function gets
-- called when a new sentinel is created
function Banshee:init()

    -- Add the base mesh to the object.  Each mesh gets
    -- added to its own scene node (this will be important
    -- later for explosions)
    self:addEntity{name="Hull", mesh="BansheeBody.mesh"}
    self:addEntity{name="Wing", mesh="BansheeWings.mesh"}
    self:addEntity{name="Pylon", mesh="BansheePylon.mesh"}
    self.hitPoints = 4
    self.cooldown = 3
end

-- This function gets called once per timestep by the
-- C++ peer class connected to this Lua class
function Banshee:onTimeStep()
    self.cooldown = self.cooldown - Level:getTimeStep()
    if (self.cooldown <= 0 and not self.destroyed) then
        self:createMissile{type="Orb"}:launch(self)
        self.cooldown = 1.5
    end
end

