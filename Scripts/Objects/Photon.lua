-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Photon = Object:new()

-- Test enemy for the game.  The init() function gets
-- called when a new sentinel is created
function Photon:init()
    Level:playSFX{id=0, gain=2} -- queue chuck sound effect

    --self:addParticleSystem{name="Photon", template="Photon"}
end

-- This function gets called once per timestep by the
-- C++ peer class connected to this Lua class
function Photon:onTimeStep()
    local target = self:getTarget()
    if (target ~= nil) then
        local velocity = target:getPosition() - self:getPosition()
        velocity:normalize()
        velocity = velocity * 80
        --velocity = velocity + target:getVelocity()
        self:setVelocity(velocity)
    
    end
end

function Photon:onTargetDestroyed()
    self:destroy()
end
