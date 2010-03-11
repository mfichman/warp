-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

TrackingPhoton = Object:new()

-- Test enemy for the game.  The init() function gets
-- called when a new sentinel is created
function TrackingPhoton:init()
    --Level:playSFX{id=0, gain=2} -- queue chuck sound effect
    local vel = Level:getPlayerPosition() - self:getPosition()
    vel:normalize();
    vel = vel * 80
    self:setPosition(self:getPosition() + vel)
    self:setVelocity(vel)

    --self:addParticleSystem{name="Photon", template="Photon"}
end

-- This function gets called once per timestep by the
-- C++ peer class connected to this Lua class
function TrackingPhoton:onTimeStep()
    --local alpha = 0.0
    --local vel = Level:getPlayerPosition() - self:getPosition()
    --vel:normalize();
    --vel = vel * 80
    --self:setVelocity(vel)
    --self:setVelocity(self:getVelocity()*alpha + vel*(1-alpha))
end
