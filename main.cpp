#include <Novice.h>
#include "MatrixFunction.h"
#include <cstdint>
#include <cassert>
#include <imgui.h>
#include <algorithm>


const char kWindowTitle[] = "LC1C_25_ミヤモト_ナオヤ_ゲームタイトル";

float DISTANCE(Vector3 a, Vector3 b) {
	float distance = sqrtf((a.x - b.x) * (a.x - b.x) + (a.x - b.y) * (a.x - b.y) + (a.z - b.z) * (a.z - b.z));
	return distance;
}

Vector3 Add(Vector3 v1, Vector3 v2)
{
	Vector3 RVector3;
	RVector3.x = v1.x + v2.x;
	RVector3.y = v1.y + v2.y;
	RVector3.z = v1.z + v2.z;
	return RVector3;
}

Vector3 Subtract(Vector3 v1, Vector3 v2)
{
	Vector3 RVector3;
	RVector3.x = v1.x - v2.x;
	RVector3.y = v1.y - v2.y;
	RVector3.z = v1.z - v2.z;
	return RVector3;
}

Vector3 Multiply(float scalar, Vector3 v)
{
	Vector3 RVector3;
	RVector3.x = v.x * scalar;
	RVector3.y = v.y * scalar;
	RVector3.z = v.z * scalar;
	return RVector3;
}

float Dot(Vector3 v1, Vector3 v2)
{
	float Result;
	Result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return Result;
}

float Lenght(Vector3 v)
{
	float Result;
	Result = sqrtf(v.x * v.x + v.y * v.y);
	Result = sqrtf(Result * Result + v.z * v.z);
	return Result;
}

Vector3 Normalize(Vector3 v)
{
	Vector3 RVector3;
	float lenght = sqrtf(v.x * v.x + v.y * v.y);
	lenght = sqrtf(lenght * lenght + v.z * v.z);
	RVector3 = { (v.x / lenght),(v.y / lenght),(v.z / lenght) };
	return RVector3;
}

Vector3 MakeTransform(const Vector3& vector, const Matrix4x4& matrix)
{
	Vector3 resultVector;
	resultVector.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	resultVector.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	resultVector.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	//assert(w != 0.0f);
	resultVector.x /= w;
	resultVector.y /= w;
	resultVector.z /= w;

	return resultVector;
}

Vector3 Perpendicular(const Vector3& vector)
{
	if (vector.x != 0.0f || vector.y != 0.0f)
	{
		return{ -vector.y,vector.x,0.0f };
	}
	return { 0.0f,-vector.z,vector.y };
}

float Absolute(float x) {
	if (x < 0) {
		return -x;
	}
	return x;
}

struct Sphere {
	Vector3 center;
	float radius;
	int color;
};

struct Plane {
	Vector3 normal; //!< 法線
	float distance; //!< 距離
};

struct AABB {
	Vector3 min; //!< 最小点
	Vector3 max; //!< 最大点
};

Vector3 Cross(const Vector3& v1, const Vector3& v2) {

	Vector3 result;

	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;

	return result;
}

void DrawGrid(Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix)
{
	const float kGridHalfWidth = 2.0f;                                       // Gridの半分の幅
	const uint32_t kSubdivision = 10;                                        // 分割数
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision);  // 1つ分の長さ

	// 奥から手前への線を順々に引いていく
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {

		// ワールド座標系
		Matrix4x4 startWorldMatrix = MatrixFunction::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { (xIndex * kGridEvery),0.0f,kGridHalfWidth });
		Matrix4x4 endWorldMatrix = MatrixFunction::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { (xIndex * kGridEvery),0.0f,-kGridHalfWidth });

		// 同時クリップ空間
		Matrix4x4 startWorldViewProjectionMatrix = MatrixFunction::Multiply(startWorldMatrix, viewProjectionMatrix);
		Matrix4x4 endWorldViewProjectionMatrix = MatrixFunction::Multiply(endWorldMatrix, viewProjectionMatrix);

		// 正規化デバイス座標系
		Vector3 ndcStartVertex = MakeTransform({ -2,0,0 }, startWorldViewProjectionMatrix);
		Vector3 ndcEndVertex = MakeTransform({ -2,0,0 }, endWorldViewProjectionMatrix);

		// スクリーン座標系
		Vector3 screenStartVertex = MakeTransform(ndcStartVertex, viewportMatrix);
		Vector3 screenEndVertex = MakeTransform(ndcEndVertex, viewportMatrix);

		// 描画
		Novice::DrawLine(int(screenStartVertex.x), int(screenStartVertex.y), int(screenEndVertex.x), int(screenEndVertex.y), 0xAAAAAAFF);
	}

	// 左から右への線を順々に引いていく
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {

		// ワールド座標系
		Matrix4x4 startWorldMatrix = MatrixFunction::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { kGridHalfWidth,0.0f,(zIndex * kGridEvery) });
		Matrix4x4 endWorldMatrix = MatrixFunction::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { -kGridHalfWidth,0.0f,(zIndex * kGridEvery) });

		// 同時クリップ空間
		Matrix4x4 startWorldViewProjectionMatrix = MatrixFunction::Multiply(startWorldMatrix, viewProjectionMatrix);
		Matrix4x4 endWorldViewProjectionMatrix = MatrixFunction::Multiply(endWorldMatrix, viewProjectionMatrix);

		// 正規化デバイス座標系
		Vector3 ndcStartVertex = MakeTransform({ 0,0,-2 }, startWorldViewProjectionMatrix);
		Vector3 ndcEndVertex = MakeTransform({ 0,0,-2 }, endWorldViewProjectionMatrix);

		// スクリーン座標系
		Vector3 screenStartVertex = MakeTransform(ndcStartVertex, viewportMatrix);
		Vector3 screenEndVertex = MakeTransform(ndcEndVertex, viewportMatrix);

		// 描画
		Novice::DrawLine(int(screenStartVertex.x), int(screenStartVertex.y), int(screenEndVertex.x), int(screenEndVertex.y), 0xAAAAAAFF);
	}
}

