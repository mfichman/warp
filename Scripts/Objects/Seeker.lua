-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Seeker = Object:new()

-- Test enemy for the game.  The init() function gets
-- called when a new sentinel is created
function Seeker:init()

    -- Add the base mesh to the object.  Each mesh gets
    -- added to its own scene node (this will be important
    -- later for explosions)
    self:addEntity{name="BodyLeft", mesh="SeekerBodyLeft.mesh"}
    self:addEntity{name="BodyRight", mesh="SeekerBodyRight.mesh"}
    self:addEntity{name="Ring", mesh="SeekerRing.mesh"}
    self.destroyed = false
    self.hitPoints = 1
    --self:setOrientation(orientation)
end

-- This function gets called once per timestep by the
-- C++ peer class connected to this Lua class
function Seeker:onTimeStep()
--    position = getPlayerPosition()
--    print(position[1], position[2], position[3])
    local direction = Level:getPlayerOrientation():getDirection()
    local position = Level:getPlayerPosition()
    self:setPosition(position + (direction * 3))

end

-- This function is called when the object is selected by
-- the player
function Seeker:onSelect()
    self:target()
end

-- This function is called when a projectile hits the object
function Seeker:onProjectileHit()

end

function Seeker:onDestroy()
    if (self.destroyed) then return end
    self.destroyed = true
    Level:createTask(function()
        --Level:playSFX{id=1, gain=2} -- queue chuck sound effect
        --Level:sleep(.5); -- grow animation happens here
        Level:playSFX{id=2, gain=2} -- queue chuck sound effect
        local explosion = Level:createObject("Explosion")  
        explosion:set{position=self:getPosition()}
        self:explode()
    end)
end

-- Called when an object hits a player
function Seeker:onPlayerHit()
    if (self.destroyed) then return end
    self.destroyed = true
    local explosion = Level:createObject("Explosion")  
    explosion:set{position=self:getPosition()}
    self:explode()
end

