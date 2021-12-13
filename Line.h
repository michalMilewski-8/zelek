#pragma once
#include "Object.h"
; class Line :
    public Object,
    public std::enable_shared_from_this<Line>
{
public:
    Line(Shader& sh);

    void DrawObject(glm::mat4 mvp) override;
    void CreateMenu() override {};

    void AddPoint(glm::vec3 point);
    void Update() override;
    int size() { return points_.size(); };
    void ClearPoints();
    void SetColor(glm::vec4 col) { color = col; }
    void SetLineWidth(float width_) { width = width_; }
    glm::vec3 GetLastPoint() { return points_.size() > 0 ? points_.back() : glm::vec3(0,0,0); };

    void Reverse();

    int max_points = 100;

    static unsigned int counter;
private:

    void update_object() override;

    std::vector<float> points_on_curve;
    std::vector<unsigned int> lines;
    float width = 1.0f;
   

    std::vector<glm::vec3> points_;
};

