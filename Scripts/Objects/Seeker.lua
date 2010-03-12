-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Seeker = Enemy:new()

-- Test enemy for the game.  The init() function gets
-- called when a new sentinel is created
function Seeker:init()

    -- Add the base mesh to the object.  Each mesh gets
    -- added to its own scene node (this will be important
    -- later for explosions)
    self:addEntity{name="BodyLeft", mesh="SeekerBodyLeft.mesh"}
    self:addEntity{name="BodyRight", mesh="SeekerBodyRight.mesh"}
    self:addEntity{name="Ring", mesh="SeekerRing.mesh"}
    self.hitPoints = 1
    self.cooldown = 7.0
end

-- This function gets called once per timestep by the
-- C++ peer class connected to this Lua class
function Seeker:onTimeStep()
    self.cooldown = self.cooldown - Level:getTimeStep()
    if (self.cooldown <= 0 and not self.destroyed) then
        self:createMissile{type="Orb"}:launch(self)
        self.cooldown = 2.0
    end
end



