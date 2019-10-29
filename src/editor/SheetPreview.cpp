#include "SheetPreview.h"
#include "SheetDialog.h"
#include <boost/filesystem.hpp>
#include "wx/sizer.h"
#include "wx/button.h"
#include <wx/msgdlg.h>
namespace uedit
{
    SheetPreviewArea::SheetPreviewArea(wxWindow* parent,
                  wxWindowID id,
                  const wxPoint& cPosition,
                  const wxSize& cSize,
                  long cStyle) :
            RenderArea(parent, id, cPosition, cSize, cStyle),
                      mSelectedImage(nullptr),
                      mSelectedMeta(nullptr),
                      mShowAnimation(false)
    {
        mCamera.setViewSize(sf::Vector2f{(float)GetSize().x, (float)GetSize().y});
    }

    void SheetPreviewArea::preview(ungod::Image& image, ungod::MetaMap& meta, const std::string& key)
    {
        mSelectedImage = &image;
        mSelectedMeta = &meta;
        mKey = key;

        if (mSelectedMeta && mSelectedImage && mSelectedImage->isLoaded() && mSelectedMeta->isLoaded())
        {
            ungod::MetaNode node = mSelectedMeta->getNodeWithKey(mKey);
            if (node)
            {
                std::string type = node.getAttribute<std::string>("type", "");
                if (type == "object")
                {
                    auto result = node.getAttributes<int, int, int, int, float, float>(
                                {"pos_x", 0}, {"pos_y", 0}, {"width", 0}, {"height", 0}, {"offset_x", 0}, {"offset_y", 0} );
                    mRenderbody.setTextureRect(sf::FloatRect((float)std::get<0>(result), (float)std::get<1>(result), (float)std::get<2>(result), (float)std::get<3>(result)));
                    mRenderbody.setPosition({std::get<4>(result), std::get<5>(result)});
                    mContentBounds = sf::IntRect{mRenderbody.getTextureRect()};
                    Refresh(false);
                    mShowAnimation = false;
                }
                else if (type == "animation")
                {
                    mAnimation.startAnimation(node, mRenderbody.getVertices());
                    mContentBounds = mAnimation.getBounds();
                    mShowAnimation = true;
                }
                else
                {
                    mRenderbody = ungod::Sprite{};
                    mContentBounds = sf::IntRect{0,0,0,0};
                    mShowAnimation = false;
                }

                //center the view
                int renderms = std::max(mContentBounds.width, mContentBounds.height);
                int previewms = std::min(GetSize().x, GetSize().y);
                float scaling = 1.0f;
                if (renderms > previewms)
                {
                    scaling = (float)previewms/(float)renderms;
                }
                mRenderbody.setScale(scaling, scaling);
                mCamera.lookAt({ scaling*mContentBounds.width*0.5f, scaling*mContentBounds.height*0.5f });
            }
        }
    }

    void SheetPreviewArea::update(float delta)
    {
        //if animation is shown, update it
        if (mShowAnimation)
        {
            mAnimation.update(delta, mRenderbody.getVertices());
        }
    }

    void SheetPreviewArea::render(sf::RenderWindow& window, sf::RenderStates states)
    {
        if (!mSelectedImage || !mSelectedImage->isLoaded())
            return;

        //draw the renderboy centered, scale down if its size exceed the bounds of the preview area
        states.texture = mSelectedImage->get();
        mRenderbody.render(window, states);
    }


    enum SheetPrevKeys
    {
        SHEET_LISTBOX, KEY_LISTBOX, PREVIEW_AREA
    };

    wxBEGIN_EVENT_TABLE(SheetPreview, wxWindow)
        EVT_LISTBOX(SHEET_LISTBOX, SheetPreview::onSheetSelect)
        EVT_LISTBOX(KEY_LISTBOX, SheetPreview::onKeySelect)
    wxEND_EVENT_TABLE()

