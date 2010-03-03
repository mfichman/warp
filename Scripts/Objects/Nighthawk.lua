-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Nighthawk = Object:new()

-- Test enemy for the game.  The init() function gets
-- called when a new sentinel is created
function Nighthawk:init()
    
    -- Add the base mesh to the object.  Each mesh gets
    -- added to its own scene node (this will be important
    -- later for explosions)
    self:addEntity{name="Hull", mesh="NighthawkHull.mesh"}
    self:addEntity{name="RightWing", mesh="NighthawkRightWing.mesh"}
    self:addEntity{name="LeftWing", mesh="NighthawkLeftWing.mesh"}
    self.selected = false
end

-- This function gets called once per timestep by the
-- C++ peer class connected to this Lua class
function Nighthawk:onTimeStep()
    --[[self.time = self.time + 0.01
    self:set{orientation={0,0.4,1,angle=100*self.time}}]]
end

-- This function is called when the object is selected by
-- the player
function Nighthawk:onSelect()
    if (self.selected) then return end -- Make sure we don't launch > 1 task
    self.selected = true
    Level:createTask(function()
        self:target()
        Level:waitForBeat()
        local explosion = Level:createObject("Explosion")  
        explosion:set{position=self:getPosition()}
        self:explode()
        
    end)
end
