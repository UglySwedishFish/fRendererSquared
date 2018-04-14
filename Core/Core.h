#pragma once
#include <Dependencies\DependenciesMath.h> 
#include <Rendering\Window.h> 
#define PI 3.14159265

namespace fRenderer {
	namespace Core {
		template<typename T> 
		inline T mix(T A, T B, float Mix) {
			return A * static_cast<T>(Mix) + B * static_cast<T>(1.0 - Mix);
		}
		template<typename T>
		T fRenderer::Core::clamp(T A, T Max, T Min)
		{
			return A > Max ? Max : A < Min ? Min : A;
		}
		template<typename T> 
		T Interpolate(T A, T B, T X); 
		template<typename T, typename V>
		void Move_2DXY(V & Pos, T Speed, T Rotation, T FrameTime)
		{
			Pos.x -= cos(Rotation*(PI / 180.))*Speed*FrameTime;
			Pos.y -= sin(Rotation*(PI / 180.))*Speed*FrameTime;

		}
		template<typename T, typename V>
		void Move_2DXZ(V & Pos, T Speed, T Rotation, T FrameTime)
		{
			Pos.x -= cos(Rotation*(PI / 180.))*Speed*FrameTime;
			Pos.z -= sin(Rotation*(PI / 180.))*Speed*FrameTime;
		}
		template<typename T, typename V>
		void Move_3D(V & Pos, T Speed, T RotationX, T RotationY, T FrameTime)
		{
			Pos.x -= cos(RotationY*(PI / 180.))*Speed*FrameTime;
			Pos.y += sin(RotationX*(PI / 180.))*Speed*FrameTime;
			Pos.z -= sin(RotationY*(PI / 180.))*Speed*FrameTime;
		}
		template<class T>
		bool inRange(T value, T min, T max) { //is a value in a certain range
			return value > min && value < max;
		}

		template<typename T, typename F> 
		inline void HandleInput(T & Camera, F MovementSpeed, F MouseSense, Window & Window, bool Rotation, bool Position) {
			if (Position) {

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
					Window.SetFrameCount(0);
					Move_2DXZ(Camera.Position, MovementSpeed, static_cast<F>(Camera.Rotation.y - 180.0), static_cast<F>(Window.GetFrameTime()));

				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
					Window.SetFrameCount(0);
					Move_2DXZ(Camera.Position, MovementSpeed, static_cast<F>(Camera.Rotation.y), static_cast<F>(Window.GetFrameTime()));
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
					Window.SetFrameCount(0);

					Move_3D(Camera.Position, MovementSpeed, static_cast<F>(Camera.Rotation.x), static_cast<F>(Camera.Rotation.y - 90.0), static_cast<F>(Window.GetFrameTime()));


				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
					Window.SetFrameCount(0);
					Move_3D(Camera.Position, MovementSpeed, static_cast<F>(Camera.Rotation.x + 180.0), static_cast<F>(Camera.Rotation.y + 90.0), static_cast<F>(Window.GetFrameTime()));
				}
			}
			if (Rotation) {
				float RelativeMouseX = sf::Mouse::getPosition(*Window.GetRawWindow()).x - Window.GetResolution().x / 2;
				float RelativeMouseY = sf::Mouse::getPosition(*Window.GetRawWindow()).y - Window.GetResolution().y / 2;


				sf::Mouse::setPosition(sf::Vector2i(Window.GetResolution().x / 2, Window.GetResolution().y / 2), *Window.GetRawWindow());

				Vector2f PreviousCameraRotation = Vector2f(Camera.Rotation.x, Camera.Rotation.y);

				std::cout << RelativeMouseX << ' ' << RelativeMouseY << '\n';


				Camera.Rotation.y += RelativeMouseX*MouseSense;
				Camera.Rotation.x += static_cast<float>(RelativeMouseY*MouseSense); 
				if (!(inRange(F(Camera.Rotation.x), F(PreviousCameraRotation.x - 0.1), F(PreviousCameraRotation.x + 0.1)) && inRange(F(Camera.Rotation.y), F(PreviousCameraRotation.y - 0.1), F(PreviousCameraRotation.y + 0.1))))
					Window.SetFrameCount(0);
				Camera.Rotation.y = Camera.Rotation.y > 360.0 ? Camera.Rotation.y - 360.0 : Camera.Rotation.y < 0.0 ? Camera.Rotation.y + 360.0 : Camera.Rotation.y;

			}

			//Window.FrameCountOpposite++;

			//if (Window.FrameCount != 0)
			//	Window.FrameCountOpposite = 0;
		}

		Matrix4f ViewMatrix(Vector3f Position, Vector3f Rotation); 
		Matrix4f ModelMatrix(Vector3f Position, Vector3f Rotation); 
		Matrix4f ShadowOrthoMatrix(float edge, float znear, float zfar); 
		template<typename T>  
		T Lerp(T A, T B, T F); 
	}
}