    SheetPreview::SheetPreview(wxWindow * parent, wxWindowID id,  const wxPoint & pos )
                  : wxWindow(parent, id, pos, wxSize(330, 600))
                  {
                      wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

                      mPreviewArea = new SheetPreviewArea(this, PREVIEW_AREA, wxPoint(5, 240), wxSize(300, 300));
                      vbox->Add(mPreviewArea, 7, wxEXPAND);

                      {
                          wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
                          mLoadedSheets = new wxListBox(this, SHEET_LISTBOX, wxPoint(5, 20), wxSize(200, 100), 0, 0, wxLB_ALWAYS_SB);
                          mSheetKeys = new wxListBox(this, KEY_LISTBOX, wxPoint(5, 130), wxSize(200, 100), 0, 0, wxLB_ALWAYS_SB);
                          hbox->Add(mLoadedSheets,1,wxEXPAND);
                          hbox->Add(mSheetKeys,1,wxEXPAND);

                          vbox->Add(hbox, 4, wxEXPAND);
                      }

                      {
                          wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

                            wxButton* buttonNew = new wxButton(this, -1, _("load"));
                            wxButton* buttonReload = new wxButton(this, -1, _("reload all"));
                            wxButton* buttonRemove = new wxButton(this, -1, _("remove"));

                            buttonNew->Bind(wxEVT_BUTTON, &SheetPreview::onNew, this);
                            buttonReload->Bind(wxEVT_BUTTON, &SheetPreview::onReload, this);
                            buttonRemove->Bind(wxEVT_BUTTON, &SheetPreview::onRemove, this);

                            hbox->Add(buttonNew, 1, wxCENTER);
                            hbox->Add(buttonReload, 1, wxCENTER);
                            hbox->Add(buttonRemove, 1, wxCENTER);

                          vbox->Add(hbox, 1, wxEXPAND);
                      }

                      SetSizer(vbox);
                      Fit();
                  }


    std::string SheetPreview::getCurrentKey()
    {
        if (mSheetKeys->GetSelection() != wxNOT_FOUND)
        {
            auto key = std::string{ mSheetKeys->GetString(mSheetKeys->GetSelection()).mb_str() };
            auto pos = key.find(" ");
            return key.substr(0, pos);
        }
        return "";
    }

    std::string SheetPreview::getCurrentType()
    {
        if (mSheetKeys->GetSelection() != wxNOT_FOUND)
        {
            auto type = std::string{ mSheetKeys->GetString(mSheetKeys->GetSelection()).mb_str() };
            auto pos = type.find(" ");
            return type.substr(pos+2, type.length()-pos-3);
        }
        return "";
    }

    std::string SheetPreview::getCurrentTexture() const
    {
        if (mLoadedSheets->GetSelection() != wxNOT_FOUND)
        {
            return mImages[mLoadedSheets->GetSelection()].getFilePath();
        }
        return "";
    }

    std::string SheetPreview::getCurrentMeta() const
    {
        if (mLoadedSheets->GetSelection() != wxNOT_FOUND)
        {
            return mMetas[mLoadedSheets->GetSelection()].getFilePath();
        }
        return "";
    }

    void SheetPreview::selectSheetKey(const std::string& sheetID, const std::string& key)
    {
        boost::filesystem::path p = sheetID;
        for (unsigned i = 0; i < mLoadedSheets->GetCount(); ++i)
        {
            std::string cursheet = std::string(mLoadedSheets->GetString(i).mb_str());
            if (cursheet == p.stem().string())
            {
                mLoadedSheets->Deselect(mLoadedSheets->GetSelection());
                mLoadedSheets->SetSelection(i);
                updateKeysListbox(i);
                for (unsigned j = 0; j < mSheetKeys->GetCount(); ++j)
                {
                    std::string curkey = std::string(mSheetKeys->GetString(j).mb_str());
                    if (curkey == key)
                    {
                        mSheetKeys->Deselect(mSheetKeys->GetSelection());
                        mSheetKeys->SetSelection(j);
                        mPreviewArea->preview(mImages[i], mMetas[i], key);
                        break;
                    }
                }
                break;
            }
        }
    }

    void SheetPreview::updateSheetListbox()
    {
        std::size_t prevsel;
        if (mLoadedSheets->GetSelection() == wxNOT_FOUND)
            prevsel = 0;
        else
            prevsel = mLoadedSheets->GetSelection();

        mLoadedSheets->Clear();

        for (unsigned i = 0; i < mImages.size(); ++i)
        {
            boost::filesystem::path p = mImages[i].getFilePath();
            mLoadedSheets->Append(_(p.stem().string()));
        }

        if (mImages.size() > 0)
        {
            mLoadedSheets->SetSelection(prevsel);
            updateKeysListbox(prevsel);
        }
    }

