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
#include "wx/dialog.h"
#include "ungod/visual/Font.h"
#include "utilitySFML.h"
#include "CameraController.h"
#include "StatDisplayer.h"

namespace uedit
{
	class EditorCanvas;
	class EditorFrame;
	class EditorState;
	class NodeChangeDialog;

	/** \brief A state that visualizes the current world graph. Each node is a colored rect with a name.
	* Connections between nodes are also marked. A double click to a node switches back to the Editor State with the
	* clicked node as the current graph reference node. */
	class WorldGraphState : public ungod::State, public ungod::Serializable<WorldGraphState>
	{
	friend struct ungod::SerialBehavior<WorldGraphState>;
	friend struct ungod::DeserialBehavior<WorldGraphState>;
	friend EditorCanvas;
	public:
		WorldGraphState(ungod::Application& app, ungod::StateID id, EditorState& editorState, EditorFrame* editorframe);

		virtual void handleEvent(const sf::Event& event) override;
		virtual void update(float delta) override;
		virtual void render(sf::RenderTarget& target, sf::RenderStates states) override;

	private:
		virtual void init() override;
		virtual void close() override;

		void waitAll();

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
		static constexpr int TEXTSIZE = 80;
		static constexpr float CORNER_CLICK_RANGE = 500.0f; //in pixel/SCALE
		static const sf::Color NODE_DEFAULT_COLOR;
		wxColourData mColorData;
		NodeChangeDialog* mNodeChangeDialog;
	};


	class NodeChangeDialog : public wxDialog
	{
	public:
		NodeChangeDialog(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition);

		void setNode(ungod::WorldGraphNode* node);

		void refresh();

	private:
		ungod::WorldGraphNode* mNode;
		StatDisplay<float>* mPosX;
		StatDisplay<float>* mPosY;
		StatDisplay<float>* mSizeX;
		StatDisplay<float>* mSizeY;
		StatDisplay<std::string>* mIdentifier;
		StatDisplay<int>* mPriority;
		owls::SignalLink<void> mNodeChangedLink;

	private:
		void updateGraphRefPos();
	};
}

namespace ungod
{
	//define how to (de)serialize the filemanager
	template <>
	struct SerialIdentifier<uedit::WorldGraphState>
	{
		static std::string get() { return "WorldGraphState"; }
	};

	template <>
	struct SerialBehavior<uedit::WorldGraphState>
	{
		static void serialize(const uedit::WorldGraphState& data, MetaNode serializer, SerializationContext& context);
	};

	template <>
	struct DeserialBehavior<uedit::WorldGraphState>
	{
		static void deserialize(uedit::WorldGraphState& data, MetaNode deserializer, DeserializationContext& context);
	};
}

#endif // UEDIT_WORLD_GRAPH_STATE_H
