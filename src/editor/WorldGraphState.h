#ifndef UEDIT_WORLD_GRAPH_STATE_H
#define UEDIT_WORLD_GRAPH_STATE_H

#include "ungod/application/Application.h"
#include "ungod/application/ScriptedGameState.h"
#include "ungod/base/WorldGraph.h"
#include "ungod/visual/Camera.h"
#include "ungod/content/EntityTypes.h"
#include "ungod/base/World.h"
#include <wx/wx.h>
#include <wx/colordlg.h>
#include "ungod/visual/Font.h"
#include "utilitySFML.h"
#include "CameraController.h"

namespace uedit
{
	class EditorCanvas;
	class EditorFrame;
	class EditorState;

	/** \brief A state that visualizes the current world graph. Each node is a colored rect with a name.
	* Connections between nodes are also marked. A double click to a node switches back to the Editor State with the
	* clicked node as the current graph reference node. */
	class WorldGraphState : public ungod::State
	{
	friend EditorCanvas;
	public:
		WorldGraphState(ungod::Application& app, ungod::StateID id, EditorState& editorState, EditorFrame* editorframe);

		virtual void handleEvent(const sf::Event& event) override;
		virtual void update(float delta) override;
		virtual void render(sf::RenderTarget& target, sf::RenderStates states) override;

	private:
		virtual void init() override;
		virtual void close() override;

	private: 
		EditorFrame* mEditorframe;
		EditorState& mEditorState;
		PressedKeyClickChecker mClickChecker;
		ungod::Camera mCamera;
		CameraController mCamContrl;
		std::unordered_map<ungod::WorldGraphNode*, sf::Color> mColorMapper;
		ungod::Font mFont;
		ungod::WorldGraphNode* mSelectedNode;
		bool mClicked;
		sf::Vector2i mMouseLastPos;
		ungod::WorldGraphNode* mConnect;
		int mCornerSelected;
		sf::Vector2f mTotalMove;
		static constexpr float SCALE = 0.02f;
		static constexpr int TEXTSIZE = 20000;
		static constexpr float CORNER_CLICK_RANGE = 0.05f; //in percentage of (node.width+node.height)
		static const sf::Color NODE_DEFAULT_COLOR;
		wxColourData mColorData;
	};
}

#endif // UEDIT_WORLD_GRAPH_STATE_H
