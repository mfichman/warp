-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

TrackingPhoton = Object:new()

-- Test enemy for the game.  The init() function gets
-- called when a new sentinel is created
function TrackingPhoton:init()
    --Level:playSFX{id=0, gain=2} -- queue chuck sound effect
end

function TrackingPhoton:realInit(parent)
    local position = parent:getPosition() + parent:getOrientation() * {0,0,-1} * 1.5
    local vel = Level:getPlayerPosition() - self:getPosition();
    vel:normalize()
    vel = vel * 30
    vel = vel + { math.random(-6, 6), math.random(-6, 6), math.random(-6, 6) }
    self:setPosition(position)
    self:setVelocity(vel)
end

-- This function gets called once per timestep by the
-- C++ peer class connected to this Lua class
function TrackingPhoton:onTimeStep()
   -- local alpha = 0.4
    --local vel = Level:getPlayerPosition() - self:getPosition() + Level:getPlayerVelocity() * 0.1;
    --vel:normalize();
    --vel = vel * 40
   -- self:setVelocity(vel)
   -- self:setVelocity(self:getVelocity()*alpha + vel*(1-alpha))
end
