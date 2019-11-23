#include "EntityVisualsWindow.h"
#include "EditorFrame.h"
#include "wx/sizer.h"

namespace uedit
{

    wxBEGIN_EVENT_TABLE(EntityVisualsWindow, wxWindow)
        EVT_FILEPICKER_CHANGED(FILEPICKER, EntityVisualsWindow::onTexSelect)
        EVT_BUTTON(TEX_COPY_BUTTON, EntityVisualsWindow::onTexCopyButton)
        EVT_BUTTON(SPRITE_RECT_BY_KEY, EntityVisualsWindow::onRectByMetaKey)
        EVT_BUTTON(MULTI_SPRITE_RECT_BY_KEY, EntityVisualsWindow::onMultiRectByMetaKey)
        EVT_BUTTON(SET_MULTI_SPRITE_COMPONENT_COUNT, EntityVisualsWindow::onMultiSpriteCompCountChange)
        EVT_CHOICE(CHOICE_MULTISPRITE, EntityVisualsWindow::onMultiSpriteSelect)
        EVT_BUTTON(SET_VERTEX_RECT_COUNT, EntityVisualsWindow::onVertexRectCountChange)
        EVT_CHOICE(CHOICE_VERTEX, EntityVisualsWindow::onVectexRectSelect)
        EVT_BUTTON(VERTEX_RECT_BY_KEY, EntityVisualsWindow::onVertexRectByKey)
        EVT_COLOURPICKER_CHANGED(VERTEX_COLOR_PICKER, EntityVisualsWindow::onVertexColorSelect)
    wxEND_EVENT_TABLE()

    EntityVisualsWindow::EntityVisualsWindow(ungod::Entity e, WorldActionWrapper& waw, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz) :
        wxWindow(parent, id, pos, siz), mEntity(e), mWorldAction(waw), mTypeNotebook(nullptr), mTexRectLabel(nullptr), mMultiTexRectLabel(nullptr),
        mCompCountCtrl(nullptr), mMultiSpriteChoice(nullptr), mSpritePanel(nullptr), mMultiSpritePanel(nullptr),
        mVertexPanel(nullptr), mVertexRectCountCtrl(nullptr), mVertexChoice(nullptr), mVertexTexRectLabel(nullptr),
        mVertexPositionX(nullptr), mVertexPositionY(nullptr), mSelectedVertex(-1), mVertexColor(nullptr)
    {
        wxSizer* boxsizer = new wxBoxSizer(wxVERTICAL);

        {
            wxPanel* texloadPanel = new wxPanel(this);
            wxBoxSizer* texloadSizer = new wxBoxSizer(wxVERTICAL);

            //build an environment to load a texture for the entity and a button, that uses the currently in the sheet-manager selected texture as the entity texture
            wxStaticText* texlabel = new wxStaticText(texloadPanel, -1, _("Loaded texture: "));
            wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
            hbox1->Add(texlabel, 1, wxALL, 8);
            texloadSizer->Add(hbox1,0,wxALIGN_CENTER_HORIZONTAL);

            mSheetPicker = new wxFilePickerCtrl( texloadPanel, FILEPICKER, fileDialogHeader("png"), wxFileSelectorPromptStr, fileDialogWildcard("png")  );
            wxBoxSizer* hbox2 = new wxBoxSizer(wxHORIZONTAL);
            hbox2->Add(mSheetPicker,1);
            texloadSizer->Add(hbox2,0,wxALIGN_CENTER_HORIZONTAL);

            wxBoxSizer* hbox3 = new wxBoxSizer(wxHORIZONTAL);
            hbox3->Add(new wxButton(texloadPanel, TEX_COPY_BUTTON, "Use selected texture"),1, wxTOP | wxBOTTOM, 10);
            texloadSizer->Add(hbox3,0,wxALIGN_CENTER_HORIZONTAL);

            mHideForCameraChecked = new wxCheckBox(texloadPanel, -1, _("Hide for camera"));
            mHideForCameraChecked->Bind(wxEVT_CHECKBOX, &EntityVisualsWindow::onHideForCameraChecked, this);
            mHideForCameraChecked->SetValue(mEntity.get<ungod::VisualsComponent>().isHiddenForCamera());
            texloadSizer->Add(mHideForCameraChecked,0,wxALIGN_CENTER_HORIZONTAL);

            texloadPanel->SetSizer(texloadSizer);
            texloadPanel->Fit();

            boxsizer->Add(texloadPanel,0,wxALIGN_CENTER | wxALL, 8);
        }

        //create a type notebook an add pages depending on what components are present
        {
            mTypeNotebook = new wxNotebook(this, TYPE_NOTEBOOK, wxDefaultPosition, wxDefaultSize, wxNB_TOP);

            boxsizer->Add(mTypeNotebook,0,wxALIGN_CENTER);
        }

        if (mEntity.has<ungod::SpriteComponent>())
        {
            buildSpriteEdit(mEntity.modify<ungod::SpriteComponent>());
        }

        if (mEntity.has<ungod::MultiSpriteComponent>())
        {
            setUpMultiSpriteEdit();
        }

        if (mEntity.has<ungod::VertexArrayComponent>())
        {
            buildVertexEdit();
        }

        SetSizer(boxsizer);
        Fit();

        //if the entity already has a texture, display it in the text field
        if (e.get<ungod::VisualsComponent>().isLoaded())  //has<visuals> is true but get returns broken reference??
        {
            mSheetPicker->SetPath(_(e.get<ungod::VisualsComponent>().getFilePath()));
        }
    }

