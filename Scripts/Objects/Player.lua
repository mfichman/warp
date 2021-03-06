-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Player = Object:new()

-- Test enemy for the game.  The init() function gets
-- called when a new sentinel is created
function Player:init()

    -- Add the base mesh to the object.  Each mesh gets
    -- added to its own scene node (this will be important
    -- later for explosions)
    --self:addEntity{name="Body", mesh="LightBikeBody.mesh"}
    --self:addEntity{name="LeftWing", mesh="LightBikeLeftWing.mesh"}
    --self:addEntity{name="RightWing", mesh="LightBikeRightWing.mesh"}
    --self:addEntity{name="TopEngine", mesh="LightBikeTopEngine.mesh"}
    --self:addEntity{name="BottomEngine", mesh="LightBikeBottomEngine.mesh"}
    self:addEntity{name="Hull", mesh="SharkHull.mesh"}
    self:addEntity{name="MainEngine", mesh="SharkMainEngine.mesh"}
    self:addEntity{name="LeftEngine", mesh="SharkLeftEngine.mesh"}
    self:addEntity{name="RightEngine", mesh="SharkRightEngine.mesh"}
    self:addParticleSystem{name="Jet", template="Jet", position={0.0, 0.15, 0.481}}
    self:addParticleSystem{name="JetLeft", template="SmallJet", position={-0.3, 0.08, 0.461}}
    self:addParticleSystem{name="JetRight", template="SmallJet", position={0.3, 0.08, 0.461}}
    self:addParticleSystem{name="Explosion", template="SmallExplosion", position={0.0, 0.1, -0.4}, on=0}
--    self:addEntity{name="Hull", mesh="DaggerHull.mesh"}
--    self:addEntity{name="LeftWing", mesh="DaggerLeftWing.mesh"}
--    self:addEntity{name="RightWing", mesh="DaggerRightWing.mesh"}
--    self:addEntity{name="Engine", mesh="DaggerEngine.mesh"}
end

-- This function gets called once per timestep by the
-- C++ peer class connected to this Lua class
function Player:onTimeStep()
end

function Player:onEnemyHit()
    --self:explode();
    Level:playSFX{id=6, gain=2}
end

function Player:onThrottleUp()
   -- Level:setCompositor("Radial Blur")
    self:setParticleSystem{name="Jet", width=0.4, height=0.4, velocity=-9.0}
    self:setParticleSystem{name="JetLeft", width=0.3, height=0.3, velocity=-5.0}
    self:setParticleSystem{name="JetRight", width=0.3, height=0.3, velocity=-5.0}
    print("Throttling up")
end

function Player:onThrottleDown()
    --Level:setCompositor("")
    self:setParticleSystem{name="Jet", width=0.2, height=0.2, velocity=-3.0}
    self:setParticleSystem{name="JetLeft", width=0.1, height=0.13, velocity=-1.3}
    self:setParticleSystem{name="JetRight", width=0.1, height=0.13, velocity=-1.3}
end

function Player:onThrottleNormal()
    --Level:setCompositor("")
    self:setParticleSystem{name="Jet", width=0.3, height=0.3, velocity=-5.0}
    self:setParticleSystem{name="JetLeft", width=0.2, height=0.2, velocity=-3.0}
    self:setParticleSystem{name="JetRight", width=0.2, height=0.2, velocity=-3.0}
end

function Player:onDestroy()
    local explosion = Level:createObject("Explosion")  
    explosion:setPosition(self:getPosition())
    self:explode()
    print("Explode")
end

function Player:onProjectileHit()
    self:setParticleSystem{name="Explosion", on=1}
    Level:playSFX{id=6, gain=2}
    --local explosion = Level:createObject("SmallExplosion")  
    --explosion:setPosition(self:getPosition())
    --explosion:setVelocity(self:getVelocity())
end


