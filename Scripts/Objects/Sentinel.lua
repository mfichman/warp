-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Sentinel = Object:new()

-- Test enemy for the game.  The init() function gets
-- called when a new sentinel is created
function Sentinel:init()
    -- Add the base mesh to the sentinel.  Each mesh gets
    -- added to its own scene node (this will be important
    -- later for explosions)
    --[[self:addEntity{name="Body", mesh="SentinelBody.mesh"}
    self:addEntity{name="Tentacle0", mesh="SentinelTentacle.mesh"}
    self:setEntity{name="Tentacle0", animation="Attack", orientation={0,0,1,angle=0}, position={0.25,0,0}}
    self:addEntity{name="Tentacle1", mesh="SentinelTentacle.mesh"}
    self:setEntity{name="Tentacle1", animation="Attack", orientation={0,0,1,angle=90}, position={0,0.25,0,0}}
    self:addEntity{name="Tentacle2", mesh="SentinelTentacle.mesh"}
    self:setEntity{name="Tentacle2", animation="Attack", orientation={0,0,1,angle=180}, position={-0.25,0,0}}
    self:addEntity{name="Tentacle3", mesh="SentinelTentacle.mesh"}
    self:setEntity{name="Tentacle3", animation="Attack", orientation={0,0,1,angle=270}, position={0,-0.25,0}}

    self:set{orientation={0,1,0,angle=180}}-- Flip around to face camera
    
]]
    self.time = 0.0
    --self:addEntity{name="Ship", mesh="Ship.mesh"}
    self:addEntity{name="ShipHull", mesh="ShipHull.mesh"}
    self:addEntity{name="ShipRightWing", mesh="ShipRightWing.mesh"}
    self:addEntity{name="ShipLeftWing", mesh="ShipLeftWing.mesh"}
    self:setEntity{name="ShipRightWing", position={0.6, -0.2, 0}}
    self:setEntity{name="ShipLeftWing", position={-0.6, -0.2, 0}}
    
end

-- This function gets called once per timestep by the
-- C++ peer class connected to this Lua class
function Sentinel:onTimeStep()
    --self.time = self.time + 0.01
    --self:set{orientation={0,0.4,1,angle=100*self.time}}
end

-- This function gets called when the object is destroyed
-- (but not wwhen it is deallocated)
function Sentinel:onDestroy()

end