void DrawSphere(const Sphere* sphere, Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix)
{
	const uint32_t kSubdivisiont = 12;  // 分割数

	const float kLonEvery = 2.0f * float(M_PI) / kSubdivisiont;  // 経度分割1つ分の角度
	const float kLatEvery = 2.0f * float(M_PI) / kSubdivisiont;  // 緯度分割1つ分の角度

	// 緯度の方向に分割　-π/2 ~ π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivisiont; ++latIndex) {
		float lat = -float(M_PI) / 2.0f + kLatEvery * latIndex; // 現在の軽度

		// 経度の方向に分割0 ~ 2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivisiont; ++lonIndex)
		{
			float lon = lonIndex * kLonEvery; // 現在の軽度

			Vector3 a, b, c;

			a = { sphere->radius * cosf(lon) * cosf(lat) + sphere->center.x,
				sphere->radius * sinf(lon) + sphere->center.y,
				sphere->radius * cosf(lon) * sinf(lat) + sphere->center.z };

			b = { sphere->radius * cosf(lon + kLonEvery) * cosf(lat) + sphere->center.x,
				sphere->radius * sinf(lon + kLonEvery) + sphere->center.y,
				sphere->radius * cosf(lon + kLonEvery) * sinf(lat) + sphere->center.z };

			c = { sphere->radius * cosf(lon) * cosf(lat + kLatEvery) + sphere->center.x
				,sphere->radius * sinf(lon) + sphere->center.y
				,sphere->radius * cosf(lon) * sinf(lat + kLatEvery) + sphere->center.z };

			// ワールド座標系
			Matrix4x4 aWorldMatrix = MatrixFunction::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, a);
			Matrix4x4 bWorldMatrix = MatrixFunction::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, b);
			Matrix4x4 cWorldMatrix = MatrixFunction::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, c);

			// 同時クリップ空間
			Matrix4x4 aWorldViewProjectionMatrix = MatrixFunction::Multiply(aWorldMatrix, viewProjectionMatrix);
			Matrix4x4 bWorldViewProjectionMatrix = MatrixFunction::Multiply(bWorldMatrix, viewProjectionMatrix);
			Matrix4x4 cWorldViewProjectionMatrix = MatrixFunction::Multiply(cWorldMatrix, viewProjectionMatrix);

			// 正規化デバイス座標系
			Vector3 ndcAVertex = MakeTransform({ 0,0,0 }, aWorldViewProjectionMatrix);
			Vector3 ndcBVertex = MakeTransform({ 0,0,0 }, bWorldViewProjectionMatrix);
			Vector3 ndcCVertex = MakeTransform({ 0,0,0 }, cWorldViewProjectionMatrix);

			// スクリーン座標系
			Vector3 screenAVertex = MakeTransform(ndcAVertex, viewportMatrix);
			Vector3 screenBVertex = MakeTransform(ndcBVertex, viewportMatrix);
			Vector3 screenCVertex = MakeTransform(ndcCVertex, viewportMatrix);

			// ab,ac で線を引く
			Novice::DrawLine(int(screenAVertex.x), int(screenAVertex.y), int(screenBVertex.x), int(screenBVertex.y), sphere->color);
			Novice::DrawLine(int(screenAVertex.x), int(screenAVertex.y), int(screenCVertex.x), int(screenCVertex.y), sphere->color);
		}
	}
}

void DarwPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color)
{
	Vector3 center = Multiply(plane.distance, plane.normal); // 1
	Vector3 perpendiculars[4];
	perpendiculars[0] = Normalize(Perpendicular(plane.normal));
	perpendiculars[1] = { -perpendiculars[0].x, -perpendiculars[0].y, -perpendiculars[0].z };
	perpendiculars[2] = Cross(plane.normal, perpendiculars[0]);
	perpendiculars[3] = { -perpendiculars[2].x, -perpendiculars[2].y, -perpendiculars[2].z };

	Vector3 points[4];
	for (int32_t index = 0; index < 4; ++index)
	{
		Vector3 extend = Multiply(2.0f, perpendiculars[index]);
		Vector3 point = Add(center, extend);
		points[index] = MakeTransform(MakeTransform(point, viewProjectionMatrix), viewportMatrix);
	}
	Novice::DrawLine(static_cast<int>(points[0].x), static_cast<int>(points[0].y), static_cast<int>(points[3].x), static_cast<int>(points[3].y), color);
	Novice::DrawLine(static_cast<int>(points[1].x), static_cast<int>(points[1].y), static_cast<int>(points[3].x), static_cast<int>(points[3].y), color);
	Novice::DrawLine(static_cast<int>(points[2].x), static_cast<int>(points[2].y), static_cast<int>(points[1].x), static_cast<int>(points[1].y), color);
	Novice::DrawLine(static_cast<int>(points[2].x), static_cast<int>(points[2].y), static_cast<int>(points[0].x), static_cast<int>(points[0].y), color);
}

