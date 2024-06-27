#include "Player.h"
#include "Vector3.h"
#include "ViewProjection.h"
#include "MathUtilityForText.h"

class Player;

class CameraController {

	static inline const float kInterpolatiomrate = 0.3f;
	static inline const float kVelocityBitz = 10.0f;

public:

	struct Rect {
		float left = 0.0f;
		float Rihgth = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;

	};

	void Initialize();
	void Update();
	void Reset();

	void setTarget(Player* target) { target_ = target; }
	void SetMovebleArea(Rect area) { movebleArea_ = area; }

	const ViewProjection& GetViewProjection() const { return viewProjection_; }

private:
	ViewProjection viewProjection_;
	Vector3 arriLevel_point_;
	Player* target_ = nullptr;
	Vector3 targetOffset_ = {0, 0, -15.0f};
	Rect movebleArea_ = {0, 100, 0, 100};

	static inline const Rect margin = {-50.0f, 50.0f, -50.0f, 50.0f};
};
