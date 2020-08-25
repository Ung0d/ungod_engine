#include "FileManager.h"
#include "wx/filedlg.h"

namespace uedit
{
    wxDEFINE_EVENT(EVT_FILE_LOAD, wxCommandEvent);
    wxDEFINE_EVENT(EVT_FILE_SAVE, wxCommandEvent);
    wxDEFINE_EVENT(EVT_FILE_NEW, wxCommandEvent);
    wxDEFINE_EVENT(EVT_MENU_MOD, wxCommandEvent);

    FileManager::FileManager(const std::string& filetype, const std::string& baseFilepath, wxWindow* parent, wxWindowID id, const wxPoint& pos)
        : wxTreeCtrl(parent, id, pos), mFiletype(filetype), mBasePath(baseFilepath)
    {
        mRoot = AddRoot(mBasePath.filename().string());

        Bind(wxEVT_RIGHT_DOWN, &FileManager::onRightDown, this);
    }


    void FileManager::addPath(const std::string& file)
    {
        auto res = mLoadedFiles.find(file);

        if (res == mLoadedFiles.end())  //file not loaded yet
        {
            boost::filesystem::path path(file);

            std::vector<std::string> dirs;

            std::string base = file;
            while (path.parent_path().string() != "")
            {
                base = path.filename().string();
                path = path.parent_path();

                dirs.emplace_back(base);

                if (base == mBasePath.filename().string())
                    break;
            }

            wxTreeItemId curid = mRoot;
            for (auto i = dirs.rbegin()+1; i != dirs.rend(); ++i)
            {
                curid = accomodateItem(curid, *i);
            }

        //note down that the item was added
            mLoadedFiles.emplace(file, curid);
        }
    }


    void FileManager::markAsUnsaved(const std::string& file)
    {
        auto res = mLoadedFiles.find(file);
        if (res != mLoadedFiles.end())
        {
            SetItemText(res->second, GetItemText(res->second).Append('*'));
        }
    }


    void FileManager::markAsSaved(const std::string& file)
    {
        auto res = mLoadedFiles.find(file);
        if (res != mLoadedFiles.end())
        {
            boost::filesystem::path path(file);
            SetItemText(res->second, path.filename().string());
        }
    }

    void FileManager::markAsError(const std::string& file)
    {
        auto res = mLoadedFiles.find(file);
        if (res != mLoadedFiles.end())
        {
            SetItemBackgroundColour(res->second, wxColor(249, 84, 54));
        }
    }


    void FileManager::markAsOk(const std::string& file)
    {
        auto res = mLoadedFiles.find(file);
        if (res != mLoadedFiles.end())
        {
            SetItemBackgroundColour(res->second, wxColor(160, 249, 142));
        }
    }

    std::vector<std::string> FileManager::getFilepaths() const
    {
        std::vector<std::string> files;
        for (const auto& file : mLoadedFiles)
            files.emplace_back(file.first);
        return files;
    }


    void FileManager::addMenuItem(int id, const std::string& itemtxt)
    {
        mMenuMods.emplace_back(id, itemtxt);
    }


    void FileManager::onRightDown(wxMouseEvent& event)
    {
        wxMenu mnu;
        mnu.Append(POP_UP_NEW, 	"new");
        mnu.Append(POP_UP_LOAD, "load");
        mnu.Append(POP_UP_SAVE, "save");
        for (const auto& mod : mMenuMods)
        {
            mnu.Append(mod.first, mod.second);
        }
        mnu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FileManager::onPopupClick), NULL, this);
        PopupMenu(&mnu);
    }


    void FileManager::onPopupClick(wxCommandEvent &evt)
    {
        switch(evt.GetId())
        {
            case POP_UP_NEW:
            {
                wxFileDialog openFileDialog(this, fileDialogHeader(mFiletype), "", "", fileDialogWildcard(mFiletype), wxFD_SAVE);
                if (openFileDialog.ShowModal() == wxID_OK)
                {
                    std::string path = std::string(openFileDialog.GetPath().mb_str());

                    //create empty file
                    std::ofstream script;
                    script.open(path);
                    script.close();

                    //not down file
                    addPath(path);

                    //emit event
                    wxCommandEvent event(EVT_FILE_NEW, GetId());
                    event.SetEventObject(this);
                    event.SetString(openFileDialog.GetPath());
                    ProcessWindowEvent(event);
                }
                break;
            }
            case POP_UP_LOAD:
            {
                wxFileDialog openFileDialog(this, fileDialogHeader(mFiletype), "", "", fileDialogWildcard(mFiletype), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
                if (openFileDialog.ShowModal() == wxID_OK)
                {
                    //not down file
                    addPath(std::string(openFileDialog.GetPath().mb_str()));

                    //emit event
                    wxCommandEvent event(EVT_FILE_LOAD, GetId());
                    event.SetEventObject(this);
                    event.SetString(openFileDialog.GetPath());
                    ProcessWindowEvent(event);
                }
                break;
            }
            case POP_UP_SAVE:
            {
                //emit event
                wxCommandEvent event(EVT_FILE_SAVE, GetId());
                event.SetEventObject(this);
                ProcessWindowEvent(event);
                break;
            }
            default:
            {
                //emit event
                wxCommandEvent event(EVT_MENU_MOD, GetId());
                event.SetEventObject(this);
                event.SetInt(evt.GetId());
                ProcessWindowEvent(event);
                break;
            }
        }
    }


    wxTreeItemId FileManager::accomodateItem(const wxTreeItemId& id, const std::string& item)
    {
        wxTreeItemIdValue cookie;
        for (wxTreeItemId i = GetFirstChild(id,cookie); i.IsOk(); i = GetNextChild(i,cookie))
        {
            if (GetItemText(i) == item)
                return i;
        }
        return AppendItem(id, item);
    }
}

namespace ungod
{
    //serialization code
    void SerialBehavior<uedit::FileManager>::serialize(const uedit::FileManager& data, MetaNode serializer, SerializationContext& context)
    {
        for (const auto& e : data.mLoadedFiles)
        {
            MetaNode sub = context.appendSubnode(serializer, "file");
            context.serializeProperty("path", e.first, sub);
        }
    }

    void DeserialBehavior<uedit::FileManager>::deserialize(uedit::FileManager& data, MetaNode deserializer, DeserializationContext& context)
    {
        forEachSubnode(deserializer, [&data] (MetaNode sub)
        {
            auto path = sub.getAttribute<std::string>("path", "");
            if (path != "")
            {
                data.addPath(path);
            }
        }, "file");
    }
}
