#include "ScriptManager.h"
#include "EditorFrame.h"

namespace uedit
{
    wxDEFINE_EVENT(EVT_SCRIPT_LOADED, wxCommandEvent);

    CodeEditor::CodeEditor(std::size_t index, const boost::filesystem::path& path, wxWindow* parent, wxWindowID id)
     : wxStyledTextCtrl(parent, id), mUnsaved(false), mIndex(index), mPath(path)
    {}

    enum ScriptManagerIDs
    {
        SCRIPT_EDITOR,
        LOADED_SCRIPTS,
        SCRIPT_TABS,
        SCRIPT_RUN = 999
    };

    ScriptManager::ScriptManager(const std::string& baseFilepath, ungod::Application& app,WorldActionWrapper& waw, wxWindow* parent, wxWindowID id, const wxPoint& pos) :
                                      wxFrame(parent, id, "Script Manager", pos),
                                          mWorldAction(waw), mApp(app), mEditorTabs(nullptr), mLoadedScripts(nullptr), mBasePath(baseFilepath)
    {
        wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

        {
            mLoadedScripts = new FileManager("lua", baseFilepath, this, LOADED_SCRIPTS);
            mLoadedScripts->addMenuItem(SCRIPT_RUN, "run scripts");
            Bind(EVT_FILE_LOAD, &ScriptManager::onScriptLoad, this, LOADED_SCRIPTS);
            Bind(EVT_FILE_SAVE, &ScriptManager::onScriptSave, this, LOADED_SCRIPTS);
            Bind(EVT_FILE_NEW, &ScriptManager::onScriptNew, this, LOADED_SCRIPTS);
            Bind(EVT_MENU_MOD, &ScriptManager::onMenuMod, this, LOADED_SCRIPTS);

            hbox->Add(mLoadedScripts, 1, wxEXPAND);
        }

        {
            mEditorTabs = new wxNotebook(this, SCRIPT_TABS);

            hbox->Add(mEditorTabs, 4, wxEXPAND);
        }

        SetSize(wxSize(800, 600));
        SetSizer(hbox);
        Fit();
    }


    void ScriptManager::onCharAdded(wxStyledTextEvent& event)
    {
        //int curPos = codeeditor->GetCurrentPos();
        //int wordStartPos = codeeditor->WordStartPosition(curPos, true);
    }


    void ScriptManager::onFileChanged(wxStyledTextEvent& event)
    {
        CodeEditor* editor = static_cast<CodeEditor*>(event.GetEventObject());

        if (!editor->isUnsaved())
        {
            editor->setUnsaved(true);
            boost::filesystem::path path = editor->getPath();
            std::string key = boost::filesystem::relative(path, mBasePath).string();
            mEditorTabs->SetPageText(editor->getIndex(), key.append(" *"));

            mLoadedScripts->markAsUnsaved(path.string());
        }
    }


    void ScriptManager::onScriptLoad(wxCommandEvent& event)
    {
        if (!mWorldAction.getEditorFrame()->getSelectedWorld())
            return;

        std::string file = std::string{event.GetString().mb_str()};
        //load the script
        ungod::ScriptErrorCode err = mWorldAction.getEditorFrame()->getCanvas()->getEditorState()->getEntityBehaviorManager().loadBehaviorScript(file);
        evaluateScriptExecution(err, file);

        //display the text
        openTextCtrl({event.GetString().mb_str()});

        //emit event
        wxCommandEvent fwdevent(EVT_SCRIPT_LOADED, GetId());
        fwdevent.SetEventObject(this);
        fwdevent.SetString(event.GetString());
        ProcessWindowEvent(fwdevent);
    }


    void ScriptManager::onScriptSave(wxCommandEvent& event)
    {
        for (const auto& editor : mEditors)
        {
            if (editor->isUnsaved())
            {
                //set the flag
                editor->setUnsaved(false);

                //remove the stars
                boost::filesystem::path path = editor->getPath();
                std::string key = boost::filesystem::relative(path, mBasePath).string();
                mEditorTabs->SetPageText(editor->getIndex(), key);

                mLoadedScripts->markAsSaved(path.string());

                //actually save the file
                std::ofstream script;
                script.open (path.string());
                script << editor->GetText();
                script.close();
            }
        }
    }


