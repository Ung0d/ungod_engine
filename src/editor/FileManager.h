#ifndef UEDIT_FILE_MANAGER_H
#define UEDIT_FILE_MANAGER_H

#include "ungod/serialization/Serializable.h"
#include "Utility.h"
#include <wx/treectrl.h>
#include <wx/menu.h>
#include <boost/filesystem.hpp>
#include <unordered_map>

namespace uedit
{
    wxDECLARE_EVENT(EVT_FILE_LOAD, wxCommandEvent);
    wxDECLARE_EVENT(EVT_FILE_SAVE, wxCommandEvent);
    wxDECLARE_EVENT(EVT_FILE_NEW, wxCommandEvent);
    wxDECLARE_EVENT(EVT_MENU_MOD, wxCommandEvent);

    enum
    {
        POP_UP_LOAD,
        POP_UP_NEW,
        POP_UP_SAVE
    };

    class FileManager : public wxTreeCtrl, public ungod::Serializable<FileManager>
    {
    friend struct ungod::SerialBehavior<FileManager>;
    friend struct ungod::DeserialBehavior<FileManager>;
    public:
        FileManager(const std::string& filetype, const std::string& baseFilepath, wxWindow* parent = nullptr, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition);

        void addPath(const std::string& file);

        void markAsUnsaved(const std::string& file);
        void markAsSaved(const std::string& file);

        void markAsError(const std::string& file);
        void markAsOk(const std::string& file);

        std::vector<std::string> getFilepaths() const;

        void addMenuItem(int id, const std::string& itemtxt);

    private:
        std::string mFiletype;
        boost::filesystem::path mBasePath;
        wxTreeItemId mRoot;
        std::unordered_map<std::string, wxTreeItemId> mLoadedFiles;
        std::vector< std::pair<int, std::string> > mMenuMods;

    private:
        void onRightDown(wxMouseEvent& event);
        void onRightUp(wxMouseEvent& event);
        void onPopupClick(wxCommandEvent &evt);

        wxTreeItemId accomodateItem(const wxTreeItemId& id, const std::string& item);
    };
}

namespace ungod
{
    //define how to (de)serialize the filemanager
    template <>
    struct SerialIdentifier<uedit::FileManager>
    {
        static std::string get()  { return "FileManager"; }
    };

    template <>
    struct SerialBehavior<uedit::FileManager>
    {
        static void serialize(const uedit::FileManager& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<uedit::FileManager>
    {
        static void deserialize(uedit::FileManager& data, MetaNode deserializer, DeserializationContext& context);
    };
}

#endif // UEDIT_FILE_MANAGER_H
