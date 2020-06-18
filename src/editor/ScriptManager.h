#ifndef UEDIT_SCRIPT_MANAGER_H
#define UEDIT_SCRIPT_MANAGER_H

#include "ungod/serialization/Serializable.h"
#include "ActionManager.h"
#include "FileManager.h"
#include "wx/frame.h"
#include "wx/stc/stc.h"
#include "wx/sizer.h"
#include <wx/menu.h>
#include <iostream>
#include <fstream>
#include <wx/notebook.h>
#include <wx/font.h>
#include <boost/filesystem.hpp>

namespace uedit
{
    wxDECLARE_EVENT(EVT_SCRIPT_LOADED, wxCommandEvent);

    enum
    {
        MARGIN_LINE_NUMBERS,
        MARGIN_FOLD
    };


    class CodeEditor : public wxStyledTextCtrl
    {
    public:
        CodeEditor(std::size_t index, const boost::filesystem::path& path, wxWindow* parent = nullptr, wxWindowID id = -1);

        bool isUnsaved() const { return mUnsaved; }

        std::size_t getIndex() const { return mIndex; }

        const boost::filesystem::path& getPath() const { return mPath; }

        void setUnsaved(bool unsaved) { mUnsaved = unsaved; }

    private:
        bool mUnsaved;
        std::size_t mIndex;
        boost::filesystem::path mPath;
    };


    class ScriptManager : public wxFrame, public ungod::Serializable<ScriptManager>
    {
    friend struct ungod::SerialBehavior<ScriptManager>;
    friend struct ungod::DeserialBehavior<ScriptManager>;
    public:
        ScriptManager(const std::string& baseFilepath,
                      ungod::Application& app,
                      ActionManager& actionManager,
                      wxWindow* parent = nullptr,
                      wxWindowID id = -1,
                      const wxPoint& pos = wxDefaultPosition);


    const std::vector<CodeEditor*>& getEditors() const { return mEditors; }

    private:
        ActionManager& mActionManager;
        ungod::Application& mApp;
        wxNotebook* mEditorTabs;
        FileManager* mLoadedScripts;
        boost::filesystem::path mBasePath;
        std::vector<CodeEditor*> mEditors;

    private:
        void onCharAdded(wxStyledTextEvent& event);
        void onFileChanged(wxStyledTextEvent& event);
        void onScriptLoad(wxCommandEvent& event);
        void onScriptSave(wxCommandEvent& event);
        void onScriptNew(wxCommandEvent& event);
        void onMenuMod(wxCommandEvent& event);

        void openTextCtrl(const std::string& file);
        void evaluateScriptExecution(ungod::ScriptErrorCode err, const std::string& file);  //sets color markers for OK scripts and ERROR scripts
    };
}



namespace ungod
{
    //define how to (de)serialize the filemanager
    template <>
    struct SerialIdentifier<uedit::ScriptManager>
    {
        static std::string get()  { return "ScriptManager"; }
    };

    template <>
    struct SerialBehavior<uedit::ScriptManager>
    {
        static void serialize(const uedit::ScriptManager& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<uedit::ScriptManager>
    {
        static void deserialize(uedit::ScriptManager& data, MetaNode deserializer, DeserializationContext& context);
    };
}

#endif // UEDIT_SCRIPT_MANAGER_H