    void ScriptManager::onScriptNew(wxCommandEvent& event)
    {
        //display the text
        openTextCtrl({event.GetString().mb_str()});
    }


    void ScriptManager::onMenuMod(wxCommandEvent& event)
    {
        if (!mWorldAction.getEditorFrame()->getSelectedWorld())
            return;
        switch (event.GetInt())
        {
            case SCRIPT_RUN:
            {
                onScriptSave(event);
                std::vector<std::pair<std::string, ungod::ScriptErrorCode>> err = 
                        mWorldAction.getEditorFrame()->getCanvas()->getEditorState()->getEntityBehaviorManager().reload(mApp.getScriptState(), mApp.getGlobalScriptEnv());
                for (const auto& e : err)
                {
                    evaluateScriptExecution(e.second, e.first);
                }
                break;
            }
            default:
                break;
        }
    }


    void ScriptManager::openTextCtrl(const std::string& file)
    {
        boost::filesystem::path path(file);

        CodeEditor* codeeditor = new CodeEditor(mEditorTabs->GetPageCount(), path.string(), mEditorTabs, SCRIPT_EDITOR);

        mEditors.emplace_back(codeeditor);

        path = boost::filesystem::relative(path, mBasePath);
        mEditorTabs->AddPage(codeeditor, path.string());

        wxFont font (16, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

        codeeditor->StyleSetFont(wxSTC_STYLE_DEFAULT, font);
        codeeditor->SetMarginWidth (MARGIN_LINE_NUMBERS, 50);
        codeeditor->StyleSetForeground (wxSTC_STYLE_LINENUMBER, wxColour (75, 75, 75) );
        codeeditor->StyleSetBackground (wxSTC_STYLE_LINENUMBER, wxColour (220, 220, 220));
        codeeditor->SetMarginType (MARGIN_LINE_NUMBERS, wxSTC_MARGIN_NUMBER);

        codeeditor->StyleClearAll();
        codeeditor->SetLexer(wxSTC_LEX_LUA);
        codeeditor->StyleSetForeground(wxSTC_LUA_DEFAULT,	   wxColour(0,0,0));
        codeeditor->StyleSetForeground(wxSTC_LUA_COMMENT,	   wxColour(128,128,128));
        codeeditor->StyleSetItalic(wxSTC_LUA_COMMENT,		   true);
        codeeditor->StyleSetForeground(wxSTC_LUA_COMMENTLINE,   wxColour(128,128,128));
        codeeditor->StyleSetItalic(wxSTC_LUA_COMMENTLINE,	   true);
        codeeditor->StyleSetForeground(wxSTC_LUA_COMMENTDOC,	wxColour(128,128,128));
        codeeditor->StyleSetItalic(wxSTC_LUA_COMMENTDOC,		true);
        codeeditor->StyleSetForeground(wxSTC_LUA_NUMBER,		wxColour(0,196,0));
        codeeditor->StyleSetForeground(wxSTC_LUA_STRING,		wxColour(225,0,0));
        codeeditor->StyleSetForeground(wxSTC_LUA_CHARACTER,	 wxColour(225,128,0));
        codeeditor->StyleSetForeground(wxSTC_LUA_LITERALSTRING, wxColour(196,0,0));
        codeeditor->StyleSetForeground(wxSTC_LUA_PREPROCESSOR,  wxColour(64,64,64));
        codeeditor->StyleSetForeground(wxSTC_LUA_OPERATOR,	  wxColour(128,0,196));
        codeeditor->StyleSetBold(wxSTC_LUA_OPERATOR, true);
        codeeditor->StyleSetForeground(wxSTC_LUA_IDENTIFIER,	wxColour(0,0,0));
        codeeditor->StyleSetForeground(wxSTC_LUA_STRINGEOL,	 wxColour(225,0,0));
        codeeditor->StyleSetForeground (wxSTC_LUA_WORD,		 wxColour(0,0,255));
        codeeditor->StyleSetBold(wxSTC_LUA_WORD, true);
        codeeditor->StyleSetForeground (wxSTC_LUA_WORD2,		wxColour(0,0,255));
        codeeditor->StyleSetForeground (wxSTC_LUA_WORD3,		wxColour(0,128,255));
        codeeditor->StyleSetForeground (wxSTC_LUA_WORD4,		wxColour(196,196,0));
        // Key words
        codeeditor->SetKeyWords(0, wxT("function for while repeat until if else elseif end break return in do then"));
        codeeditor->SetKeyWords(1, wxT("local"));
        codeeditor->SetKeyWords(2, wxT("and or not"));
        codeeditor->SetKeyWords(3, wxT("nil true false"));
        // Brace matching
        codeeditor->StyleSetForeground(wxSTC_STYLE_BRACELIGHT, wxColour(255,255,255));
        codeeditor->StyleSetBackground(wxSTC_STYLE_BRACELIGHT, wxColour(128,0,196));
        codeeditor->StyleSetForeground(wxSTC_STYLE_BRACEBAD, wxColour(255,255,255));
        codeeditor->StyleSetBackground(wxSTC_STYLE_BRACEBAD, wxColour(255,0,0));
        // Margin (folding)
        codeeditor->SetMarginType(MARGIN_FOLD, wxSTC_MARGIN_SYMBOL);
        codeeditor->SetMarginWidth(MARGIN_FOLD, 15);
        codeeditor->SetMarginMask(MARGIN_FOLD, wxSTC_MASK_FOLDERS);
        codeeditor->SetMarginSensitive(MARGIN_FOLD, true);
        // Folding
        codeeditor->SetProperty(wxT("fold"),		 wxT("1") );
        codeeditor->SetProperty(wxT("fold.comment"), wxT("1") );
        codeeditor->SetProperty(wxT("fold.compact"), wxT("1") );
        codeeditor->SetProperty(wxT("fold.preprocessor"), wxT("1") );
        codeeditor->MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_PLUS, wxColor(225,0,0), wxColor(0,0,0));
        codeeditor->MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_MINUS, wxColor(0,0,225), wxColor(0,0,0));
        codeeditor->MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY);
        codeeditor->MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_EMPTY);
        codeeditor->MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_EMPTY);
        codeeditor->MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY);
        codeeditor->MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY);

        Bind(wxEVT_STC_CHARADDED, &ScriptManager::onCharAdded, this, SCRIPT_EDITOR);
        Bind(wxEVT_STC_CHANGE, &ScriptManager::onFileChanged, this, SCRIPT_EDITOR);

        //display the contents of the script file
        std::ifstream script;
        script.open (file);
        std::stringstream buffer;
        buffer << script.rdbuf();
        codeeditor->SetText(buffer.str());
    }

    void ScriptManager::evaluateScriptExecution(ungod::ScriptErrorCode err, const std::string& file)
    {
        switch (err)
        {
            case ungod::SCRIPT_OK:
            case ungod::SCRIPT_ALREADY_LOADED:
            {
                mLoadedScripts->markAsOk(file);
                break;
            }
            case ungod::SCRIPT_NOT_FOUND:
            case ungod::SCRIPT_LOAD_ERROR:
            case ungod::SCRIPT_EXECUTION_ERROR:
            {
                mLoadedScripts->markAsError(file);
                break;
            }
            default: break;
        }
    }
}

namespace ungod
{
    //serialization code
    void SerialBehavior<uedit::ScriptManager>::serialize(const uedit::ScriptManager& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeObject("filemanager", *data.mLoadedScripts, serializer);
    }

    void DeserialBehavior<uedit::ScriptManager>::deserialize(uedit::ScriptManager& data, MetaNode deserializer, DeserializationContext& context)
    {
        context.first(context.deserializeObject(*data.mLoadedScripts), "filemanager", deserializer);
    }
}