void DrawAABB(const AABB& aabb, Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportmatrx, uint32_t color)
{
	Vector3 minXYZ = aabb.min;
	Vector3 minXYmaxZ = { aabb.min.x,aabb.min.y,aabb.max.z };
	Vector3 minYZmaxX = { aabb.max.x,aabb.min.y,aabb.min.z };
	Vector3 minYmaxXZ = { aabb.max.x,aabb.min.y,aabb.max.z };

	// ワールド座標系
	Matrix4x4 minXYZWorldMatrix = MatrixFunction::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, minXYZ);
	Matrix4x4 minXYmaxZWorldMatrix = MatrixFunction::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, minXYmaxZ);
	Matrix4x4 minYZmaxXWorldMatrix = MatrixFunction::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, minYZmaxX);
	Matrix4x4 minYmaxXZWorldMatrix = MatrixFunction::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, minYmaxXZ);

	// 同時クリップ空間
	Matrix4x4 minXYZWorldViewProjectionMatrix = MatrixFunction::Multiply(minXYZWorldMatrix, viewProjectionMatrix);
	Matrix4x4 minXYmaxZWorldViewProjectionMatrix = MatrixFunction::Multiply(minXYmaxZWorldMatrix, viewProjectionMatrix);
	Matrix4x4 minYZmaxXWorldViewProjectionMatrix = MatrixFunction::Multiply(minYZmaxXWorldMatrix, viewProjectionMatrix);
	Matrix4x4 minYmaxXZWorldViewProjectionMatrix = MatrixFunction::Multiply(minYmaxXZWorldMatrix, viewProjectionMatrix);

	// 正規化デバイス座標系
	Vector3 ndcminXYZVertex = MakeTransform({ 0,0,0 }, minXYZWorldViewProjectionMatrix);
	Vector3 ndcminXYmaxZVertex = MakeTransform({ 0,0,0 }, minXYmaxZWorldViewProjectionMatrix);
	Vector3 ndcminYZmaxXVertex = MakeTransform({ 0,0,0 }, minYZmaxXWorldViewProjectionMatrix);
	Vector3 ndcminYmaxXZVertex = MakeTransform({ 0,0,0 }, minYmaxXZWorldViewProjectionMatrix);

	// スクリーン座標系
	Vector3 screenminXYZ = MakeTransform(ndcminXYZVertex, viewportmatrx);
	Vector3 screenminXYmaxZ = MakeTransform(ndcminXYmaxZVertex, viewportmatrx);
	Vector3 screenminYZmaxX = MakeTransform(ndcminYZmaxXVertex, viewportmatrx);
	Vector3 screenminYmaxXZ = MakeTransform(ndcminYmaxXZVertex, viewportmatrx);




	Vector3 maxXYZ = aabb.max;
	Vector3 maxXYminZ = { aabb.max.x,aabb.max.y,aabb.min.z };
	Vector3 maxYZminX = { aabb.min.x,aabb.max.y,aabb.max.z };
	Vector3 maxYminXZ = { aabb.min.x,aabb.max.y,aabb.min.z };

	// ワールド座標系
	Matrix4x4 maxXYZWorldMatrix = MatrixFunction::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, maxXYZ);
	Matrix4x4 maxXYminZWorldMatrix = MatrixFunction::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, maxXYminZ);
	Matrix4x4 maxYZminXWorldMatrix = MatrixFunction::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, maxYZminX);
	Matrix4x4 maxYminXZWorldMatrix = MatrixFunction::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, maxYminXZ);

	// 同時クリップ空間
	Matrix4x4 maxXYZWorldViewProjectionMatrix = MatrixFunction::Multiply(maxXYZWorldMatrix, viewProjectionMatrix);
	Matrix4x4 maxXYminZWorldViewProjectionMatrix = MatrixFunction::Multiply(maxXYminZWorldMatrix, viewProjectionMatrix);
	Matrix4x4 maxYZminXWorldViewProjectionMatrix = MatrixFunction::Multiply(maxYZminXWorldMatrix, viewProjectionMatrix);
	Matrix4x4 maxYminXZWorldViewProjectionMatrix = MatrixFunction::Multiply(maxYminXZWorldMatrix, viewProjectionMatrix);

	// 正規化デバイス座標系
	Vector3 ndcmaxXYZVertex = MakeTransform({ 0,0,0 }, maxXYZWorldViewProjectionMatrix);
	Vector3 ndcmaxXYminZVertex = MakeTransform({ 0,0,0 }, maxXYminZWorldViewProjectionMatrix);
	Vector3 ndcmaxYZminXVertex = MakeTransform({ 0,0,0 }, maxYZminXWorldViewProjectionMatrix);
	Vector3 ndcmaxYminXZVertex = MakeTransform({ 0,0,0 }, maxYminXZWorldViewProjectionMatrix);

	// スクリーン座標系
	Vector3 screenmaxXYZ = MakeTransform(ndcmaxXYZVertex, viewportmatrx);
	Vector3 screenmaxXYminZ = MakeTransform(ndcmaxXYminZVertex, viewportmatrx);
	Vector3 screenmaxYZminX = MakeTransform(ndcmaxYZminXVertex, viewportmatrx);
	Vector3 screenmaxYminXZ = MakeTransform(ndcmaxYminXZVertex, viewportmatrx);

	Novice::DrawLine(static_cast<int>(screenminXYZ.x), static_cast<int>(screenminXYZ.y), static_cast<int>(screenminXYmaxZ.x), static_cast<int>(screenminXYmaxZ.y), color);
	Novice::DrawLine(static_cast<int>(screenminXYZ.x), static_cast<int>(screenminXYZ.y), static_cast<int>(screenminYZmaxX.x), static_cast<int>(screenminYZmaxX.y), color);
	Novice::DrawLine(static_cast<int>(screenminXYZ.x), static_cast<int>(screenminXYZ.y), static_cast<int>(screenmaxYminXZ.x), static_cast<int>(screenmaxYminXZ.y), color);//

	Novice::DrawLine(static_cast<int>(screenmaxYZminX.x), static_cast<int>(screenmaxYZminX.y), static_cast<int>(screenmaxYminXZ.x), static_cast<int>(screenmaxYminXZ.y), color);//
	Novice::DrawLine(static_cast<int>(screenmaxYZminX.x), static_cast<int>(screenmaxYZminX.y), static_cast<int>(screenminXYmaxZ.x), static_cast<int>(screenminXYmaxZ.y), color);//
	Novice::DrawLine(static_cast<int>(screenmaxYZminX.x), static_cast<int>(screenmaxYZminX.y), static_cast<int>(screenmaxXYZ.x), static_cast<int>(screenmaxXYZ.y), color);//

	Novice::DrawLine(static_cast<int>(screenminYmaxXZ.x), static_cast<int>(screenminYmaxXZ.y), static_cast<int>(screenminXYmaxZ.x), static_cast<int>(screenminXYmaxZ.y), color);//
	Novice::DrawLine(static_cast<int>(screenminYmaxXZ.x), static_cast<int>(screenminYmaxXZ.y), static_cast<int>(screenmaxXYZ.x), static_cast<int>(screenmaxXYZ.y), color);//
	Novice::DrawLine(static_cast<int>(screenminYmaxXZ.x), static_cast<int>(screenminYmaxXZ.y), static_cast<int>(screenminYZmaxX.x), static_cast<int>(screenminYZmaxX.y), color);//

	Novice::DrawLine(static_cast<int>(screenmaxXYminZ.x), static_cast<int>(screenmaxXYminZ.y), static_cast<int>(screenmaxXYZ.x), static_cast<int>(screenmaxXYZ.y), color);//
	Novice::DrawLine(static_cast<int>(screenmaxXYminZ.x), static_cast<int>(screenmaxXYminZ.y), static_cast<int>(screenminYZmaxX.x), static_cast<int>(screenminYZmaxX.y), color);
	Novice::DrawLine(static_cast<int>(screenmaxXYminZ.x), static_cast<int>(screenmaxXYminZ.y), static_cast<int>(screenmaxYminXZ.x), static_cast<int>(screenmaxYminXZ.y), color);
}

