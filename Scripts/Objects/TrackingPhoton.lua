-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

TrackingPhoton = Object:new()

-- Test enemy for the game.  The init() function gets
-- called when a new sentinel is created
function TrackingPhoton:init()
    --Level:playSFX{id=0, gain=2} -- queue chuck sound effect
end

function TrackingPhoton:launch(parent)
    local position = parent:getPosition() + parent:getOrientation() * {0,0,-1} * 1.5
    local velocity = Level:getPlayerPosition() - self:getPosition();
    velocity:normalize()
    velocity = velocity * 20
    velocity = velocity + Level:getPlayerVelocity()
    
    self:setPosition(position)
    self:setVelocity(velocity)
    self:setTarget(Level:getPlayer())
end

function TrackingPhoton:onTargetHit()
    self:destroy()
end
