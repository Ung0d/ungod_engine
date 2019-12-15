#ifndef SHEET_PREVIEW_H
#define SHEET_PREVIEW_H

#include "ungod/serialization/Serializable.h"
#include "RenderArea.h"
#include "ungod/visual/Visual.h"
#include "ungod/serialization/MetaData.h"
#include "wx/frame.h"
#include <wx/listbox.h>

namespace uedit
{
    /** \brief Defines an area to preview contents of a spritesheet. */
    class SheetPreviewArea : public RenderArea
    {
    public :
        SheetPreviewArea(wxWindow* parent = nullptr,
                      wxWindowID id = -1,
                      const wxPoint& cPosition = wxDefaultPosition,
                      const wxSize& cSize = wxDefaultSize,
                      long cStyle = 0);

        /** \brief Previews the specified content. */
        void preview(ungod::Image& image, ungod::MetaMap& meta, const std::string& key);

        virtual void update(float delta) override;
        virtual void render(sf::RenderWindow& window, sf::RenderStates states) override;

        virtual ~SheetPreviewArea() { }

    private:
        ungod::Sprite mRenderbody;
        ungod::Image* mSelectedImage;
        ungod::MetaMap* mSelectedMeta;
        std::string mKey;
        bool mShowAnimation;
        ungod::Animation mAnimation;
        sf::FloatRect mContentBounds;
    };

    class SheetPreview : public wxWindow, public ungod::Serializable<SheetPreview>
    {
    friend struct ungod::SerialBehavior<SheetPreview>;
    friend struct ungod::DeserialBehavior<SheetPreview>;
    public:
        SheetPreview(wxWindow * parent, wxWindowID id,
                      const wxPoint & pos = wxDefaultPosition );

        /** \brief Returns the currently active sheet key. */
        std::string getCurrentKey();

        /** \brief Returns the type of the currently action sheet object. */
        std::string getCurrentType();

        /** \brief Returns the currently selected sheet ID. */
        std::string getCurrentTexture() const;

        /** \brief Returns the currently selected meta ID. */
        std::string getCurrentMeta() const;

        /** \brief Selects the given sheet and key if found. */
        void selectSheetKey(const std::string& sheetID, const std::string& key);

        void updateSheetListbox();
        void updateKeysListbox(std::size_t sheetID);

        void onSheetSelect(wxCommandEvent& event);
        void onKeySelect(wxCommandEvent& event);

        bool loadSheet(const std::string& sheetID, const std::string& metaID);
        void reloadSheets();

        void onNew(wxCommandEvent& event);
        void onReload(wxCommandEvent& event);
        void onRemove(wxCommandEvent& event);

    private:
        wxListBox* mLoadedSheets;
        wxListBox* mSheetKeys;
        SheetPreviewArea* mPreviewArea;
        std::vector<ungod::Image> mImages;
        std::vector<ungod::MetaMap> mMetas;

    public:
        wxDECLARE_EVENT_TABLE();
    };
}


namespace ungod
{
    //define how to (de)serialize the filemanager
    template <>
    struct SerialIdentifier<uedit::SheetPreview>
    {
        static std::string get()  { return "SheetPreview"; }
    };

    template <>
    struct SerialBehavior<uedit::SheetPreview>
    {
        static void serialize(const uedit::SheetPreview& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<uedit::SheetPreview>
    {
        static void deserialize(uedit::SheetPreview& data, MetaNode deserializer, DeserializationContext& context);
    };
}

#endif // SHEET_PREVIEW_H
