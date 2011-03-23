#include "FringeTileEditor.h"
#include "../Graphics/FringeTile.h"
#include "../Level.h"
#include "../Scene.h"
#include "../Input.h"
#include "../Monocle.h"
#include <cstdio>

namespace Monocle
{
	FringeTileEditor::FringeTileEditor()
		: scene(NULL), selectedEntity(NULL), selectedFringeTile(NULL), isOn(false), state(FTES_NONE)
	{
	}

	void FringeTileEditor::Init(Scene *scene)
	{
		this->scene = scene;
	}

	void FringeTileEditor::Enable()
	{
		isOn = true;

		Sprite::showBounds = true;
	}

	void FringeTileEditor::Disable()
	{
		isOn = false;

		Sprite::showBounds = false;
	}

	void FringeTileEditor::Update()
	{
		if (isOn)
		{
			UpdateCamera();
			if (state == FTES_NONE)
			{
				UpdateSelect();
			}

			if (selectedEntity)
			{
				switch (state)
				{
				case FTES_NONE:
					UpdateOpportunity();
					break;

				case FTES_MOVE:
					UpdateMove();
					break;

				case FTES_ROTATE:
					UpdateRotate();
					break;

				case FTES_SCALE:
					UpdateScale();
					break;
				}
			}
		}
	}

	void FringeTileEditor::UpdateCamera()
	{
		if (!Input::IsKeyHeld(KEY_LCTRL))
		{
			float moveSpeed = 200.0f;
			float zoomSpeed = 0.5f;

			if (Input::IsKeyHeld(KEY_LSHIFT))
			{
				moveSpeed *= 5.0f;
				zoomSpeed *= 2.0f;
			}
			if (Input::IsKeyHeld(KEY_D))
			{
				Graphics::AdjustCameraPosition(Vector2::right * moveSpeed * Monocle::deltaTime);
			}
			if (Input::IsKeyHeld(KEY_A))
			{
				Graphics::AdjustCameraPosition(Vector2::left * moveSpeed * Monocle::deltaTime);
			}
			if (Input::IsKeyHeld(KEY_W))
			{
				Graphics::AdjustCameraPosition(Vector2::up * moveSpeed * Monocle::deltaTime);
			}
			if (Input::IsKeyHeld(KEY_S))
			{
				Graphics::AdjustCameraPosition(Vector2::down * moveSpeed * Monocle::deltaTime);
			}
			if (Input::IsKeyHeld(KEY_Q))
			{
				Graphics::AdjustCameraZoom(-1*Vector2::one * zoomSpeed * Monocle::deltaTime);
			}
			if (Input::IsKeyHeld(KEY_E))
			{
				Graphics::AdjustCameraZoom(Vector2::one * zoomSpeed * Monocle::deltaTime);
			}
		}
	}

	void FringeTileEditor::UpdateSelect()
	{
		if (Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || Input::IsKeyPressed(KEY_Z))
		{
			Debug::Log("select");

			printf("mouse position: %d, %d", (int)Input::GetMousePosition().x, (int)Input::GetMousePosition().y);

			Entity *entity = scene->GetNearestEntityContaining(Input::GetMousePosition(), selectedEntity);//scene->GetEntityAtPosition(Input::GetMousePosition());
			if (entity)
			{
				Select(entity);
			}
		}
	}

	void FringeTileEditor::Clone()
	{
		if (selectedEntity && selectedFringeTile)
		{
			FringeTile *fringeTile = Level::AddFringeTile(selectedFringeTile->GetFringeTileset(), selectedFringeTile->GetTileID(), selectedEntity->GetLayer(), Input::GetMousePosition(), selectedEntity->scale, selectedEntity->rotation);
			Select(fringeTile->entity);
		}
	}

	void FringeTileEditor::Select(Entity *entity)
	{
		selectedEntity = entity;
		Sprite::selectedSpriteEntity = selectedEntity;
		if (entity)
		{
			// unhappy with this so far:
			selectedFringeTile = Level::GetFringeTileForEntity(selectedEntity);
		}
		else
		{
			selectedFringeTile = NULL;
		}
	}

	void FringeTileEditor::UpdateOpportunity()
	{
		if (selectedFringeTile)
		{
			if (Input::IsKeyPressed(KEY_MINUS))
			{
				selectedFringeTile->PrevTile();
			}
			if (Input::IsKeyPressed(KEY_EQUALS))
			{
				selectedFringeTile->NextTile();
			}
			if (Input::IsKeyPressed(KEY_SPACE))
			{
				Clone();
			}
			if (Input::IsKeyPressed(KEY_1))
			{
				Monocle::OpenURL("C:\\Program Files (x86)\\Adobe\\Adobe Photoshop CS5\\Photoshop.exe " + selectedFringeTile->texture->filename);
			}
		}

		if (Input::IsKeyPressed(KEY_I))
		{
			selectedEntity->AdjustLayer(1);
			printf("layer is now: %d\n", selectedEntity->GetLayer());
		}
		if (Input::IsKeyPressed(KEY_K))
		{
			selectedEntity->AdjustLayer(-1);
			printf("layer is now: %d\n", selectedEntity->GetLayer());
		}

		if (Input::IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			moveStartPosition = selectedEntity->position;
			moveOffset = selectedEntity->position - Input::GetMousePosition();
			SetState(FTES_MOVE);
			return;
		}
	}

	void FringeTileEditor::UpdateMove()
	{
		selectedEntity->position = Input::GetMousePosition() + moveOffset;
		
		// cancel out of move by hitting escape
		if (Input::IsKeyPressed(KEY_ESCAPE))
		{
			selectedEntity->position = moveStartPosition;
			SetState(FTES_NONE);
			return;
		}

		// if let go, return to none state
		if (Input::IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
		{
			SetState(FTES_NONE);
			return;
		}
	}

	void FringeTileEditor::UpdateRotate()
	{
		// let go, return to none state
		if (Input::IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
		{
			SetState(FTES_NONE);
		}
	}

	void FringeTileEditor::UpdateScale()
	{
		if (Input::IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
		{
			SetState(FTES_NONE);
		}
	}

	void FringeTileEditor::SetState(FringeTileEditorState state)
	{
		this->state = state;
	}
}