bool IsCollision(const Sphere& s1, const Sphere& s2)
{
	float distance = sqrtf(float(pow(s1.center.x - s2.center.x, 2.0f)) + float(pow(s1.center.y - s2.center.y, 2.0f)) + float(pow(s1.center.z - s2.center.z, 2.0f)));

	if (distance <= s1.radius + s2.radius)
	{
		return true;
	}
	else if (distance > s1.radius + s2.radius)
	{
		return false;
	}
	else
	{
		return false;
	}
}

bool SpherePlaneCollision(Sphere& sphere, const Plane& plane) {
	float distance = ((plane.normal.x * sphere.center.x) + (plane.normal.y * sphere.center.y) + (plane.normal.z * sphere.center.z)) - plane.distance;

	float absoluteRadius = Absolute(sphere.radius);

	distance = Absolute(distance);

	if (distance <= absoluteRadius)
	{
		return true;
	}
	else {
		return false;
	}
}

bool AABBtoAABBCollision(AABB& a, AABB b) {
	if ((a.min.x <= b.max.x && a.max.x >= b.min.x) &&
		(a.min.y <= b.max.y && a.max.y >= b.min.y) &&
		(a.min.z <= b.max.z && a.max.z >= b.min.z)) {

		return true;
	}
	else {
		return false;
	}
}

bool AABBSphereCollision(AABB& aabb, Sphere& sphere) {
	Vector3 closestPoint{ std::clamp(sphere.center.x,aabb.min.x,aabb.max.x),std::clamp(sphere.center.y,aabb.min.y,aabb.max.y) ,std::clamp(sphere.center.z,aabb.min.z,aabb.max.z) };

	float distance = DISTANCE(closestPoint, sphere.center);

	if (distance <= sphere.radius) {
		return true;
	}
	else {
		return false;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	Vector3 cameraTranslate{ 0.0f,1.9f,-6.49f };

	Vector3 cameraRotate{ 0.26F,0.0F,0.0F };

	AABB aabb1 = {};

	Sphere sphere = {};

	uint32_t AABB1Color = WHITE;

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		ImGui::Begin("Window");
		ImGui::DragFloat3("CameraTranslate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("AABB1.min", &aabb1.min.x, 0.1f);
		ImGui::DragFloat3("AABB1.max", &aabb1.max.x, 0.1f);
		ImGui::DragFloat3("sphere.center", &sphere.center.x, 0.01f);
		ImGui::DragFloat("sphere.radius", &sphere.radius, 0.01f);
		ImGui::End();


		Matrix4x4 cameraMatrix = MatrixFunction::MakeAffineMatrix({ 1.0f, 1.0f,1.0f }, cameraRotate, cameraTranslate);

		Matrix4x4 viewMatrix = MatrixFunction::Inverse(cameraMatrix);

		Matrix4x4 projectionMatrix = MatrixFunction::MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.f);

		Matrix4x4 viewportMatrix = MatrixFunction::MakeViewportMatrix(0, 0, 1280.0f, 720.0f, 0.0f, 1.0f);

		Matrix4x4 viewProjectionMatrix = MatrixFunction::Multiply(viewMatrix, projectionMatrix);

		if (AABBSphereCollision(aabb1,sphere))
		{
			AABB1Color = RED;
			sphere.color = WHITE;
		}
		else {
			AABB1Color = WHITE;
			sphere.color = WHITE;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawGrid(viewProjectionMatrix, viewportMatrix);

		DrawAABB(aabb1, viewProjectionMatrix, viewportMatrix, AABB1Color);

		DrawSphere(&sphere, viewProjectionMatrix, viewportMatrix);
		
		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}