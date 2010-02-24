-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Ship = Object:new()

-- Test enemy for the game.  The init() function gets
-- called when a new sentinel is created
function Ship:init()

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
    self.time = 0.0
]]
    
    -- Add the base mesh to the object.  Each mesh gets
    -- added to its own scene node (this will be important
    -- later for explosions)
    self:addEntity{name="ShipHull", mesh="ShipHull.mesh"}
    self:addEntity{name="ShipRightWing", mesh="ShipRightWing.mesh"}
    self:addEntity{name="ShipLeftWing", mesh="ShipLeftWing.mesh"}
    self:setEntity{name="ShipRightWing", position={0.6, -0.2, 0}}
    self:setEntity{name="ShipLeftWing", position={-0.6, -0.2, 0}}
    self.selected = false
end

-- This function gets called once per timestep by the
-- C++ peer class connected to this Lua class
function Ship:onTimeStep()
    --[[self.time = self.time + 0.01
    self:set{orientation={0,0.4,1,angle=100*self.time}}]]
end

-- This function is called when the object is selected by
-- the player
function Ship:onSelect()
    if (self.selected) then return end -- Make sure we don't launch > 1 task
    self.selected = true
    Level:createTask(function()
        print("Sleeping...")
        --Level:waitForBeatSet()   
        --Level:sleep(100)
        print("Task awake")  
        local explosion = Level:createObject("Explosion")  
        explosion:set{position=self:getPosition()}
        self:explode()
    end)
end
