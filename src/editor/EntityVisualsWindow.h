#ifndef ENTITY_VISUALS_PREVIEW_H
#define ENTITY_VISUALS_PREVIEW_H

#include "ActionManager.h"
#include "StatDisplayer.h"
#include "wx/stattext.h"
#include "Utility.h"
#include "wx/filepicker.h"
#include "wx/stattext.h"
#include "wx/textctrl.h"
#include "wx/choice.h"
#include "wx/listbox.h"
#include "wx/notebook.h"
#include "wx/clrpicker.h"
#include "wx/checkbox.h"

namespace uedit
{
    enum VisualsWindowIDs
    {
        FILEPICKER,
        TEX_COPY_BUTTON,
        TYPE_NOTEBOOK,
        SPRITE_RECT_BY_KEY,
        MULTI_SPRITE_RECT_BY_KEY,
        SET_MULTI_SPRITE_COMPONENT_COUNT,
        CHOICE_MULTISPRITE,
        SPRITE_CTRL_POS_X,
        SPRITE_CTRL_POS_Y,
        SPRITE_CTRL_SCALE_X,
        SPRITE_CTRL_SCALE_Y,
        SPRITE_CTRL_ROTATION,
        SET_VERTEX_RECT_COUNT,
        CHOICE_VERTEX,
        VERTEX_RECT_BY_KEY,
        VERTEX_COLOR_PICKER,
        SPRITE_COLOR_PICKER
    };

    class EntityVisualsWindow : public wxWindow
    {
    public:
        EntityVisualsWindow(ungod::Entity e, ActionManager& actionManager, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz);

        void onEntityContentsChanged(ungod::Entity e);

    private:
        wxFilePickerCtrl* mSheetPicker;
        ungod::Entity mEntity;
        ActionManager& mActionManager;
        wxNotebook* mTypeNotebook;
        wxCheckBox* mHideForCameraChecked;
        wxStaticText* mTexRectLabel;
        wxStaticText* mMultiTexRectLabel;
        wxTextCtrl* mCompCountCtrl;
        wxChoice* mMultiSpriteChoice;
        wxPanel* mSpritePanel;
        wxPanel* mMultiSpritePanel;
        StatDisplay<float>* mSpritePositionX;
        StatDisplay<float>* mSpritePositionY;
        StatDisplay<float>* mSpriteScaleX;
        StatDisplay<float>* mSpriteScaleY;
        StatDisplay<float>* mSpriteRotation;
        wxPanel* mMultiSpriteParent;
        wxBoxSizer* mMultiSpriteSizer;
        StatDisplay<float>* mMultiSpritePositionX;
        StatDisplay<float>* mMultiSpritePositionY;
        StatDisplay<float>* mMultiSpriteScaleX;
        StatDisplay<float>* mMultiSpriteScaleY;
        StatDisplay<float>* mMultiSpriteRotation;
        std::size_t mCurMultiSelect;

        wxPanel* mVertexPanel;
        wxChoice* mVertexChoice;
        wxStaticText* mVertexTexRectLabel;
        StatDisplay<float>* mVertexPositionX;
        StatDisplay<float>* mVertexPositionY;
        int mSelectedVertex;
        wxColourPickerCtrl* mVertexColor;
        wxColourPickerCtrl* mSpriteColor;
        wxCheckBox* mVertexFlipX;
        wxCheckBox* mVertexFlipY;

    private:
        void onTexSelect(wxFileDirPickerEvent& event);
        void onTexCopyButton(wxCommandEvent & event);
        void onRectByMetaKey(wxCommandEvent & event);
        void onHideForCameraChecked(wxCommandEvent & event);

        void onMultiRectByMetaKey(wxCommandEvent & event);
        void onMultiSpriteCompCountChange(wxCommandEvent & event);
        void onMultiSpriteSelect(wxCommandEvent & event);

        void onVertexRectCountChange(wxCommandEvent & event);
        void onVectexRectSelect(wxCommandEvent & event);
        void onVertexRectByKey(wxCommandEvent & event);
        void onVertexColorSelect(wxColourPickerEvent& event);

        void onSpriteColorSelect(wxColourPickerEvent& event);

        static void setTexRectLabel(wxStaticText* text, const sf::FloatRect& rect);

        void setUpMultiSpriteEdit();
        void buildSpriteEdit(ungod::SpriteComponent& sprite);
        void buildMultiSpriteEdit(ungod::SpriteComponent& sprite, std::size_t index);
        void buildVertexEdit();
        void updateSpriteData();
        void updateMultiSpriteData();

        void displayVertexRect(std::size_t index);

        wxDECLARE_EVENT_TABLE();
    };
}

#endif // ENTITY_VISUALS_PREVIEW_H
