-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

AI = Object:new()

function AI:trackFromSide(enemy)
    local proj = Level:getSpineProjection(100)
    local right = proj.forward:cross{0,1,0}
    enemy:setPosition(proj.position + right * 50)
    enemy:setVelocity{30,0,0}
    enemy:setOrientation(proj.forward)

    enemy.onTimeStep = function(self)
        local target_direction = Level:getPlayerPosition() - enemy:getPosition()
        target_direction:normalize()
        local cur_orientation = enemy:getOrientation()
        local normal = target_direction:cross(cur_orientation:getDirection())
        -- cross product is ab*sin(theta)n = sin(theta)n
        if (normal:squaredLength() > .1) then
            rotation = Quaternion:new()
            rotation:fromAngleAxis(normal, .1) -- adjust the angle between it and the player by .1 radians
            cur_orientation = rotation * cur_orientation
            enemy:setOrientation(cur_orientation)
            enemy:setVelocity(cur_orientation * 25)
        end
        
    end
end


function AI:flyFromBehind(enemy)
    local proj = Level:getSpineProjection(-50)
    local right = proj.forward:cross{0, 1, 0}
    enemy:setPosition(proj.position + right * 20)
    enemy:setVelocity(proj.forward * 30)
end