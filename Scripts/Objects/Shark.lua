-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Shark = Enemy:new()

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
    self.hitPoints = 1
end