    void EntityVisualsWindow::onEntityContentsChanged(ungod::Entity e)
    {
        if (mEntity.has<ungod::SpriteComponent>())
        {
            updateSpriteData();
        }
        if (mEntity.has<ungod::MultiSpriteComponent>())
        {
            updateMultiSpriteData();
        }

        if (mEntity.has<ungod::VertexArrayComponent>())
        {
            displayVertexRect(mSelectedVertex);
        }
    }

    void EntityVisualsWindow::onTexSelect(wxFileDirPickerEvent& event)
    {
        mWorldAction.loadTexture(mEntity, std::string(event.GetPath().mb_str()));
    }

    void EntityVisualsWindow::onTexCopyButton(wxCommandEvent & event)
    {
        if (!mWorldAction.getEditorFrame()->getSheetPreview())
        {
            auto err = wxMessageDialog(this, _("Sheet preview is currently closed!"));
            err.ShowModal();
            return;
        }

        std::string selectedTex = mWorldAction.getEditorFrame()->getSheetPreview()->getCurrentTexture();

        if (selectedTex == "")
        {
            auto err = wxMessageDialog(this, _("No valid sheet selected in the sheet preview!"));
            err.ShowModal();
            return;
        }

        mWorldAction.loadTexture(mEntity, selectedTex);
        if (mEntity.get<ungod::VisualsComponent>().isLoaded())
        {
            mSheetPicker->SetPath(_(selectedTex));
        }
    }

    void EntityVisualsWindow::onRectByMetaKey(wxCommandEvent & event)
    {
        if (!mWorldAction.getEditorFrame()->getSheetPreview())
        {
            auto err = wxMessageDialog(this, _("Sheet preview is currently closed!"));
            err.ShowModal();
            return;
        }

        std::string selectedKey = mWorldAction.getEditorFrame()->getSheetPreview()->getCurrentKey();

        if (selectedKey == "")
        {
            auto err = wxMessageDialog(this, _("No valid key selected in the sheet preview!"));
            err.ShowModal();
            return;
        }

        std::string selectedMeta = mWorldAction.getEditorFrame()->getSheetPreview()->getCurrentMeta();

        if (selectedMeta == "")
        {
            auto err = wxMessageDialog(this, _("No valid meta file selected in the sheet preview!"));
            err.ShowModal();
            return;
        }

        mWorldAction.loadMetadata(mEntity, selectedMeta);
        mWorldAction.setSpriteTextureRect(mEntity, selectedKey);
    }


    void EntityVisualsWindow::onHideForCameraChecked(wxCommandEvent & event)
    {
        mWorldAction.setEntityHideForCamera(mEntity, mHideForCameraChecked->IsChecked());
    }