    void SheetPreview::updateKeysListbox(std::size_t sheetID)
    {
        mSheetKeys->Clear();
        for (const auto& p : mMetas[sheetID].get()->metamap)
        {
            std::string identifier = p.first;
            identifier.append(" (");
            identifier.append(p.second.getAttribute<std::string>("type", ""));
            identifier.append(")");
            mSheetKeys->Append(_(identifier));
        }
    }

    void SheetPreview::onSheetSelect(wxCommandEvent& event)
    {
        updateKeysListbox(event.GetSelection());
    }

    void SheetPreview::onKeySelect(wxCommandEvent& event)
    {
        if (mLoadedSheets->GetSelection() == wxNOT_FOUND)
            return;

        std::string key = getCurrentKey();
        mPreviewArea->preview(mImages[mLoadedSheets->GetSelection()], mMetas[mLoadedSheets->GetSelection()], key);
    }

    bool SheetPreview::loadSheet(const std::string& sheetID, const std::string& metaID)
    {
        mImages.emplace_back( sheetID, ungod::SYNC );
        mMetas.emplace_back( metaID );

        if (mImages.back().isLoaded() && mMetas.back().isLoaded())
        {
            updateSheetListbox();
            return true;
        }
        else
        {
            mImages.pop_back();
            mMetas.pop_back();
        }
        return false;
    }

    void SheetPreview::reloadSheets()
    {
        for (std::size_t i = 0; i < mImages.size();)
        {
            mImages[i].load(mImages[i].getFilePath(), ungod::SYNC );
            mMetas[i].load(mMetas[i].getFilePath() );
            if (!mImages[i].isLoaded() || !mMetas[i].isLoaded())
            {
                mImages.erase(mImages.begin() + i);
                mMetas.erase(mMetas.begin() + i);
            }
            else
            {
                ++i;
            }
        }
    }

    void SheetPreview::onNew(wxCommandEvent& event)
    {
        SheetDialog sheetdiaglog ( this, -1, _("Load sheet"), wxPoint(100, 100) );
        if ( sheetdiaglog.ShowModal() == wxID_OK )
        {
            loadSheet(std::string{sheetdiaglog.getSheetID().mb_str()}, std::string{sheetdiaglog.getMetaID().mb_str()});
        }
    }

    void SheetPreview::onReload(wxCommandEvent& event)
    {
        reloadSheets();
    }

    void SheetPreview::onRemove(wxCommandEvent& event)
    {
        if (mLoadedSheets->GetSelection() != wxNOT_FOUND)
        {
            mImages.erase(mImages.begin() + mLoadedSheets->GetSelection());
            mMetas.erase(mMetas.begin() + mLoadedSheets->GetSelection());
            mLoadedSheets->SetSelection(wxNOT_FOUND);
            updateSheetListbox();
        }
    }
}

namespace ungod
{
    //serialization code
    void SerialBehavior<uedit::SheetPreview>::serialize(const uedit::SheetPreview& data, MetaNode serializer, SerializationContext& context)
    {
        for (std::size_t i = 0; i < data.mImages.size(); ++i)
        {
            MetaNode sub = context.appendSubnode(serializer, "sheet");
            context.serializeProperty("tex", data.mImages[i].getFilePath(), sub);
            context.serializeProperty("meta", data.mMetas[i].getFilePath(), sub);
        }
    }

    void DeserialBehavior<uedit::SheetPreview>::deserialize(uedit::SheetPreview& data, MetaNode deserializer, DeserializationContext& context)
    {
        forEachSubnode(deserializer, [&data] (MetaNode sub)
        {
            auto sheetdata = sub.getAttributes<std::string, std::string>({"tex", ""}, {"meta", ""});
            if (std::get<0>(sheetdata) != "" && std::get<1>(sheetdata) != "")
            {
                data.loadSheet(std::get<0>(sheetdata), std::get<1>(sheetdata));
            }
        }, "sheet");
    }
}
