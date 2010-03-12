-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Enemy = Object:new{destroyed=false}

function Enemy:onDestroy()
    if (self.destroyed) then return end
    self.destroyed = true
    Level:createTask(function()
        if (self.on_death_sound) then
            --Level:playSFX{id=self.on_death_sound, gain=2} -- queue chuck sound effect
            --Level:sleep(.25)
            if (self.gain) then
                Level:playSFX{id=self.on_death_sound, gain=self.gain} -- queue chuck sound effect
            else
                Level:playSFX{id=self.on_death_sound, gain=2} -- queue chuck sound effect
            end
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
