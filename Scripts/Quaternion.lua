-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

-- Note: most of this code is translated from the ogre source ogrequaternion.cpp

Quaternion = Object:new()

-- rotates a vector
-- adapted from wikipedia pseudo-code http://en.wikipedia.org/wiki/Quaternion_rotation
function Quaternion:__mul(v)
    if (v[4]) then
        return self:quat_mult(v)
    else
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
end

-- rotate another quaternion by this one and return the result
function Quaternion:quat_mult(other)
    result = Quaternion:new()
--   w * rkQ.w - x * rkQ.x - y * rkQ.y - z * rkQ.z,
--   w * rkQ.x + x * rkQ.w + y * rkQ.z - z * rkQ.y,
--   w * rkQ.y + y * rkQ.w + z * rkQ.x - x * rkQ.z,
--   w * rkQ.z + z * rkQ.w + x * rkQ.y - y * rkQ.x
    result[4] = self[4] * other[4] - self[1] * other[1] - self[2] * other[2] - self[3] * other[3]
    result[1] = self[4] * other[1] + self[1] * other[4] + self[2] * other[3] - self[3] * other[2]
    result[2] = self[4] * other[2] + self[2] * other[4] + self[3] * other[1] - self[1] * other[3]
    result[3] = self[4] * other[3] + self[3] * other[4] + self[1] * other[2] - self[2] * other[1]
    return result
end

function Quaternion:toAngleAxis() 
    local sqr_length = self[1] * self[1] + self[2] * self[2] + self[3] * self[3]
    local angle = 0
    local axis = Vector:new()
    if (sqr_length > 0) then
        angle = 2 * math.acos(self[4])
        local inv_length = 1 / math.sqrt(sqr_length)
        axis[1] = self[1] * inv_length
        axis[2] = self[2] * inv_length
        axis[3] = self[3] * inv_length
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
    local half_angle = 0.5*angle
    local f_sin = math.sin(half_angle)
    self[4] = math.cos(half_angle);
    self[1] = f_sin*vector[1];
    self[2] = f_sin*vector[2];
    self[3] = f_sin*vector[3];
end

function Quaternion:fromRotationMatrix(kRot)
    -- Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
    -- article "Quaternion Calculus and Fast Animation".

    local fTrace = kRot[1][1]+kRot[2][2]+kRot[3][3]
    local fRoot

    if ( fTrace > 0.0 ) then
        -- |w| > 1/2, may as well choose w > 1/2
        fRoot = math.sqrt(fTrace + 1.0) -- 2w
        self[4] = 0.5*fRoot
        fRoot = 0.5/fRoot -- 1/(4w)
        self[1] = (kRot[3][2]-kRot[2][3])*fRoot
        self[2] = (kRot[1][3]-kRot[3][1])*fRoot
        self[3] = (kRot[2][1]-kRot[1][2])*fRoot
    else
        -- |w| <= 1/2
        s_iNext = { 2, 3, 1 }
        local i = 1
        if ( kRot[2][2] > kRot[1][1] ) then
            i = 2
        end
        if ( kRot[3][3] > kRot[i][i] ) then
            i = 3
        end
        local j = s_iNext[i]
        local k = s_iNext[j]

        fRoot = math.sqrt(kRot[i][i]-kRot[j][j]-kRot[k][k] + 1.0)
        self[i] = 0.5*fRoot
        fRoot = 0.5/fRoot
        self[4] = (kRot[k][j]-kRot[j][k])*fRoot
        self[j] = (kRot[j][i]+kRot[i][j])*fRoot
        self[k] = (kRot[k][i]+kRot[i][k])*fRoot
    end
end

function Quaternion:toRotationMatrix()
    -- return value
    local kRot = {}
    kRot[1] = {}
    kRot[2] = {}
    kRot[3] = {}

    local fTx  = 2.0*self[1]
    local fTy  = 2.0*self[2]
    local fTz  = 2.0*self[3]
    local fTwx = fTx*self[4]
    local fTwy = fTy*self[4]
    local fTwz = fTz*self[4]
    local fTxx = fTx*self[1]
    local fTxy = fTy*self[1]
    local fTxz = fTz*self[1]
    local fTyy = fTy*self[2]
    local fTyz = fTz*self[2]
    local fTzz = fTz*self[3]

    kRot[1][1] = 1.0-(fTyy+fTzz)
    kRot[1][2] = fTxy-fTwz
    kRot[1][3] = fTxz+fTwy
    kRot[2][1] = fTxy+fTwz
    kRot[2][2] = 1.0-(fTxx+fTzz)
    kRot[2][3] = fTyz-fTwx
    kRot[3][1] = fTxz-fTwy
    kRot[3][2] = fTyz+fTwx
    kRot[3][3] = 1.0-(fTxx+fTyy)

    return kRot
end

function Quaternion:fromAxes(xAxis, yAxis, zAxis)
    local kRot = {}
    kRot[1] = {}
    kRot[2] = {}
    kRot[3] = {}
 
    kRot[1][1] = xAxis[1];
    kRot[2][1] = xAxis[2];
    kRot[3][1] = xAxis[3];
 
    kRot[1][2] = yAxis[1];
    kRot[2][2] = yAxis[2];
    kRot[3][2] = yAxis[3];
 
    kRot[1][3] = zAxis[1];
    kRot[2][3] = zAxis[2];
    kRot[3][3] = zAxis[3];
 
    self:fromRotationMatrix(kRot);
end

function Quaternion:toAxes()
    kRot = self:toRotationMatrix(kRot);

    local x_axis = Vector:new()
    local y_axis = Vector:new()
    local z_axis = Vector:new()

    x_axis[1] = kRot[1][1]
    x_axis[2] = kRot[2][1]
    x_axis[3] = kRot[3][1]

    y_axis[1] = kRot[1][2]
    y_axis[2] = kRot[2][2]
    y_axis[3] = kRot[3][2]

    z_axis[1] = kRot[1][3]
    z_axis[2] = kRot[2][3]
    z_axis[3] = kRot[3][3]
    return x_axis, y_axis, z_axis
end
   

function Quaternion:__tostring()
    return self[1]..", "..self[2]..", "..self[3]..", "..self[4]
end
