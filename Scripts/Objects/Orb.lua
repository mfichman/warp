-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Orb = Object:new()

-- Test enemy for the game.  The init() function gets
-- called when a new sentinel is created
function Orb:init()
    --Level:playSFX{id=0, gain=2} -- queue chuck sound effect

end

function Orb:launch(parent)
    self:setMaterial("Orb")
    local position = parent:getPosition() + parent:getOrientation() * {0,0,-1} * 1.5
    local velocity = Level:getPlayerPosition() - self:getPosition();
    velocity:normalize()
    velocity = velocity * 45
    velocity = velocity + Level:getPlayerVelocity()
    
    self:setPosition(position)
    self:setVelocity(velocity)
    self:setTarget(Level:getPlayer())
end

function Orb:onTargetHit()
    self:destroy()
end
