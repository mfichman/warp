-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

SmallExplosion = Object:new()

-- Test enemy for the game.  The init() function gets
-- called when a new sentinel is created
function SmallExplosion:init()
    self:addParticleSystem{name="SmallExplosion", template="SmallExplosion"}
    --self:addParticleSystem{name="Smoke", template="Smoke"}
end
