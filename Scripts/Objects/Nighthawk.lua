-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Nighthawk = Enemy:new()

-- Test enemy for the game.  The init() function gets
-- called when a new sentinel is created
function Nighthawk:init()
    
    -- Add the base mesh to the object.  Each mesh gets
    -- added to its own scene node (this will be important
    -- later for explosions)
    self:addEntity{name="Hull", mesh="NighthawkHull.mesh"}
    self:addEntity{name="RightWing", mesh="NighthawkRightWing.mesh"}
    self:addEntity{name="LeftWing", mesh="NighthawkLeftWing.mesh"}
end