    void EntityVisualsWindow::onMultiRectByMetaKey(wxCommandEvent & event)
    {
        if (!mWorldAction.getEditorFrame()->getSheetPreview())
        {
            auto err = wxMessageDialog(this, _("Sheet preview is currently closed!"));
            err.ShowModal();
            return;
        }

        std::string selectedKey = mWorldAction.getEditorFrame()->getSheetPreview()->getCurrentKey();

        if (selectedKey == "")
        {
            auto err = wxMessageDialog(this, _("No valid key selected in the sheet preview!"));
            err.ShowModal();
            return;
        }

        std::string selectedMeta = mWorldAction.getEditorFrame()->getSheetPreview()->getCurrentMeta();

        if (selectedMeta == "")
        {
            auto err = wxMessageDialog(this, _("No valid meta file selected in the sheet preview!"));
            err.ShowModal();
            return;
        }

        mWorldAction.loadMetadata(mEntity, selectedMeta);
        mWorldAction.setMultiSpriteTextureRect(mEntity, selectedKey, mMultiSpriteChoice->GetSelection());
    }

    void EntityVisualsWindow::onMultiSpriteCompCountChange(wxCommandEvent & event)
    {
        try
        {
            std::size_t cc = (std::size_t)(std::stoi( std::string(mCompCountCtrl->GetValue().mb_str()) ));
            mEntity.initMulti<ungod::MultiSpriteComponent>(cc);

			std::vector<wxString> keys{ cc };
            for (std::size_t i = 0; i < cc; ++i)
            {
                keys[i] = std::to_string(i);
            }
            mMultiSpriteChoice->Set(keys);

            if (cc > 0)
            {
                mMultiSpriteChoice->SetSelection(0);
                buildMultiSpriteEdit(mEntity.modify<ungod::MultiSpriteComponent>().getComponent(0), 0);
            }
        }
        catch(const std::exception& e)
        {
            auto err = wxMessageDialog(this, _("Text field must contain a valid number."));
            err.ShowModal();
        }
    }

    void EntityVisualsWindow::setTexRectLabel(wxStaticText* text, const sf::FloatRect& rect)
    {
        wxString recttex;
        recttex.append("(");
        recttex.append(std::to_string((int)rect.left));
        recttex.append(", ");
        recttex.append(std::to_string((int)rect.top));
        recttex.append(", ");
        recttex.append(std::to_string((int)rect.width));
        recttex.append(", ");
        recttex.append(std::to_string((int)rect.height));
        recttex.append(")");

        text->SetLabel(recttex);
    }

    void EntityVisualsWindow::setUpMultiSpriteEdit()
    {
        ungod::MultiSpriteComponent& multisprite = mEntity.modify<ungod::MultiSpriteComponent>();

        mMultiSpriteParent = new wxPanel(mTypeNotebook);
        mMultiSpriteSizer = new wxBoxSizer(wxVERTICAL);

        {
            wxPanel* multispritePanel = new wxPanel(mMultiSpriteParent);
            wxBoxSizer* multispriteSizer = new wxBoxSizer(wxVERTICAL);

            mCompCountCtrl = new wxTextCtrl(multispritePanel, -1, _(std::to_string(multisprite.getComponentCount())));
            wxBoxSizer* hbox0 = new wxBoxSizer(wxHORIZONTAL);
            hbox0->Add(mCompCountCtrl);

            hbox0->Add(new wxButton(multispritePanel, SET_MULTI_SPRITE_COMPONENT_COUNT, "Set sprite count"));
            multispriteSizer->Add(hbox0,0,wxALIGN_CENTER_HORIZONTAL);

            std::size_t compCount = mEntity.get<ungod::MultiSpriteComponent>().getComponentCount();

            wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
            mMultiSpriteChoice = new wxChoice(multispritePanel, CHOICE_MULTISPRITE, wxDefaultPosition, wxDefaultSize, compCount);
            for (std::size_t i = 0; i < compCount; ++i)
            {
                mMultiSpriteChoice->SetString(i, std::to_string(i));
            }
            hbox1->Add(mMultiSpriteChoice, 1, wxALL, 5);

            multispriteSizer->Add(hbox1,0,wxALIGN_CENTER_HORIZONTAL);

            multispritePanel->SetSizer(multispriteSizer);
            multispritePanel->Fit();

            mMultiSpriteSizer->Add(multispritePanel,0,wxALIGN_CENTER | wxALL, 5);

            if (compCount > 0)
            {
                mMultiSpriteChoice->SetSelection(0);
                buildMultiSpriteEdit(mEntity.modify<ungod::MultiSpriteComponent>().getComponent(0), 0);
            }
        }

        mMultiSpriteParent->SetSizer(mMultiSpriteSizer);
        mMultiSpriteParent->Fit();

        mTypeNotebook->AddPage(mMultiSpriteParent, "MultiSprite");
    }

