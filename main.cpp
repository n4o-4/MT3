#include <Novice.h>
#include "MatrixFunction.h"
#include <cstdint>
#include <cassert>
#include <imgui.h>


const char kWindowTitle[] = "LC1C_25_ミヤモト_ナオヤ_ゲームタイトル";

Vector3 Cross(const Vector3& v1, const Vector3& v2) {

	Vector3 result;

	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;

	return result;
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

struct Sphere {
	Vector3 center;
	float radius;
};

void DrawGrid( Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix)
{
	const float kGridHalfWidth = 2.0f;                                       // Gridの半分の幅
	const uint32_t kSubdivision = 10;                                        // 分割数
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision);  // 1つ分の長さ

	// 奥から手前への線を順々に引いていく
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {

		// ワールド座標系
		Matrix4x4 startWorldMatrix = MatrixFunction::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { (xIndex * kGridEvery),0.0f,kGridHalfWidth});
		Matrix4x4 endWorldMatrix = MatrixFunction::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { (xIndex * kGridEvery),0.0f,-kGridHalfWidth});

		// 同時クリップ空間
		Matrix4x4 startWorldViewProjectionMatrix = MatrixFunction::Multiply(startWorldMatrix, viewProjectionMatrix);
		Matrix4x4 endWorldViewProjectionMatrix = MatrixFunction::Multiply(endWorldMatrix, viewProjectionMatrix);

		// 正規化デバイス座標系
		Vector3 ndcStartVertex = MakeTransform({-2,0,0}, startWorldViewProjectionMatrix);
		Vector3 ndcEndVertex = MakeTransform({-2,0,0}, endWorldViewProjectionMatrix);

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
				sphere->radius * cosf(lon) * sinf(lat)+ sphere->center.z };

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
			Novice::DrawLine(int(screenAVertex.x), int(screenAVertex.y), int(screenBVertex.x), int(screenBVertex.y), 0xAAAAAAFF);
			Novice::DrawLine(int(screenAVertex.x), int(screenAVertex.y), int(screenCVertex.x), int(screenCVertex.y), 0xAAAAAAFF);
		}
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	Vector3 cameraTranslate{ 0.0f,1.9f,-6.49f };

	Vector3 cameraRotate{ 0.26F,0.0F,0.0F };

	Sphere sphere = {
		{0,0,0},
		1.0f,
	};

	

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

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
		ImGui::DragFloat3("SphereCenter", &sphere.center.x, 0.01f);
		ImGui::DragFloat("CameraTranslate", &sphere.radius, 0.01f);
		ImGui::End();

		Matrix4x4 cameraMatrix = MatrixFunction::MakeAffineMatrix({ 1.0f, 1.0f,1.0f }, cameraRotate, cameraTranslate);

		Matrix4x4 viewMatrix = MatrixFunction::Inverse(cameraMatrix);

		Matrix4x4 projectionMatrix = MatrixFunction::MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.f);

		Matrix4x4 viewportMatrix = MatrixFunction::MakeViewportMatrix(0, 0, 1280.0f, 720.0f, 0.0f, 1.0f);

		Matrix4x4 viewProjectionMatrix = MatrixFunction::Multiply(viewMatrix, projectionMatrix);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		

		DrawGrid(viewProjectionMatrix, viewportMatrix);

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
