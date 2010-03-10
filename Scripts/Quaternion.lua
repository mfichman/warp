-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

-- Note: most of this code is translated from the ogre source ogrequaternion.cpp

Quaternion = Object:new()

-- rotates a vector
-- adapted from wikipedia pseudo-code http://en.wikipedia.org/wiki/Quaternion_rotation
function Quaternion:__mul(v)
    local t2  =  self[4] * self[1]
    local t3  =  self[4] * self[2]
    local t4  =  self[4] * self[3]
    local t5  = -self[1] * self[1]
    local t6  =  self[1] * self[2]
    local t7  =  self[1] * self[3]
    local t8  = -self[2] * self[2]
    local t9  =  self[2] * self[3]
    local t10 = -self[3] * self[3]
    local result = Vector:new()
    result[1] = 2*( (t8 + t10) * v[1] + (t6 -  t4) * v[2] + (t3 + t7) * v[3]) + v[1]
    result[2] = 2*( (t4 +  t6) * v[1] + (t5 + t10) * v[2] + (t9 - t2) * v[3]) + v[2]
    result[3] = 2*( (t7 -  t3) * v[1] + (t2 +  t9) * v[2] + (t5 + t8) * v[3]) + v[3]
    return result
end

function Quaternion:toAngleAxis() 
    local sqr_length = self[1] * self[1] + self[2] * self[2] + self[3] * self[3]
    local angle = 0
    local axis = Vector:new()
    if (sqr_length > 0) then
        angle = 2 * math.acos(self[4])
        local inv_length = 1 / math.sqrt(sqr_length)
        axis[1] = self[1] * sqr_length
        axis[2] = self[2] * sqr_length
        axis[3] = self[3] * sqr_length
    else
        axis:set(1,0,0)
    end
    return angle, axis
end

function Quaternion:getDirection()
    -- a little hacky. assume that meshes point down -z axis by default
    -- default behavior should probably be the positive z axis
    return self * {0,0,-1}
end

function Quaternion:fromAngleAxis(vector, angle)
    
end
