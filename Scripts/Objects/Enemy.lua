-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Enemy = Object:new{destroyed=false}

function Enemy:onDestroy()
    if (self.destroyed) then return end
    self.destroyed = true
    Level:createTask(function()
        if (on_death_sound) then
            Level:playSFX{id=on_death_sound, gain=2} -- queue chuck sound effect
        end
        local explosion = Level:createObject("Explosion")  
        explosion:setPosition(self:getPosition())
        self:explode()
    end)
end

-- Called when an object hits a player
function Enemy:onPlayerHit()
    if (self.destroyed) then return end
    self.destroyed = true
    local explosion = Level:createObject("Explosion")  
    explosion:setPosition(self:getPosition())
    self:explode()
end

-- This function is called when the object is selected by
-- the player
function Enemy:onSelect()
    self:target()
end

-- Default timestep function
function Enemy:onTimeStep()
end 
