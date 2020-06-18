#ifndef EDITOR_CANVAS_H
#define EDITOR_CANVAS_H

#include "ungod/application/Application.h"
#include "ungod/application/ScriptedGameState.h"
#include "ungod/content/tilemap/TileMap.h"
#include "ungod/content/water/Water.h"
#include "ungod/base/World.h"
#include "ungod/visual/Camera.h"
#include "ungod/content/EntityTypes.h"
#include "ActionManager.h"
#include "CameraController.h"
#include <wx/wx.h>
#include "utilitySFML.h"
#include "ungod/visual/Font.h"

namespace uedit
{
    enum EditorStates { EDITOR_STATE, TILEMAP_EDIT_STATE, ENTITY_EDIT_STATE, WORLD_GRAPH_STATE };

    class EditorFrame;
    class EditorCanvas;
    class TileMapEditState;
    class EntityEditState;

    /** \brief A state that is runned by the underlying ungod-engine application.
    * World rendering and updating is done here. */
    class EditorState : public ungod::ScriptedGameState
    {
    friend EditorCanvas;
    friend TileMapEditState;
    friend EntityEditState;
    public:
        EditorState(ungod::Application& app, ungod::StateID id);

        /** \brief Sets the flag for updating the canvas content. */
        void setUpdateFlag(bool update);

        /** \brief Sets the flag for rendering the canvas content. */
        void setRenderFlag(bool render);

        virtual void handleEvent(const sf::Event& curEvent) override;
        virtual void update(const float delta) override;
        virtual void render(sf::RenderTarget& target, sf::RenderStates states) override;

    private:
        virtual void init() override;
        virtual void close() override;

    private:
        bool mUpdateFlag;   ///< if true, game world in canvas is updated constantly
        bool mRenderFlag;   ///< if true, game world in canvas is rendered
        bool mDebugRendering; ///< if true, game world in canvas is rendered with debug-info (only if render flag is also true)
        CameraController mCamContrl;
    };


    /** \brief A state that is toggled for entity editing. */
    class EntityEditState : public ungod::State
    {
    friend EditorCanvas;
    public:
        EntityEditState(ungod::Application& app, ungod::StateID id, EditorState* editorstate, EditorFrame* editorframe);

        virtual void handleEvent(const sf::Event& curEvent) override;
        virtual void update(const float delta) override;
        virtual void render(sf::RenderTarget& target, sf::RenderStates states) override;

        /** \brief Adds the given entity to the selection or removes it from the selection, if it is contained in it. */
        void selectEntity(ungod::Entity e);
        void clearEntitySelection();

    private:
        virtual void init() override;
        virtual void close() override;

        void onClick(const sf::Event& event);
        void openDesigners();

    private:
        EditorState* mEditorState;
        EditorFrame* mEFrame;
        bool mMouseLeftDown;
        bool mEntityMoveGoingOn;
        bool mDesignerOpenPressed;
        bool mCopyPressed;
        bool mPastePressed;
        sf::Vector2f mCopyTimeCenter;
        sf::Vector2i mMouseLastPos;
        std::vector<ungod::Entity> mSelectedEntities;
        std::vector<ungod::Entity> mCopiedEntities;
    };


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
	};


    /** \brief Defines the canvas of the editor, where the game world is rendered
    * and edit operations can be performed. */
    class EditorCanvas : public wxControl, public ungod::Application
    {
    friend class EditorFrame;
    public :
        EditorCanvas(EditorFrame* editorframe,
                     wxWindow* parent = nullptr,
                      wxWindowID id = -1,
                      const wxPoint& cPosition = wxDefaultPosition,
                      const wxSize& cSize = wxDefaultSize,
                      long cStyle = 0);

        virtual ~EditorCanvas();

        /** \brief Loads serialized project from file. */
        void load(const std::string& filepath);

        /** \brief Saves the current world and ground objects to file. */
        void save();

        EditorState* getEditorState() const { return mEditorState; }
        EntityEditState* getEntityEditState() const { return mEntityEditState; }

        const std::string& getCurrentFile() const { return mCurrentFile; }

        void lookAt(const sf::Vector2f& position) const;
        const sf::Vector2f& getCameraCenter() const;

        void selectEntity(ungod::Entity e) const;

    private :
        //game data
        EditorState* mEditorState;
        TileMapEditState* mGroundEditState;
        TileMapEditState* mWaterEditState;
        EntityEditState* mEntityEditState;
		WorldGraphState* mWorldGraphState;
        std::string mCurrentFile;

        /*
        * wx event handling
        */
        void OnIdle(wxIdleEvent& idleEvent);
        void OnPaint(wxPaintEvent& paintEvent);
        void OnEraseBackground(wxEraseEvent& eraseEvent) {}
        void onSizeChanged(wxSizeEvent& sizeEvent);

    public:
        wxDECLARE_EVENT_TABLE();
    };
}

#endif // WX_SFML_CANVAS_H
