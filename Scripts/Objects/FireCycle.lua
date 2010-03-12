-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

FireCycle = Enemy:new()

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
    self.hitPoints = 2
end

