#include "EntityTileMapWindow.h"
#include "EditorFrame.h"
#include "TileMapDialog.h"
#include "WaterDialog.h"
#include "EntityDesigner.h"
#include "wx/sizer.h"
#include "TilemapEditState.h"

namespace uedit
{
    EntityTileMapWindow::EntityTileMapWindow(EditorCanvas& canvas, ungod::Entity e, EntityDesigner* d, WorldActionWrapper& waw, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz) :
        wxWindow(parent, id, pos, siz), mEntity(e), mWorldAction(waw), mDesigner(d)
    {
        wxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        auto* initButton = new wxButton(this, -1, "init");
        initButton->Bind(wxEVT_BUTTON, &EntityTileMapWindow::onInitClicked, this);
        vbox->Add(initButton,1,wxCENTER);

        auto* fitButton = new wxButton(this, -1, "fit to world");
        fitButton->Bind(wxEVT_BUTTON, &EntityTileMapWindow::onFitClicked, this);
        vbox->Add(fitButton, 1, wxCENTER);

        auto* resetViewBut = new wxButton(this, -1, "reset view");
        resetViewBut->Bind(wxEVT_BUTTON, &EntityTileMapWindow::onViewReset, this);
        vbox->Add(resetViewBut,1,wxCENTER);

        auto* brushbut = new wxButton(this, -1, "brush");
        brushbut->Bind(wxEVT_BUTTON, [this] (wxCommandEvent & event) { mDesigner->getEntityPreview()->toggle<TileMapEditState>(false); });
        vbox->Add(brushbut,1,wxCENTER);

        auto* floodfillbut = new wxButton(this, -1, "flood fill");
        floodfillbut->Bind(wxEVT_BUTTON, [this] (wxCommandEvent & event) { mDesigner->getEntityPreview()->toggle<TileMapFloodFillState>(false); });
        vbox->Add(floodfillbut,1,wxCENTER);

        SetSizer(vbox);
        Fit();
        Layout();

        mDesigner->getEntityPreview()->lookAt(canvas.getCameraCenter());
    }

    void EntityTileMapWindow::onInitClicked(wxCommandEvent & event)
    {
        TileMapDialog dia(this, -1);
        if (dia.ShowModal() == wxID_OK)
        {
            mWorldAction.loadTiles(mEntity, std::string{dia.getSheetID().mbc_str()}, std::string{dia.getMetaID().mbc_str()},
                                   dia.getTileWidth(), dia.getTileHeight());
            mWorldAction.reserveTileCount(mEntity, dia.getMapWidth()*dia.getMapHeight(), dia.getMapWidth(), dia.getMapHeight());
        }
    }

    void EntityTileMapWindow::onFitClicked(wxCommandEvent& event)
    {
        mWorldAction.setEntityPosition(mEntity, {});
        const auto& tm = mEntity.get<ungod::TileMapComponent>().getTileMap();
        if (tm.getTileWidth() != 0 && tm.getTileHeight() != 0)
        {
            unsigned width = (unsigned)std::floor(mEntity.getWorld().getSize().x / tm.getTileWidth());
            unsigned height = (unsigned)std::floor(mEntity.getWorld().getSize().y / tm.getTileHeight());
            mWorldAction.reserveTileCount(mEntity, width * height, width, height);
        }
        else
        {
            wxMessageBox(wxT("Initialize a tile-width and a tile-height > 0."), wxT("Error"), wxICON_INFORMATION);
        }
    }

    void EntityTileMapWindow::onViewReset(wxCommandEvent & event)
    {
        mDesigner->getEntityPreview()->resetView();
    }


    EntityWaterWindow::EntityWaterWindow(EditorCanvas& canvas, ungod::Entity e, EntityDesigner* d, WorldActionWrapper& waw, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz) :
        wxWindow(parent, id, pos, siz), mEntity(e), mWorldAction(waw), mDesigner(d)
    {
        wxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        auto* initButton = new wxButton(this, -1, "init");
        initButton->Bind(wxEVT_BUTTON, &EntityWaterWindow::onInitClicked, this);
        vbox->Add(initButton,1,wxCENTER);

        auto* resetViewBut = new wxButton(this, -1, "reset view");
        resetViewBut->Bind(wxEVT_BUTTON, &EntityWaterWindow::onViewReset, this);
        vbox->Add(resetViewBut,1,wxCENTER);

        SetSizer(vbox);
        Fit();
        Layout();

        mDesigner->getEntityPreview()->lookAt(canvas.getCameraCenter());
    }

    void EntityWaterWindow::onInitClicked(wxCommandEvent & event)
    {
        WaterDialog dia(this, -1);
        if (dia.ShowModal() == wxID_OK)
        {
            mWorldAction.loadWaterTiles(mEntity, std::string{dia.getSheetID().mbc_str()}, std::string{dia.getMetaID().mbc_str()},
                                   dia.getTileWidth(), dia.getTileHeight());
            mWorldAction.reserveWaterTileCount(mEntity, dia.getMapWidth()*dia.getMapHeight(), dia.getMapWidth(), dia.getMapHeight());
            mWorldAction.loadWaterShaders(mEntity, std::string{dia.getDistortionMapID().mbc_str()},
                                                   std::string{dia.getFragmentShaderID().mbc_str()},
                                                   std::string{dia.getVertexShaderID().mbc_str()});
        }
    }

    void EntityWaterWindow::onViewReset(wxCommandEvent & event)
    {
        mDesigner->getEntityPreview()->resetView();
    }
}
