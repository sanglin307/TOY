#pragma once

enum class LightType
{
    Directional = 0,
    Spot,
    Point
};

class Light : public Component
{
public:
    struct Desc
    {
        LightType Type;
        glm::vec3 Color;

        /** Point and spot lights use candela (lm/sr) while directional use lux (lm/m^2) */
        float Intensity;
        /** Range for point and spot lights. If not present, range is infinite. */
        float Range;

        /** The inner and outer cone angles only apply to spot lights */
        float InnerConeAngle;
        float OuterConeAngle;
    };

    Light(const std::string& name, const Desc& desc);

    const std::string& GetName() const
    {
        return _Name;
    }
private:
    Desc _Desc;
    std::string _Name;
};