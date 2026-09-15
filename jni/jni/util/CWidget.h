struct WidgetPosition
{
	float originX;
	float originY;
	float scaleX;
	float scaleY;
};

enum HIDMapping {  };


class CGameWidget
{
public:
	uint32_t field_0;
	uint32_t field_4;
	uint32_t field_8;
	WidgetPosition position;
	float field_1C;
	CRect displayPosition;
	float scaleFactor;
};