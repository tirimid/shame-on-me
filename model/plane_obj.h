#define plane_obj_VERT_CNT 4
#define plane_obj_IDX_CNT 6
static float const plane_obj_VertData[] =
{
	-1.0, 0.0, 1.0, 0.0, 0.0, -0.0, 1.0, -0.0,
	-1.0, -0.0, -1.0, 0.0, 1.0, -0.0, 1.0, -0.0,
	1.0, 0.0, 1.0, 1.0, 0.0, -0.0, 1.0, -0.0,
	1.0, -0.0, -1.0, 1.0, 1.0, -0.0, 1.0, -0.0,
};
static unsigned const plane_obj_IdxData[] =
{
	2, 1, 0,
	2, 3, 1,
};