    void EntityVisualsWindow::onMultiSpriteSelect(wxCommandEvent & event)
    {
        std::size_t i = (std::size_t)event.GetSelection();
        buildMultiSpriteEdit(mEntity.modify<ungod::MultiSpriteComponent>().getComponent(i), i);
    }

    void EntityVisualsWindow::onVertexRectCountChange(wxCommandEvent & event)
    {
        try
        {
            std::size_t cc = (std::size_t)(std::stoi( std::string(mVertexRectCountCtrl->GetValue().mb_str()) ));
            mWorldAction.setVertexRectCount(mEntity, cc);

			std::vector<wxString> keys{ cc };
            for (std::size_t i = 0; i < cc; ++i)
            {
                keys[i] = std::to_string(i);
            }
            mVertexChoice->Set(keys);

            if (cc > 0)
            {
                mVertexChoice->SetSelection(0);
                displayVertexRect(0);
            }
            else
                displayVertexRect(-1);
        }
        catch(const std::exception& e)
        {
            auto err = wxMessageDialog(this, _("Text field must contain a valid number."));
            err.ShowModal();
        }
    }

    void EntityVisualsWindow::onVectexRectSelect(wxCommandEvent & event)
    {
        std::size_t i = (std::size_t)event.GetSelection();
        displayVertexRect(i);
    }

    void EntityVisualsWindow::onVertexRectByKey(wxCommandEvent & event)
    {
        if (mSelectedVertex == -1)
            return;

        if (!mWorldAction.getEditorFrame()->getSheetPreview())
        {
            auto err = wxMessageDialog(this, _("Sheet preview is currently closed!"));
            err.ShowModal();
            return;
        }

        std::string selectedKey = mWorldAction.getEditorFrame()->getSheetPreview()->getCurrentKey();

        if (selectedKey == "")
        {
            auto err = wxMessageDialog(this, _("No valid key selected in the sheet preview!"));
            err.ShowModal();
            return;
        }

        std::string selectedMeta = mWorldAction.getEditorFrame()->getSheetPreview()->getCurrentMeta();

        if (selectedMeta == "")
        {
            auto err = wxMessageDialog(this, _("No valid meta file selected in the sheet preview!"));
            err.ShowModal();
            return;
        }

        mWorldAction.loadMetadata(mEntity, selectedMeta);
        mWorldAction.setVertexArrayRect(mEntity, mSelectedVertex, selectedKey);

        displayVertexRect(mSelectedVertex);
    }

    void EntityVisualsWindow::onVertexColorSelect(wxColourPickerEvent& event)
    {
        if (mSelectedVertex != -1)
        {
            mWorldAction.setVertexArrayRectColor(mEntity, mSelectedVertex, convertColor(event.GetColour()));
            displayVertexRect(mSelectedVertex);
        }
    }

