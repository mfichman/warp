-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

StarBlazer = Enemy:new()

-- Test enemy for the game.  The init() function gets
-- called when a new sentinel is created
function StarBlazer:init()

    -- Add the base mesh to the object.  Each mesh gets
    -- added to its own scene node (this will be important
    -- later for explosions)
    self:addEntity{name="Body", mesh="StarBlazerBody.mesh"}
    self:addEntity{name="LeftWing", mesh="StarBlazerLWing.mesh"}
    self:addEntity{name="RightWing", mesh="StarBlazerRWing.mesh"}
    self:addEntity{name="TopWing", mesh="StarBlazerTopWing.mesh"}
    self:addEntity{name="Ring", mesh="StarBlazerRing.mesh"}
    self.hitPoints = 4
    self.cooldown = 6.0
end

-- This function gets called once per timestep by the
-- C++ peer class connected to this Lua class
function StarBlazer:onTimeStep()
    self.cooldown = self.cooldown - Level:getTimeStep()
    if (self.cooldown <= 0 and not self.destroyed) then
        self:createMissile{type="Orb"}:launch(self)
        self.cooldown = 2.0
    end
end
