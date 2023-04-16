#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"
#include "Engine/Scene/Entity.h"
#include "Editor/Panels/EditorPanel.h"
#include "Editor/Scene/EditorScene.h"
#include "Engine/Renderer/Framebuffer.h"

#include "Engine/Events/MouseEvent.h"
#include "Engine/Events/KeyEvent.h"

#include <ImGuizmo.h>

namespace Engine
{
	class ViewportPanel : public EditorPanel
	{
	public:
		ViewportPanel();

		virtual void OnCreate() override;

		virtual void OnUpdate(float DeltaTime) override;

		virtual void OnRender(float DeltaTime) override;

		virtual void OnResize(uint32_t width, uint32_t height) override;

		virtual void OnEvent(Event& event) override;

	private:
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& event);
		bool OnKeyPressedEvent(KeyPressedEvent& event);

		virtual void OnData(const WString& Name, void* Data, size_t size) override;

		void OnSetActiveScene(Scene* NewScene);

		void OnBeginPlay();
		void OnEndPlay();

		void RenderToolbar(float CursorPosY);

	private:
		struct SnapGridInfo
		{
			bool Enabled = false;
			bool ShowPopup = false;
			float Value = 1.f;
		};

	private:
		Scene* ActiveScene = nullptr;
		Ref<Framebuffer> m_FrameBuffer = nullptr;

		Entity HoveredEntity = { };
		Entity SelectedEntity = { };

		int GizmoType = ImGuizmo::OPERATION::TRANSLATE;

		bool bIsViewportFocused = false,
			bIsViewportHovered = false,
			bIsCopyingEntity = false;

		SnapGridInfo PositionSnap = { false, false, 1.f },
			RotationSnap = { false, false, 10.f },
			ScaleSnap = { false, false, 0.25f };
	};
}
