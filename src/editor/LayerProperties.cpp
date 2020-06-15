#include "LayerProperties.h"
#include "StatDisplayer.h"

namespace uedit
{
    LayerPropertiesWindow::LayerPropertiesWindow(ungod::World* world,
                                                  wxWindow * parent, wxWindowID id,
                                                    const wxPoint & pos) :
        wxFrame(parent, id, _("layer properties"), pos), mWorld(world)
    {
        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        {
            wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
            hbox->Add(new wxStaticText(this, -1, "ambient light color: "), 1, wxCENTER);
            wxColourPickerCtrl* colpick = new wxColourPickerCtrl(this, -1);
            colpick->Bind(wxEVT_COLOURPICKER_CHANGED, &LayerPropertiesWindow::onAmbientColourPick, this);
            hbox->Add(colpick, 1, wxCENTER);
            vbox->Add(hbox, 1, wxCENTER);
        }

        {
            wxCheckBox* lightRender = new wxCheckBox(this, -1, _("render light"));
            lightRender->SetValue(world->isLightToggled());
            lightRender->Bind(wxEVT_CHECKBOX, &LayerPropertiesWindow::onLightCheck, this);
            vbox->Add(lightRender, 1, wxCENTER);
        }

        {
            StatDisplay<float>* depthDisp = new StatDisplay<float>("depth:", this, -1);
            depthDisp->connectSetter([this](float depth)
                {
                    mWorld->setRenderDepth(depth);
                });
            depthDisp->connectGetter([this]()
                {
                    return mWorld->getRenderDepth();
                });
            vbox->Add(depthDisp, 1, wxCENTER);
        }

        SetSizer(vbox);
        Fit();
        Centre();
    }

    void LayerPropertiesWindow::onAmbientColourPick(wxColourPickerEvent& event)
    {
        mWorld->getLightHandler().setAmbientColor( convertColor(event.GetColour()) );
    }

    void LayerPropertiesWindow::onLightCheck(wxCommandEvent& event)
    {
        mWorld->toggleLight(event.IsChecked());
    }
}
