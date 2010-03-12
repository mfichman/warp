-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Shield = Enemy:new()

-- Test enemy for the game.  The init() function gets
-- called when a new sentinel is created
function Shield:init()

    -- Add the base mesh to the object.  Each mesh gets
    -- added to its own scene node (this will be important
    -- later for explosions)
    --self:addEntity{name="Left", mesh="ShieldLeft.mesh"}
    --self:addEntity{name="Right", mesh="ShieldRight.mesh"}
    --self:addEntity{name="Top", mesh="ShieldTop.mesh"}
    --self:addEntity{name="Bottom", mesh="ShieldBottom.mesh"}
    self.hitPoints = 5
    
    local proj = Level:getSpineProjection(200)
    --print(proj.position[0], proj.position[1], proj.position[2])
    self:setPosition(proj.position)
    self:setOrientation(proj.rotation)
end


function Shield:top()
    self:addEntity{name="Top", mesh="ShieldTop.mesh"}
    self:addEntity{name="Right", mesh="ShieldRight.mesh"}
    return self
end

function Shield:bottom()
    self:addEntity{name="Left", mesh="ShieldLeft.mesh"}
    self:addEntity{name="Bottom", mesh="ShieldBottom.mesh"}
    return self
end

function Shield:left()
    self:addEntity{name="Right", mesh="ShieldRight.mesh"}
    self:addEntity{name="Bottom", mesh="ShieldBottom.mesh"}
    return self
end

function Shield:right()
    self:addEntity{name="Left", mesh="ShieldLeft.mesh"}
    self:addEntity{name="Top", mesh="ShieldTop.mesh"}
    return self
end
