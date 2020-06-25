#ifndef EDITOR_FRAME_H
#define EDITOR_FRAME_H

#include "Canvas.h"
#include "ActionManager.h"
#include "SheetPreview.h"
#include "ScriptManager.h"
#include "WorldCreateDialog.h"
#include "ProjectCreateDialog.h"
#include "EntityDesigner.h"
#include "LayerDisplay.h"
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/filepicker.h>
#include <wx/combobox.h>
#include <wx/notebook.h>

namespace uedit
{
    class EditorCanvas;
    class EditorState;
    class EntityDesigner;


	//metainfo that is automatically serialized and deserialized
	struct MetaInfo : public ungod::Serializable<MetaInfo>
	{
		std::string lastProject;
	};

    /**
    * \brief Main window of the editor.
    */
    class EditorFrame : public wxFrame, public ungod::Serializable<EditorFrame>
    {
    friend struct ungod::SerialBehavior<EditorFrame>;
    friend struct ungod::DeserialBehavior<EditorFrame>;
    public:
        EditorFrame();

        SheetPreview* getSheetPreview() { return mSheetPreview; }
        ScriptManager* getScriptManager() { return mScriptManager; }
        LayerDisplay* getLayerDisplay() { return mLayerDisplay; }
        EditorCanvas* getCanvas() { return mCanvas; }

        void clearEntityDesigner(EntityDesigner* ed);

        ungod::World* getSelectedWorld();

        void addEntity(ungod::Entity e);
        void addEntities(std::vector<ungod::Entity> entities, const sf::Vector2f& relativeTo);

        //opens a new entity desginer for the given entity, if no designer is currently open for it
        void openEntityDesigner(ungod::Entity e);

        ActionManager& getActionManager() { return mActionManager; }

        void registerWorld(ungod::World* world);

        void notifyProjectChanged();

		virtual ~EditorFrame();

    private:
        bool mContentSaved;  ///< indicates whether unsaved changes are currently made
        wxPanel* mParent;
        EditorCanvas* mCanvas;
        SheetPreview* mSheetPreview;
        ScriptManager* mScriptManager;
        LayerDisplay* mLayerDisplay;
        EditorStates mCurrentState;
        std::vector<EntityDesigner*> mActiveDesigners;
        wxNotebook* mEditorTabs;
        ActionManager mActionManager;
        std::string mProjectFilePath;
		MetaInfo mMetaInfo;

    private:
        void onFileNew(wxCommandEvent& event);
        void onFileLoad(wxCommandEvent& event);
        void onFileSave(wxCommandEvent& event);

        void onWorldNew(wxCommandEvent& event);
		void onNodeNew(wxCommandEvent& event);
        void onStateProperties(wxCommandEvent& event);

        void onEditUndo(wxCommandEvent& event);
        void onEditRedo(wxCommandEvent& event);

        void onEditGround(wxCommandEvent& event);

        void onProcessCheck(wxCommandEvent& event);

        void onScriptToggle(wxCommandEvent& event);

        bool saveBeforeProceed();

        void saveProject();
        void loadProject(const std::string& filepath);

    private:
        wxDECLARE_EVENT_TABLE();
    };
}

#include "ActionManager.inl"



namespace ungod
{
    //define how to (de)serialize the filemanager
    template <>
    struct SerialIdentifier<uedit::EditorFrame>
    {
        static std::string get()  { return "EditorFrame"; }
    };

    template <>
    struct SerialBehavior<uedit::EditorFrame>
    {
        static void serialize(const uedit::EditorFrame& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<uedit::EditorFrame>
    {
        static void deserialize(uedit::EditorFrame& data, MetaNode deserializer, DeserializationContext& context);
    };


	template <>
	struct SerialIdentifier<uedit::MetaInfo>
	{
		static std::string get() { return "MetaInfo"; }
	};

	template <>
	struct SerialBehavior<uedit::MetaInfo>
	{
		static void serialize(const uedit::MetaInfo& data, MetaNode serializer, SerializationContext& context);
	};

	template <>
	struct DeserialBehavior<uedit::MetaInfo>
	{
		static void deserialize(uedit::MetaInfo& data, MetaNode deserializer, DeserializationContext& context);
	};
}

#endif // EDITOR_FRAME_H
