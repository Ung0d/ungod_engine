#ifndef UEDIT_LAYER_DISPLAY_H
#define UEDIT_LAYER_DISPLAY_H

#include "ungod/serialization/Serializable.h"
#include "Canvas.h"
#include "LayerProperties.h"
#include "wx/frame.h"
#include "wx/rearrangectrl.h"
#include "wx/listctrl.h"

namespace uedit
{
    class EditorCanvas;

    class LayerDisplay : public wxWindow, public ungod::Serializable<LayerDisplay>
    {
    friend struct ungod::SerialBehavior<LayerDisplay>;
    friend struct ungod::DeserialBehavior<LayerDisplay>;
    public :
        LayerDisplay(EditorCanvas* canvas,
                     EditorFrame* eframe,
                     wxWindow* parent = nullptr,
                      wxWindowID id = -1,
                      const wxPoint& cPosition = wxDefaultPosition,
                      const wxSize& cSize = wxDefaultSize,
                      long cStyle = 0);

        void setup();

        int getSelection() const;

        ungod::World* getSelectedWorld() const;  //nullptr if no world selected

    private:
        EditorCanvas* mCanvas;
        EditorFrame* mEditorFrame;
        wxRearrangeList* mLayerlist;
        LayerPropertiesWindow* mLpw;
        wxListCtrl* mEntityList;
        owls::SignalLink<void, ungod::Entity> mEnCreationLink;
        owls::SignalLink<void, ungod::Entity> mEnDestructionLink;
        std::unordered_map<std::size_t, std::size_t> mIDMapping;
        std::vector<ungod::Entity> mEntities;

        static constexpr unsigned INSPECT = 0;
        static constexpr unsigned SHOW = 1;
        static constexpr unsigned RENAME = 2;

    private:
        void onMoveDown(wxCommandEvent & event);
        void onMoveUp(wxCommandEvent & event);
        void toggleSelection(wxCommandEvent & event);
        void onEntityCreate(wxCommandEvent& event);
        void onEntityEdit(wxCommandEvent& event);
        void onEntityDeselect(wxCommandEvent& event);
        void onLayerPropterties(wxCommandEvent& event);
        void onPropertiesClose(wxCloseEvent& event);
        void onLayerSelect(wxCommandEvent& event);
        void listEntity(ungod::Entity e);
        void onEntityRightClick(wxListEvent& event);
        void onEntityMenuSelect(wxCommandEvent& event);
        void onEntityListSelect(wxListEvent& event);
    };
}


namespace ungod
{
    //define how to (de)serialize the filemanager
    template <>
    struct SerialIdentifier<uedit::LayerDisplay>
    {
        static std::string get()  { return "LayerDisplay"; }
    };

    template <>
    struct SerialBehavior<uedit::LayerDisplay>
    {
        static void serialize(const uedit::LayerDisplay& data, MetaNode serializer, SerializationContext& context);
    };

    template <>
    struct DeserialBehavior<uedit::LayerDisplay>
    {
        static void deserialize(uedit::LayerDisplay& data, MetaNode deserializer, DeserializationContext& context);
    };
}

#endif // UEDIT_LAYER_DISPLAY_H

