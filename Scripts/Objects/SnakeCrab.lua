-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

SnakeCrab = Enemy:new()

-- Test enemy for the game.  The init() function gets
-- called when a new sentinel is created
function SnakeCrab:init()

    -- Add the base mesh to the object.  Each mesh gets
    -- added to its own scene node (this will be important
    -- later for explosions)
    self:addEntity{name="Body", mesh="SnakeCrab.mesh", animation="Moving"}
    self.hitPoints = 1
end
