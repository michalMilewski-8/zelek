#pragma once
class Parameters
{
public:
	float c1 = 20.0f;
	float c2 = 15.0f;
	float k = 1.0f;
	float mi = 1.0f;
	bool collision_version; // true - zmienna "mi" dzia�a na wszystkie sk�adowe, false - tylko na jedn�
	bool draw_bezier_springs = true;
	bool draw_frame_springs = true;
	bool draw_bezier_points = true;
	bool draw_frame = true;
	bool draw_bezier_box = true;
};