    void EntityVisualsWindow::buildSpriteEdit(ungod::SpriteComponent& sprite)
    {
        mSpritePanel = new wxPanel(mTypeNotebook);
        wxBoxSizer* spriteSizer = new wxBoxSizer(wxVERTICAL);

        //build a text display for the current tex rect and a button to change it
        wxStaticText* rectlabel = new wxStaticText(mSpritePanel, -1, _("sprite data: "));
        wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
        hbox1->Add(rectlabel, 1, wxALL, 5);
        spriteSizer->Add(hbox1,0,wxALIGN_CENTER_HORIZONTAL);


        wxBoxSizer* hbox2 = new wxBoxSizer(wxHORIZONTAL);
        hbox2->Add(new wxStaticText(mSpritePanel, -1, _("tex rect: ")), 1, wxALIGN_LEFT | wxALL, 3);
        mTexRectLabel = new wxStaticText(mSpritePanel, -1, _(""));
        hbox2->Add(mTexRectLabel, 1, wxALL, 3);
        spriteSizer->Add(hbox2,0,wxALIGN_CENTER_HORIZONTAL);


        {
            mSpritePositionX = new StatDisplay<float>("position x:", mSpritePanel, -1);
            mSpritePositionX->connectSetter( [this](float x)
                    {
                        mWorldAction.setSpritePosition(mEntity, { x, mEntity.get<ungod::SpriteComponent>().getSprite().getPosition().y });
                    } );
            mSpritePositionX->connectGetter( [this]()
                    {
                        return mEntity.get<ungod::SpriteComponent>().getSprite().getPosition().x;
                    } );
            spriteSizer->Add(mSpritePositionX,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mSpritePositionY = new StatDisplay<float>("position y:", mSpritePanel, -1);
            mSpritePositionY->connectSetter( [this](float y)
                    {
                        mWorldAction.setSpritePosition(mEntity, { mEntity.get<ungod::SpriteComponent>().getSprite().getPosition().x, y });
                    } );
            mSpritePositionY->connectGetter( [this]()
                    {
                        return mEntity.get<ungod::SpriteComponent>().getSprite().getPosition().y;
                    } );
            spriteSizer->Add(mSpritePositionY,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mSpriteScaleX = new StatDisplay<float>("scale x:", mSpritePanel, -1);
            mSpriteScaleX->connectSetter( [this](float scalex)
                    {
                        mWorldAction.setSpriteScale(mEntity, { scalex, mEntity.get<ungod::SpriteComponent>().getSprite().getScale().y });
                    } );
            mSpriteScaleX->connectGetter( [this]()
                    {
                        return mEntity.get<ungod::SpriteComponent>().getSprite().getScale().x;
                    } );
            spriteSizer->Add(mSpriteScaleX,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mSpriteScaleY = new StatDisplay<float>("scale y:", mSpritePanel, -1);
            mSpriteScaleY->connectSetter( [this](float scaley)
                    {
                        mWorldAction.setSpriteScale(mEntity, { mEntity.get<ungod::SpriteComponent>().getSprite().getScale().x, scaley });
                    } );
            mSpriteScaleY->connectGetter( [this]()
                    {
                        return mEntity.get<ungod::SpriteComponent>().getSprite().getScale().y;
                    } );
            spriteSizer->Add(mSpriteScaleY,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mSpriteRotation = new StatDisplay<float>("rotation:", mSpritePanel, -1);
            mSpriteRotation->connectSetter( [this](float rota)
                    {
                        mWorldAction.setSpriteRotation(mEntity, rota);
                    } );
            mSpriteRotation->connectGetter( [this]()
                    {
                        return mEntity.get<ungod::SpriteComponent>().getSprite().getRotation();
                    } );
            spriteSizer->Add(mSpriteRotation,0,wxALIGN_CENTER_HORIZONTAL);
        }

        if (mEntity.has<ungod::SpriteMetadataComponent>())
        {
            wxBoxSizer* hboxmeta = new wxBoxSizer(wxHORIZONTAL);
            hboxmeta->Add(new wxButton(mSpritePanel, SPRITE_RECT_BY_KEY, "Set rect by meta key"), 1, wxALL, 5);
            spriteSizer->Add(hboxmeta,0,wxALIGN_CENTER_HORIZONTAL);
        }

        mSpritePanel->SetSizer(spriteSizer);
        mSpritePanel->Fit();

        mTypeNotebook->AddPage(mSpritePanel,"Sprite");

        updateSpriteData();
    }

    void EntityVisualsWindow::buildMultiSpriteEdit(ungod::SpriteComponent& sprite, std::size_t index)
    {
        mCurMultiSelect = index;

        if (mMultiSpritePanel)
        {
            mMultiSpriteParent->RemoveChild(mMultiSpritePanel);
            mMultiSpritePanel->Destroy();
        }
        mMultiSpritePanel = new wxPanel(mMultiSpriteParent);
        wxBoxSizer* spriteSizer = new wxBoxSizer(wxVERTICAL);

        //build a text display for the current tex rect and a button to change it
        wxStaticText* rectlabel = new wxStaticText(mMultiSpritePanel, -1, _("tex rect: "));
        wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
        hbox1->Add(rectlabel, 1, wxALL, 5);
        spriteSizer->Add(hbox1,0,wxALIGN_CENTER_HORIZONTAL);

        mMultiTexRectLabel = new wxStaticText(mMultiSpritePanel, -1, _(""));
        wxBoxSizer* hbox3 = new wxBoxSizer(wxHORIZONTAL);
        hbox3->Add(mMultiTexRectLabel, 1, wxALL, 5);
        spriteSizer->Add(hbox3,0,wxALIGN_CENTER_HORIZONTAL);

         {
            mMultiSpritePositionX = new StatDisplay<float>("position x:", mMultiSpritePanel, -1);
            mMultiSpritePositionX->connectSetter( [this, index](float x)
                    {
                        mWorldAction.setMultiSpritePosition(mEntity, { x, mEntity.get<ungod::SpriteComponent>().getSprite().getPosition().y }, index);
                    } );
            mMultiSpritePositionX->connectGetter( [this, index]()
                    {
                        return mEntity.get<ungod::MultiSpriteComponent>().getComponent(index).getSprite().getPosition().x;
                    } );
            spriteSizer->Add(mMultiSpritePositionX,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mMultiSpritePositionY = new StatDisplay<float>("position y:", mMultiSpritePanel, -1);
            mMultiSpritePositionY->connectSetter( [this, index](float y)
                    {
                        mWorldAction.setMultiSpritePosition(mEntity, { mEntity.get<ungod::SpriteComponent>().getSprite().getPosition().x, y }, index);
                    } );
            mMultiSpritePositionY->connectGetter( [this, index]()
                    {
                        return mEntity.get<ungod::MultiSpriteComponent>().getComponent(index).getSprite().getPosition().y;
                    } );
            spriteSizer->Add(mMultiSpritePositionY,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mMultiSpriteScaleX = new StatDisplay<float>("scale x:", mMultiSpritePanel, -1);
            mMultiSpriteScaleX->connectSetter( [this, index](float scalex)
                    {
                        mWorldAction.setMultiSpriteScale(mEntity, { scalex, mEntity.get<ungod::SpriteComponent>().getSprite().getScale().y }, index);
                    } );
            mMultiSpriteScaleX->connectGetter( [this, index]()
                    {
                        return mEntity.get<ungod::MultiSpriteComponent>().getComponent(index).getSprite().getScale().x;
                    } );
            spriteSizer->Add(mMultiSpriteScaleX,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mMultiSpriteScaleY = new StatDisplay<float>("scale y:", mMultiSpritePanel, -1);
            mMultiSpriteScaleY->connectSetter( [this, index](float scaley)
                    {
                        mWorldAction.setMultiSpriteScale(mEntity, { mEntity.get<ungod::SpriteComponent>().getSprite().getScale().x, scaley }, index);
                    } );
            mMultiSpriteScaleY->connectGetter( [this, index]()
                    {
                        return mEntity.get<ungod::MultiSpriteComponent>().getComponent(index).getSprite().getScale().y;
                    } );
            spriteSizer->Add(mMultiSpriteScaleY,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mMultiSpriteRotation = new StatDisplay<float>("rotation:", mMultiSpritePanel, -1);
            mMultiSpriteRotation->connectSetter( [this, index](float rota)
                    {
                        mWorldAction.setMultiSpriteRotation(mEntity, rota, index);
                    } );
            mMultiSpriteRotation->connectGetter( [this, index]()
                    {
                        return mEntity.get<ungod::MultiSpriteComponent>().getComponent(index).getSprite().getRotation();
                    } );
            spriteSizer->Add(mMultiSpriteRotation,0,wxALIGN_CENTER_HORIZONTAL);
        }

        if (mEntity.has<ungod::SpriteMetadataComponent>())
        {
            wxBoxSizer* hbox4 = new wxBoxSizer(wxHORIZONTAL);
            hbox4->Add(new wxButton(mMultiSpritePanel, MULTI_SPRITE_RECT_BY_KEY, "Set rect by meta key"), 1, wxALL, 5);
            spriteSizer->Add(hbox4,0,wxALIGN_CENTER_HORIZONTAL);
        }

        mMultiSpritePanel->SetSizer(spriteSizer);
        mMultiSpritePanel->Fit();

        mMultiSpriteSizer->Add(mMultiSpritePanel,0,wxALIGN_CENTER | wxALL, 5);
        mMultiSpriteSizer->Fit(mMultiSpriteParent);

        updateMultiSpriteData();
    }

    void EntityVisualsWindow::buildVertexEdit()
    {
        ungod::VertexArrayComponent& vertices = mEntity.modify<ungod::VertexArrayComponent>();

        mVertexPanel = new wxPanel(mTypeNotebook);
        wxBoxSizer* vertexSizer = new wxBoxSizer(wxVERTICAL);


        std::size_t rectCount = vertices.getVertices().textureRectCount();


        mVertexRectCountCtrl = new wxTextCtrl(mVertexPanel, -1, _(std::to_string(rectCount)));
        wxBoxSizer* hbox0 = new wxBoxSizer(wxHORIZONTAL);
        hbox0->Add(mVertexRectCountCtrl);

        hbox0->Add(new wxButton(mVertexPanel, SET_VERTEX_RECT_COUNT, "Set tex rect count"));
        vertexSizer->Add(hbox0,0,wxALIGN_CENTER_HORIZONTAL);


        wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
        mVertexChoice = new wxChoice(mVertexPanel, CHOICE_VERTEX, wxDefaultPosition, wxDefaultSize);
        for (std::size_t i = 0; i < rectCount; ++i)
        {
            mVertexChoice->SetString(i, std::to_string(i));
        }
        hbox1->Add(mVertexChoice, 1, wxALL, 5);

        vertexSizer->Add(hbox1,0,wxALIGN_CENTER_HORIZONTAL);


        {
            //build a text display for the current tex rect and a button to change it
            wxStaticText* rectlabel = new wxStaticText(mVertexPanel, -1, _("vertex rect data: "));
            {
                wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
                hbox->Add(rectlabel, 1, wxALL, 5);
                vertexSizer->Add(hbox,0,wxALIGN_CENTER_HORIZONTAL);
            }


            {
                wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
                hbox->Add(new wxStaticText(mVertexPanel, -1, _("tex rect: ")), 1, wxALIGN_LEFT | wxALL, 3);
                mVertexTexRectLabel = new wxStaticText(mVertexPanel, -1, _(""));
                hbox->Add(mVertexTexRectLabel, 1, wxALL, 3);
                vertexSizer->Add(hbox,0,wxALIGN_CENTER_HORIZONTAL);
            }

            {
                mVertexPositionX = new StatDisplay<float>("position x:", mVertexPanel, -1);
                vertexSizer->Add(mVertexPositionX,0,wxALIGN_CENTER_HORIZONTAL);
            }

            {
                mVertexPositionY = new StatDisplay<float>("position y:", mVertexPanel, -1);
                vertexSizer->Add(mVertexPositionY,0,wxALIGN_CENTER_HORIZONTAL);
            }

            {
                wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
                hbox->Add(new wxStaticText(mVertexPanel, -1, _("color: ")), 1, wxALIGN_LEFT | wxALL, 3);
                mVertexColor = new wxColourPickerCtrl(mVertexPanel, VERTEX_COLOR_PICKER);
                hbox->Add(mVertexColor, 1, wxALL, 3);
                vertexSizer->Add(hbox,0,wxALIGN_CENTER_HORIZONTAL);
            }

            if (mEntity.has<ungod::SpriteMetadataComponent>())
            {
                wxBoxSizer* hboxmeta = new wxBoxSizer(wxHORIZONTAL);
                hboxmeta->Add(new wxButton(mVertexPanel, VERTEX_RECT_BY_KEY, "Set rect by meta key"), 1, wxALL, 5);
                vertexSizer->Add(hboxmeta,0,wxALIGN_CENTER_HORIZONTAL);
            }
        }



        mVertexPanel->SetSizer(vertexSizer);
        mVertexPanel->Fit();

        mTypeNotebook->AddPage(mVertexPanel,"VertexArray");

        if (rectCount > 0)
            displayVertexRect(0);
        else
            displayVertexRect(-1);
    }

    void EntityVisualsWindow::updateSpriteData()
    {
        setTexRectLabel(mTexRectLabel, mEntity.modify<ungod::SpriteComponent>().getSprite().getTextureRect());
        mSpritePositionX->setValue(mEntity.modify<ungod::SpriteComponent>().getSprite().getPosition().x);
        mSpritePositionY->setValue(mEntity.modify<ungod::SpriteComponent>().getSprite().getPosition().y);
        mSpriteScaleX->setValue(mEntity.modify<ungod::SpriteComponent>().getSprite().getScale().x);
        mSpriteScaleY->setValue(mEntity.modify<ungod::SpriteComponent>().getSprite().getScale().y);
        mSpriteRotation->setValue(mEntity.modify<ungod::SpriteComponent>().getSprite().getRotation());
    }

    void EntityVisualsWindow::updateMultiSpriteData()
    {
        setTexRectLabel(mMultiTexRectLabel, mEntity.modify<ungod::MultiSpriteComponent>().getComponent(mCurMultiSelect).getSprite().getTextureRect());
        mMultiSpritePositionX->setValue(mEntity.modify<ungod::MultiSpriteComponent>().getComponent(mCurMultiSelect).getSprite().getPosition().x);
        mMultiSpritePositionY->setValue(mEntity.modify<ungod::MultiSpriteComponent>().getComponent(mCurMultiSelect).getSprite().getPosition().y);
        mMultiSpriteScaleX->setValue(mEntity.modify<ungod::MultiSpriteComponent>().getComponent(mCurMultiSelect).getSprite().getScale().x);
        mMultiSpriteScaleY->setValue(mEntity.modify<ungod::MultiSpriteComponent>().getComponent(mCurMultiSelect).getSprite().getScale().y);
        mMultiSpriteRotation->setValue(mEntity.modify<ungod::MultiSpriteComponent>().getComponent(mCurMultiSelect).getSprite().getRotation());
    }

    void EntityVisualsWindow::displayVertexRect(std::size_t index)
    {
        mSelectedVertex = index;
        if ((int)mSelectedVertex != -1)
        {
            setTexRectLabel(mVertexTexRectLabel, mEntity.modify<ungod::VertexArrayComponent>().getVertices().getTextureRect(index));
            mVertexPositionX->connectSetter( [this, index](float posx)
                    {
                        mWorldAction.setVertexarrayPosition( mEntity,
                                    sf::Vector2f { posx, mEntity.modify<ungod::VertexArrayComponent>().getVertices().getPosition(index).y }, index );
                    } );
            mVertexPositionX->connectGetter( [this, index]()
                    {
                        return mEntity.modify<ungod::VertexArrayComponent>().getVertices().getPosition(index).x;
                    } );
            mVertexPositionY->connectSetter( [this, index](float posy)
                    {
                        mWorldAction.setVertexarrayPosition( mEntity,
                                    sf::Vector2f { mEntity.modify<ungod::VertexArrayComponent>().getVertices().getPosition(index).x, posy },index );
                    } );
            mVertexPositionY->connectGetter( [this, index]()
                    {
                        return mEntity.modify<ungod::VertexArrayComponent>().getVertices().getPosition(index).y;
                    } );
            mVertexPositionX->setValue(mEntity.modify<ungod::VertexArrayComponent>().getVertices().getPosition(index).x);
            mVertexPositionY->setValue(mEntity.modify<ungod::VertexArrayComponent>().getVertices().getPosition(index).y);
            mVertexColor->SetColour(convertColor(mEntity.modify<ungod::VertexArrayComponent>().getVertices().getRectColor(index)));
            mVertexTexRectLabel->Show();
            mVertexPositionX->Show();
            mVertexPositionY->Show();
            mVertexColor->Show();
        }
        else  //-1 means display, when no rect is selected (empty fields)
        {
            setTexRectLabel(mVertexTexRectLabel, sf::FloatRect{0,0,0,0});
            mVertexPositionX->connectSetter( [this, index](float pos)
                    {
                    } );
            mVertexPositionX->connectGetter( [this, index]()
                    {
                        return 0.0f;
                    } );
            mVertexPositionY->connectSetter( [this, index](float pos)
                    {
                    } );
            mVertexPositionY->connectGetter( [this, index]()
                    {
                        return 0.0f;
                    } );
            mVertexPositionX->setValue(0.0f);
            mVertexPositionY->setValue(0.0f);
            mVertexTexRectLabel->Hide();
            mVertexPositionX->Hide();
            mVertexPositionY->Hide();
            mVertexColor->Hide();
        }
        mVertexPanel->Fit();
        mTypeNotebook->Fit();
    }
}
