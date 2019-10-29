#include "EntityParticleSystemWindow.h"
#include "EditorFrame.h"
#include "ParticleEditState.h"
#include "wx/sizer.h"
#include "wx/clrpicker.h"

namespace uedit
{
    EntityParticleSystemWindow::EntityParticleSystemWindow(ungod::Entity e, WorldActionWrapper& waw, EntityPreview& preview, wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize& siz) :
        wxWindow(parent, id, pos, siz), mEntity(e), mWorldAction(waw), mPreview(preview), mEmitterPanel(nullptr), mTexrectInitPanel(nullptr), mAffectorsPanel(nullptr)
    {
        mBox = new wxBoxSizer(wxVERTICAL);

        setupStatDisplays();

        mTabs = new wxNotebook(this, -1);

        setupEmitter();

        updateTexrectInit();

        updateAffectors();

        mBox->Add(mTabs,0,wxEXPAND);

        refreshStats();

        SetSizer(mBox);
        Layout();
        Fit();
    }

    void EntityParticleSystemWindow::refreshStats()
    {
        mMaxForce->refreshValue();
        mMaxVel->refreshValue();
        mSpeed->refreshValue();
    }

    void EntityParticleSystemWindow::setupStatDisplays()
    {
        {
            mMaxForce = new StatDisplay<float>("max force:", this, -1);
            mMaxForce->connectSetter( [this](float x)
            {
                mWorldAction.setParticleMaxForce(mEntity, x);
            } );
            mMaxForce->connectGetter( [this]()
            {
                return mEntity.get<ungod::ParticleSystemComponent>().getSystem().getMaxForce();
            } );

            mBox->Add(mMaxForce,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mMaxVel = new StatDisplay<float>("max velocity:", this, -1);
            mMaxVel->connectSetter( [this](float x)
            {
                mWorldAction.setParticleMaxVelocity(mEntity, x);
            } );
            mMaxVel->connectGetter( [this]()
            {
                return mEntity.get<ungod::ParticleSystemComponent>().getSystem().getMaxVelocity();
            } );

            mBox->Add(mMaxVel,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mSpeed = new StatDisplay<float>("speed:", this, -1);
            mSpeed->connectSetter( [this](float x)
            {
                mWorldAction.setParticleSpeed(mEntity, x);
            } );
            mSpeed->connectGetter( [this]()
            {
                return mEntity.get<ungod::ParticleSystemComponent>().getSystem().getParticleSpeed();
            } );

            mBox->Add(mSpeed,0,wxALIGN_CENTER_HORIZONTAL);
        }
    }


    void EntityParticleSystemWindow::setupEmitter()
    {
        wxArrayString emitters;
        emitters.Add(ungod::PS_UNIVERSAL_EMITTER);
        mEmitterChoice = new LabeledChoice(this, -1, emitters, _("Emitter: "));
        mEmitterChoice->bindChoice([this] (wxCommandEvent& event) { updateEmitter(); } );
        mBox->Add(mEmitterChoice,0,wxALIGN_CENTER_HORIZONTAL | wxTOP | wxBOTTOM, 16);
        onParticleEmitterChanged( mEntity.get<ungod::ParticleSystemComponent>().getSystem().getEmitterKey(),
                                 mEntity.get<ungod::ParticleSystemComponent>().getSystem().getEmitter<ungod::PSData>() );
    }


    void EntityParticleSystemWindow::updateEmitter()
    {
        int s = mEmitterChoice->getChoice()->GetSelection();
        if (s == wxNOT_FOUND)
            return;
        std::string key = std::string{mEmitterChoice->getChoice()->GetString(s).mb_str()};

        if (mEmitterPanel) //if a panel already exist, detach and destroy it first
        {
            mTabs->DeletePage(0);
            mEmitterPanel = nullptr;
        }

        if (key == ungod::PS_UNIVERSAL_EMITTER)
        {
            mEmitterPanel = new UniversalEmitterPanel(mEntity, mWorldAction, mTabs);
        }
        else
        {
            mEmitterPanel = new wxPanel(mTabs);
        }

        mTabs->InsertPage(0, mEmitterPanel, _("Emitter"));
        mTabs->SetSelection(0);

        if (mPreview.isInState<ParticleEditState>())
            mPreview.getState<ParticleEditState>().refresh(mPreview, mEntity);

        Layout();
        Fit();
    }


    void EntityParticleSystemWindow::updateTexrectInit()
    {
        if (mTexrectInitPanel) //if a panel already exist, detach and destroy it first
        {
            mTabs->DeletePage(1);
            mTexrectInitPanel = nullptr;
        }

        mTexrectInitPanel = new TexrectInitailizerPanel(mEntity, mWorldAction, mTabs);
        mTabs->InsertPage(1, mTexrectInitPanel, _("TexrectInit"));
        mTabs->SetSelection(1);

        Layout();
        Fit();
    }


    void EntityParticleSystemWindow::updateAffectors()
    {
        int sel = wxNOT_FOUND;

        if (mAffectorsPanel) //if a panel already exist, detach and destroy it first
        {
            sel = static_cast<AffectorsPanel*>( mAffectorsPanel )->getSelection();
            mTabs->DeletePage(2);
            mAffectorsPanel = nullptr;
        }

        mAffectorsPanel = new AffectorsPanel(mEntity, mWorldAction, mTabs, sel);
        mTabs->InsertPage(2, mAffectorsPanel, _("Affectors"));
        mTabs->SetSelection(2);

        Layout();
        Fit();
    }


    void EntityParticleSystemWindow::onParticleEmitterChanged(const std::string& key, const ungod::PSData& data)
    {
        int f = mEmitterChoice->getChoice()->FindString( mEntity.get<ungod::ParticleSystemComponent>().getSystem().getEmitterKey() );
        if (f != wxNOT_FOUND)
        {
            mEmitterChoice->getChoice()->SetSelection( f );
            updateEmitter();
        }
    }


    void EntityParticleSystemWindow::onTexrectInitChanged(const std::string& key, const ungod::PSData& data)
    {
        updateTexrectInit();
    }


    void EntityParticleSystemWindow::onAffectorsChanged(const std::string& key, const ungod::PSData& data)
    {
        updateAffectors();
    }




    UniversalEmitterPanel::UniversalEmitterPanel(ungod::Entity e, WorldActionWrapper& waw, wxWindow* parent) :
        wxPanel(parent), mPositionDistPanel(nullptr), mVelocityDistPanel(nullptr), mSpawnIntervalPanel(nullptr), mLifetimeIntervalPanel(nullptr)
    {
        SetWindowStyle(wxBORDER_SUNKEN);

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        {
            wxArrayString posdists;
            posdists.Add(ungod::PS_FIXED_POSITION);
            posdists.Add(ungod::PS_RECTANGLE_DIST);
            posdists.Add(ungod::PS_ELLIPSE_DIST);
            posdists.Add(ungod::PS_LINE_SEGMENT_DIST);
            mPosDistChoice = new LabeledChoice(this, -1, posdists, _("Position distribution: "));
            mPosDistChoice->bindChoice([this, e, &waw] (wxCommandEvent& event) { this->onPosDistChoice(event, e, waw); });

            std::string key = e.get<ungod::ParticleSystemComponent>().getSystem().getEmitter<ungod::UniversalEmitter>().getPositionDistKey();
            int f = mPosDistChoice->getChoice()->FindString( key );
            if (f != wxNOT_FOUND)
                mPosDistChoice->getChoice()->SetSelection(f);

            vbox->Add(mPosDistChoice, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP | wxBOTTOM, 8);

            if (key == ungod::PS_FIXED_POSITION)
            {
                mPositionDistPanel = new FixedPositionDistPanel(e, waw, this);
                vbox->Add(mPositionDistPanel, 0, wxALIGN_CENTER_HORIZONTAL);
            }
            else if (key == ungod::PS_ELLIPSE_DIST)
            {
                mPositionDistPanel = new EllipseDistPanel(e, waw, this);
                vbox->Add(mPositionDistPanel, 0, wxALIGN_CENTER_HORIZONTAL);
            }
            else if (key == ungod::PS_RECTANGLE_DIST)
            {
                mPositionDistPanel = new RectangleDistPanel(e, waw, this);
                vbox->Add(mPositionDistPanel, 0, wxALIGN_CENTER_HORIZONTAL);
            }
            else if (key == ungod::PS_LINE_SEGMENT_DIST)
            {
                mPositionDistPanel = new LineSegmentDistPanel(e, waw, this);
                vbox->Add(mPositionDistPanel, 0, wxALIGN_CENTER_HORIZONTAL);
            }
        }

        {
            wxArrayString veldists;
            veldists.Add(ungod::PS_FIXED_VELOCITY);
            veldists.Add(ungod::PS_CONE_DIST);
            mVelDistCoice = new LabeledChoice(this, -1, veldists, _("Velocity distribution: "));
            mVelDistCoice->bindChoice([this, e, &waw] (wxCommandEvent& event) { this->onVelDistChoice(event, e, waw); });

            std::string key = e.get<ungod::ParticleSystemComponent>().getSystem().getEmitter<ungod::UniversalEmitter>().getVelocityDistKey();
            int f = mVelDistCoice->getChoice()->FindString( key );
            if (f != wxNOT_FOUND)
                mVelDistCoice->getChoice()->SetSelection(f);

            vbox->Add(mVelDistCoice, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP | wxBOTTOM, 8);

            if (key == ungod::PS_FIXED_VELOCITY)
            {
                mVelocityDistPanel = new FixedVelocityDistPanel(e, waw, this);
                vbox->Add(mVelocityDistPanel, 0, wxALIGN_CENTER_HORIZONTAL);
            }
            else if (key == ungod::PS_CONE_DIST)
            {
                mVelocityDistPanel = new ConeDistPanel(e, waw, this);
                vbox->Add(mVelocityDistPanel, 0, wxALIGN_CENTER_HORIZONTAL);
            }
        }

        {
            wxArrayString spawnIntervals;
            spawnIntervals.Add(ungod::PS_INTERVAL_TICK);
            mSpawnIntervalChoice = new LabeledChoice(this, -1, spawnIntervals, _("Spawn interval: "));
            mSpawnIntervalChoice->bindChoice([this, e, &waw] (wxCommandEvent& event) { this->onSpawnIntervalChoice(event, e, waw); });

            std::string key = e.get<ungod::ParticleSystemComponent>().getSystem().getEmitter<ungod::UniversalEmitter>().getSpawnIntervalKey();
            int f = mSpawnIntervalChoice->getChoice()->FindString( key );
            if (f != wxNOT_FOUND)
                mSpawnIntervalChoice->getChoice()->SetSelection(f);

            vbox->Add(mSpawnIntervalChoice, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP | wxBOTTOM, 8);

            if (key == ungod::PS_INTERVAL_TICK)
            {
                mSpawnIntervalPanel = new IntervalTickPanel(e, waw, this);
                vbox->Add(mSpawnIntervalPanel, 0, wxALIGN_CENTER_HORIZONTAL);
            }
        }

        {
            wxArrayString lifetimeIntervals;
            lifetimeIntervals.Add(ungod::PS_INTERVAL_LIFETIME);
            mLifetimeIntervalChoice = new LabeledChoice(this, -1, lifetimeIntervals, _("Lifetime distribution: "));
            mLifetimeIntervalChoice->bindChoice([this, e, &waw] (wxCommandEvent& event) { this->onLifetimeIntervalChoice(event, e, waw); });

            std::string key = e.get<ungod::ParticleSystemComponent>().getSystem().getEmitter<ungod::UniversalEmitter>().getLifetimeDistKey();
            int f = mLifetimeIntervalChoice->getChoice()->FindString( key );
            if (f != wxNOT_FOUND)
                mLifetimeIntervalChoice->getChoice()->SetSelection(f);

            vbox->Add(mLifetimeIntervalChoice, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP | wxBOTTOM, 8);

            if (key == ungod::PS_INTERVAL_LIFETIME)
            {
                mLifetimeIntervalPanel = new IntervalLifetimePanel(e, waw, this);
                vbox->Add(mLifetimeIntervalPanel, 0, wxALIGN_CENTER_HORIZONTAL);
            }
        }

        SetSizer(vbox);
        Layout();
        Fit();
    }

    void UniversalEmitterPanel::onPosDistChoice(wxCommandEvent& event, ungod::Entity e, WorldActionWrapper& waw)
    {
        int sel = mPosDistChoice->getChoice()->GetSelection();
        if (sel == wxNOT_FOUND)
            return;

        std::string key {mPosDistChoice->getChoice()->GetString( sel ).mb_str()};

        //this will reset the pos dist and trigger a recreation of the universal emitter panel
        if (key == ungod::PS_FIXED_POSITION)
        {
            waw.setPositionDist<ungod::FixedPosition>(e, key, sf::Vector2f{0,0});
        }
        else if (key == ungod::PS_ELLIPSE_DIST)
        {
            waw.setPositionDist<ungod::EllipseDist>(e, key, sf::Vector2f{0,0}, sf::Vector2f{200,100});
        }
        else if (key == ungod::PS_RECTANGLE_DIST)
        {
            waw.setPositionDist<ungod::RectangleDist>(e, key, sf::Vector2f{0,0}, sf::Vector2f{100,60});
        }
        else if (key == ungod::PS_LINE_SEGMENT_DIST)
        {
            waw.setPositionDist<ungod::LineSegmentDist>(e, key, sf::Vector2f{0,0}, sf::Vector2f{60,60});
        }
    }


    void UniversalEmitterPanel::onVelDistChoice(wxCommandEvent& event, ungod::Entity e, WorldActionWrapper& waw)
    {
        int sel = mVelDistCoice->getChoice()->GetSelection();
        if (sel == wxNOT_FOUND)
            return;

        std::string key {mVelDistCoice->getChoice()->GetString( sel ).mb_str()};

        //this will reset the pos dist and trigger a recreation of the universal emitter panel
        if (key == ungod::PS_FIXED_VELOCITY)
        {
            waw.setVelocityDist<ungod::FixedVelocity>(e, key, sf::Vector2f{1,0});
        }
        else if (key == ungod::PS_CONE_DIST)
        {
            waw.setVelocityDist<ungod::ConeDist>(e, key, sf::Vector2f{-1,1}, sf::Vector2f{1,1});
        }
    }


    void UniversalEmitterPanel::onSpawnIntervalChoice(wxCommandEvent& event, ungod::Entity e, WorldActionWrapper& waw)
    {
        int sel = mSpawnIntervalChoice->getChoice()->GetSelection();
        if (sel == wxNOT_FOUND)
            return;

        std::string key {mSpawnIntervalChoice->getChoice()->GetString( sel ).mb_str()};

        //this will reset the pos dist and trigger a recreation of the universal emitter panel
        if (key == ungod::PS_INTERVAL_TICK)
        {
            waw.setSpawnInterval<ungod::IntervalTick>(e, key, 200, 500, 1);
        }
    }


    void UniversalEmitterPanel::onLifetimeIntervalChoice(wxCommandEvent& event, ungod::Entity e, WorldActionWrapper& waw)
    {
        int sel = mLifetimeIntervalChoice->getChoice()->GetSelection();
        if (sel == wxNOT_FOUND)
            return;

        std::string key {mLifetimeIntervalChoice->getChoice()->GetString( sel ).mb_str()};

        //this will reset the pos dist and trigger a recreation of the universal emitter panel
        if (key == ungod::PS_INTERVAL_LIFETIME)
        {
            waw.setLifetimeDist<ungod::IntervalLifetime>(e, key, 1500, 2000);
        }
    }



    AffectorsPanel::AffectorsPanel(ungod::Entity e, WorldActionWrapper& waw, wxWindow* parent, int sel) :
        wxPanel(parent), mEntity(e), mWaw(waw), mAffectorPanel(nullptr), mSel(sel)
    {
        const ungod::ParticleSystem& ps = e.get<ungod::ParticleSystemComponent>().getSystem();

        SetWindowStyle(wxBORDER_SUNKEN);

        mVBox = new wxBoxSizer(wxVERTICAL);

        {
            wxString c[ps.getAffectorCount()];
            for (std::size_t i = 0; i < ps.getAffectorCount(); i++)
            {
                c[i] = _(ps.getAffectorKey(i));
            }
            mList = new wxListBox(this, -1, wxDefaultPosition, wxDefaultSize, ps.getAffectorCount(), c,wxLB_SINGLE);

            mList->Bind(wxEVT_LISTBOX, [this] (wxCommandEvent& event)
                        {
                            mSel = event.GetSelection();
                            updateSelection();
                        });

            mVBox->Add(mList, 0, wxALIGN_CENTER_HORIZONTAL);
        }
        {
            wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

            mAddNew = new wxButton(this, -1, _("add"));
            mAddNew->Bind(wxEVT_BUTTON, [&waw, e, this] (wxCommandEvent& evemt)
                            {
                                bool ok = false;
                                AffectorCreateDialog::AffType type;
                                {
                                    AffectorCreateDialog dia(this);
                                    if (dia.ShowModal() == wxID_OK)
                                    {
                                        type = dia.getType();
                                        ok = true;
                                    }
                                }
                                if (ok)
                                {
                                    switch (type)
                                    {
                                        case AffectorCreateDialog::AffType::DIRECTIONAL_FORCE:
                                            waw.addAffector<ungod::DirectionalForce>(e, ungod::PS_DIRECTIONAL_FORCE, sf::Vector2f{0,1});
                                            break;
                                        case AffectorCreateDialog::AffType::DISPLACE_FORCE:
                                            waw.addAffector<ungod::DisplaceForce>(e, ungod::PS_DISPLACE_FORCE, 1.0f, 10.0f, 20.0f);
                                            break;
                                        case AffectorCreateDialog::AffType::FADE_OUT:
                                            waw.addAffector<ungod::FadeOut>(e, ungod::PS_FADE_OUT);
                                            break;
                                        case AffectorCreateDialog::AffType::ANIMATED_PARTICLES:
                                            waw.addAffector<ungod::AnimatedParticles>(e, ungod::PS_ANIMATED_PARTICLES, "", "", 0);
                                            break;
                                        case AffectorCreateDialog::AffType::COLOR_SHIFT:
                                            waw.addAffector<ungod::ColorShift>(e, ungod::PS_COLOR_SHIFT, sf::Color::White, sf::Color::Black);
                                            break;
                                        case AffectorCreateDialog::AffType::ROTATE_PARTICLES:
                                            waw.addAffector<ungod::RotateParticle>(e, ungod::PS_ROTATE_PARTICLE, 1.0f);
                                            break;
                                        case AffectorCreateDialog::AffType::SCALE_PARTICLES:
                                            waw.addAffector<ungod::ScaleParticle>(e, ungod::PS_SCALE_PARTICLE, sf::Vector2f{1.0f,1.0f}, sf::Vector2f{0.3f,0.3f});
                                            break;
                                        case AffectorCreateDialog::AffType::VELOCITY_BASED_ROTATION:
                                            waw.addAffector<ungod::VelocityBasedRotation>(e, ungod::PS_ROTATE_VELOCITY);
                                            break;
                                        default:
                                            break;
                                    }
                                }
                            });

            mRemoveCurrent = new wxButton(this, -1, _("remove"));
            mRemoveCurrent->Bind(wxEVT_BUTTON, [&waw, e] (wxCommandEvent& evemt)
                            {
                                //todo
                            });

            mClearAll = new wxButton(this, -1, _("clear all"));
            mClearAll->Bind(wxEVT_BUTTON, [&waw, e] (wxCommandEvent& evemt)
                            {
                                waw.clearAffectors(e);
                            });

            hbox->Add(mAddNew, 0, wxEXPAND);
            hbox->Add(mRemoveCurrent, 0, wxEXPAND);
            hbox->Add(mClearAll, 0, wxEXPAND);

            mVBox->Add(hbox, 0, wxALIGN_CENTER_HORIZONTAL);
        }

        if (mSel != wxNOT_FOUND)
            mList->SetSelection(mSel);

        updateSelection();

        SetSizer(mVBox);
        Layout();
        Fit();
    }


    void AffectorsPanel::updateSelection()
    {
        if (mAffectorPanel)
        {
            mVBox->Detach(mAffectorPanel);
            mAffectorPanel->Destroy();
            mAffectorPanel = nullptr;
        }

        if (mSel == wxNOT_FOUND)
        {
            mAffectorPanel = new wxPanel(this);
        }
        else
        {
            std::string activeKey = std::string{mList->GetString( mList->GetSelection() ).mb_str()};

            if (activeKey == ungod::PS_DIRECTIONAL_FORCE)
            {
                mAffectorPanel = new DirectionalForcePanel(mEntity, mWaw, mList->GetSelection(), this);
            }
            else if (activeKey == ungod::PS_DISPLACE_FORCE)
            {
                mAffectorPanel = new DisplaceForcePanel(mEntity, mWaw, mList->GetSelection(), this);
            }
            else if (activeKey == ungod::PS_FADE_OUT)
            {
                mAffectorPanel = new FadeOutPanel(mEntity, mWaw, mList->GetSelection(), this);
            }
            else if (activeKey == ungod::PS_ANIMATED_PARTICLES)
            {
                mAffectorPanel = new AnimatedParticlesPanel(mEntity, mWaw, mList->GetSelection(), this);
            }
            else if (activeKey == ungod::PS_COLOR_SHIFT)
            {
                mAffectorPanel = new ColorShiftPanel(mEntity, mWaw, mList->GetSelection(), this);
            }
            else if (activeKey == ungod::PS_ROTATE_PARTICLE)
            {
                mAffectorPanel = new RotateParticlePanel(mEntity, mWaw, mList->GetSelection(), this);
            }
            else if (activeKey == ungod::PS_SCALE_PARTICLE)
            {
                mAffectorPanel = new ScaleParticlePanel(mEntity, mWaw, mList->GetSelection(), this);
            }
            else if (activeKey == ungod::PS_ROTATE_VELOCITY)
            {
                mAffectorPanel = new VelocityBasedRotationPanel(mEntity, mWaw, mList->GetSelection(), this);
            }
        }

        mVBox->Add(mAffectorPanel, 0, wxALIGN_CENTER_HORIZONTAL);

        Layout();
        Fit();
    }


    AffectorCreateDialog::AffectorCreateDialog(wxWindow* parent)
     : wxDialog(parent, -1, _("Add affector"))
    {
        wxBoxSizer* masterSizer = new wxBoxSizer(wxVERTICAL);

        {
            wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

            mChoiceDirecForce = new wxRadioButton(this, -1, "Directional force", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
            mChoiceDisplForce = new wxRadioButton(this, -1, "Displacement force");
            mChoiceFadeOut = new wxRadioButton(this, -1, "Fade out");
            mChoiceAnimatedParticles = new wxRadioButton(this, -1, "Animated Particles");
            mChoiceColorShift = new wxRadioButton(this, -1, "Color shift");
            mChoiceRotateParticles = new wxRadioButton(this, -1, "Rotating Particles");
            mChoiceScaleParticles = new wxRadioButton(this, -1, "Scaling Particles");
            mChoiceVelocityBasedRotation = new wxRadioButton(this, -1, "Velocity based rotation");
            vbox->Add(mChoiceDirecForce, 1, wxALIGN_LEFT);
            vbox->Add(mChoiceDisplForce, 1, wxALIGN_LEFT);
            vbox->Add(mChoiceFadeOut, 1, wxALIGN_LEFT);
            vbox->Add(mChoiceAnimatedParticles, 1, wxALIGN_LEFT);
            vbox->Add(mChoiceColorShift, 1, wxALIGN_LEFT);
            vbox->Add(mChoiceRotateParticles, 1, wxALIGN_LEFT);
            vbox->Add(mChoiceScaleParticles, 1, wxALIGN_LEFT);
            vbox->Add(mChoiceVelocityBasedRotation, 1, wxALIGN_LEFT);

            masterSizer->Add(vbox, 1, wxEXPAND);
        }

        {
            wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
            wxButton* addButton = new wxButton( this, -1, _("Add affector"));
            addButton->Bind(wxEVT_BUTTON, [this] (wxCommandEvent& event)
                            {
                                onOk(event);
                            });
            hbox->Add(addButton, 1, wxCENTER);

            wxButton* cancelButton = new wxButton( this, -1, _("Cancel"));
            cancelButton->Bind(wxEVT_BUTTON, [this] (wxCommandEvent& event)
                            {
                                EndModal(wxID_CANCEL);
                            });
            hbox->Add(cancelButton, 1, wxCENTER);

            masterSizer->Add(hbox, 1, wxEXPAND);
        }

        SetSizer(masterSizer);
        Fit();
        Center();
    }


    void AffectorCreateDialog::onOk( wxCommandEvent & event )
    {
        if (mChoiceDirecForce->GetValue())
        {
            mType = DIRECTIONAL_FORCE;
        }
        else if (mChoiceDisplForce->GetValue())
        {
            mType = DISPLACE_FORCE;
        }
        else if (mChoiceFadeOut->GetValue())
        {
            mType = FADE_OUT;
        }
        else if (mChoiceAnimatedParticles->GetValue())
        {
            mType = ANIMATED_PARTICLES;
        }
        else if (mChoiceColorShift->GetValue())
        {
            mType = COLOR_SHIFT;
        }
        else if (mChoiceRotateParticles->GetValue())
        {
            mType = ROTATE_PARTICLES;
        }
        else if (mChoiceScaleParticles->GetValue())
        {
            mType = SCALE_PARTICLES;
        }
        else
        {
            mType = VELOCITY_BASED_ROTATION;
        }

        EndModal(wxID_OK);
    }


    DirectionalForcePanel::DirectionalForcePanel(ungod::Entity e, WorldActionWrapper& waw, std::size_t i, wxWindow* parent)
        : wxPanel(parent)
    {
        SetWindowStyle(wxBORDER_SUNKEN);

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        {
            auto* displ = new StatDisplay<float>("force x:", this, -1);
            displ->connectSetter( [this, e, &waw, i](float x)
            {
                waw.alterAffector<ungod::DirectionalForce>(e, i, [x] (ungod::ScopedAccessor<ungod::DirectionalForce>& f)
                                               {
                                                    f->force.x = x;
                                               });
            } );
            displ->connectGetter( [this, e, i]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().getAffector<ungod::DirectionalForce>(i).force.x;
            } );
            displ->refreshValue();

            vbox->Add(displ,0,wxALIGN_CENTER_HORIZONTAL);
        }
        {
            auto* displ = new StatDisplay<float>("force y:", this, -1);
            displ->connectSetter( [this, e, &waw, i](float y)
            {
                waw.alterAffector<ungod::DirectionalForce>(e, i, [y] (ungod::ScopedAccessor<ungod::DirectionalForce>& f)
                                               {
                                                    f->force.y = y;
                                               });
            } );
            displ->connectGetter( [this, e, i]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().getAffector<ungod::DirectionalForce>(i).force.y;
            } );
            displ->refreshValue();

            vbox->Add(displ,0,wxALIGN_CENTER_HORIZONTAL);
        }

        SetSizer(vbox);
        Layout();
        Fit();
    }


    DisplaceForcePanel::DisplaceForcePanel(ungod::Entity e, WorldActionWrapper& waw, std::size_t i, wxWindow* parent)
        : wxPanel(parent)
    {
        SetWindowStyle(wxBORDER_SUNKEN);

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        {
            auto* displ = new StatDisplay<float>("speed:", this, -1);
            displ->connectSetter( [this, e, &waw, i](float y)
            {
                waw.alterAffector<ungod::DisplaceForce>(e, i, [y] (ungod::ScopedAccessor<ungod::DisplaceForce>& f)
                                               {
                                                    f->speed = y;
                                               });
            } );
            displ->connectGetter( [this, e, i]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().getAffector<ungod::DisplaceForce>(i).speed;
            } );
            displ->refreshValue();

            vbox->Add(displ,0,wxALIGN_CENTER_HORIZONTAL);
        }
        {
            auto* displ = new StatDisplay<float>("circle:", this, -1);
            displ->connectSetter( [this, e, &waw, i](float y)
            {
                waw.alterAffector<ungod::DisplaceForce>(e, i, [y] (ungod::ScopedAccessor<ungod::DisplaceForce>& f)
                                               {
                                                    f->circle = y;
                                               });
            } );
            displ->connectGetter( [this, e, i]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().getAffector<ungod::DisplaceForce>(i).circle;
            } );
            displ->refreshValue();

            vbox->Add(displ,0,wxALIGN_CENTER_HORIZONTAL);
        }
        {
            auto* displ = new StatDisplay<float>("angle:", this, -1);
            displ->connectSetter( [this, e, &waw, i](float y)
            {
                waw.alterAffector<ungod::DisplaceForce>(e, i, [y] (ungod::ScopedAccessor<ungod::DisplaceForce>& f)
                                               {
                                                    f->angle = y;
                                               });
            } );
            displ->connectGetter( [this, e, i]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().getAffector<ungod::DisplaceForce>(i).angle;
            } );
            displ->refreshValue();

            vbox->Add(displ,0,wxALIGN_CENTER_HORIZONTAL);
        }

        SetSizer(vbox);
        Layout();
        Fit();
    }


    FadeOutPanel::FadeOutPanel(ungod::Entity e, WorldActionWrapper& waw, std::size_t i, wxWindow* parent)
        : wxPanel(parent)
    {
        SetWindowStyle(wxBORDER_SUNKEN);

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        {
            auto* displ = new StatDisplay<float>("speed [currently not implemented]:", this, -1);
            displ->connectSetter( [this, e, &waw, i](float y)
            {
                /*waw.alterAffector<ungod::DisplaceForce>(e, i, [y] (ungod::ScopedAccessor<ungod::DisplaceForce>& f)
                                               {
                                                    f->angle = y;
                                               });*/
            } );
            displ->connectGetter( [this, e, i]()
            {
                return 0.0f; //e.get<ungod::ParticleSystemComponent>().getSystem().getAffector<ungod::DisplaceForce>(i).angle;
            } );
            displ->refreshValue();

            vbox->Add(displ,0,wxALIGN_CENTER_HORIZONTAL);
        }

        SetSizer(vbox);
        Layout();
        Fit();
    }


    AnimatedParticlesPanel::AnimatedParticlesPanel(ungod::Entity e, WorldActionWrapper& waw, std::size_t i, wxWindow* parent)
        : wxPanel(parent)
    {
        SetWindowStyle(wxBORDER_SUNKEN);

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);


        {
            auto* but = new wxButton(this, -1, "set selected");
            but->Bind(wxEVT_BUTTON, [e, i, &waw] (wxCommandEvent& event)
                      {
                        waw.alterAffector<ungod::AnimatedParticles>(e, i, [&waw] (ungod::ScopedAccessor<ungod::AnimatedParticles>& f)
                                                       {
                                                            f->init(waw.getEditorFrame()->getSheetPreview()->getCurrentMeta(),
                                                                    waw.getEditorFrame()->getSheetPreview()->getCurrentKey(),
                                                                    f->animations.size());
                                                       });
                      });
        }
        {
            auto* displ = new StatDisplay<int>("num anim:", this, -1);
            displ->connectSetter( [this, e, &waw, i](float y)
            {
                waw.alterAffector<ungod::AnimatedParticles>(e, i, [y] (ungod::ScopedAccessor<ungod::AnimatedParticles>& f)
                                               {
                                                    f->init(f->meta.getFilePath(), f->key, y);
                                               });
            } );
            displ->connectGetter( [this, e, i]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().getAffector<ungod::AnimatedParticles>(i).animations.size();
            } );
            displ->refreshValue();

            vbox->Add(displ,0,wxALIGN_CENTER_HORIZONTAL);
        }

        SetSizer(vbox);
        Layout();
        Fit();
    }


    ColorShiftPanel::ColorShiftPanel(ungod::Entity e, WorldActionWrapper& waw, std::size_t i, wxWindow* parent)
        : wxPanel(parent)
    {
        SetWindowStyle(wxBORDER_SUNKEN);

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        const ungod::ParticleSystem& ps = e.get<ungod::ParticleSystemComponent>().getSystem();

        {
            wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
            hbox->Add(new wxStaticText(this, -1, "start color: "), 2, wxEXPAND);

            auto* colpick = new wxColourPickerCtrl(this, -1, convertColor( ps.getAffector<ungod::ColorShift>(i).colorBegin ));
            colpick->Bind(wxEVT_COLOURPICKER_CHANGED, [e, &waw, i] (wxColourPickerEvent& event)
              {
                waw.alterAffector<ungod::ColorShift>(e, i, [&event] (ungod::ScopedAccessor<ungod::ColorShift>& f)
                                               {
                                                    f->colorBegin = convertColor( event.GetColour() );
                                               });
              });
            hbox->Add(colpick, 2, wxEXPAND);

            vbox->Add(hbox, 0, wxALIGN_CENTER_HORIZONTAL);
        }


        {
            wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
            hbox->Add(new wxStaticText(this, -1, "end color: "), 2, wxEXPAND);

            auto* colpick = new wxColourPickerCtrl(this, -1, convertColor( ps.getAffector<ungod::ColorShift>(i).colorEnd ));
            colpick->Bind(wxEVT_COLOURPICKER_CHANGED, [e, &waw, i] (wxColourPickerEvent& event)
              {
                waw.alterAffector<ungod::ColorShift>(e, i, [&event] (ungod::ScopedAccessor<ungod::ColorShift>& f)
                                               {
                                                    f->colorEnd = convertColor( event.GetColour() );
                                               });
              });
            hbox->Add(colpick, 2, wxEXPAND);

            vbox->Add(hbox, 0, wxALIGN_CENTER_HORIZONTAL);
        }

        SetSizer(vbox);
        Layout();
        Fit();
    }


    RotateParticlePanel::RotateParticlePanel(ungod::Entity e, WorldActionWrapper& waw, std::size_t i, wxWindow* parent)
        : wxPanel(parent)
    {
        SetWindowStyle(wxBORDER_SUNKEN);

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        {
            auto* displ = new StatDisplay<float>("speed:", this, -1);
            displ->connectSetter( [this, e, &waw, i](float y)
            {
                waw.alterAffector<ungod::RotateParticle>(e, i, [y] (ungod::ScopedAccessor<ungod::RotateParticle>& f)
                                               {
                                                    f->speed = y;
                                               });
            } );
            displ->connectGetter( [this, e, i]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().getAffector<ungod::RotateParticle>(i).speed;
            } );
            displ->refreshValue();

            vbox->Add(displ,0,wxALIGN_CENTER_HORIZONTAL);
        }

        SetSizer(vbox);
        Layout();
        Fit();
    }


    ScaleParticlePanel::ScaleParticlePanel(ungod::Entity e, WorldActionWrapper& waw, std::size_t i, wxWindow* parent)
        : wxPanel(parent)
    {
        SetWindowStyle(wxBORDER_SUNKEN);

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);


        {
            auto* displ = new StatDisplay<float>("scale begin x:", this, -1);
            displ->connectSetter( [this, e, &waw, i](float x)
            {
                waw.alterAffector<ungod::ScaleParticle>(e, i, [x] (ungod::ScopedAccessor<ungod::ScaleParticle>& f)
                                               {
                                                    f->scalesBegin.x = x;
                                               });
            } );
            displ->connectGetter( [this, e, i]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().getAffector<ungod::ScaleParticle>(i).scalesBegin.x;
            } );
            displ->refreshValue();

            vbox->Add(displ,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            auto* displ = new StatDisplay<float>("scale begin y:", this, -1);
            displ->connectSetter( [this, e, &waw, i](float y)
            {
                waw.alterAffector<ungod::ScaleParticle>(e, i, [y] (ungod::ScopedAccessor<ungod::ScaleParticle>& f)
                                               {
                                                    f->scalesBegin.y = y;
                                               });
            } );
            displ->connectGetter( [this, e, i]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().getAffector<ungod::ScaleParticle>(i).scalesBegin.y;
            } );
            displ->refreshValue();

            vbox->Add(displ,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            auto* displ = new StatDisplay<float>("scale end x:", this, -1);
            displ->connectSetter( [this, e, &waw, i](float x)
            {
                waw.alterAffector<ungod::ScaleParticle>(e, i, [x] (ungod::ScopedAccessor<ungod::ScaleParticle>& f)
                                               {
                                                    f->scalesEnd.x = x;
                                               });
            } );
            displ->connectGetter( [this, e, i]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().getAffector<ungod::ScaleParticle>(i).scalesEnd.x;
            } );
            displ->refreshValue();

            vbox->Add(displ,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            auto* displ = new StatDisplay<float>("scale end y:", this, -1);
            displ->connectSetter( [this, e, &waw, i](float y)
            {
                waw.alterAffector<ungod::ScaleParticle>(e, i, [y] (ungod::ScopedAccessor<ungod::ScaleParticle>& f)
                                               {
                                                    f->scalesEnd.y = y;
                                               });
            } );
            displ->connectGetter( [this, e, i]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().getAffector<ungod::ScaleParticle>(i).scalesEnd.y;
            } );
            displ->refreshValue();

            vbox->Add(displ,0,wxALIGN_CENTER_HORIZONTAL);
        }

        SetSizer(vbox);
        Layout();
        Fit();
    }


    VelocityBasedRotationPanel::VelocityBasedRotationPanel(ungod::Entity e, WorldActionWrapper& waw, std::size_t i, wxWindow* parent)
        : wxPanel(parent)
    {
        SetWindowStyle(wxBORDER_SUNKEN);

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        SetSizer(vbox);
        Layout();
        Fit();
    }




    TexrectInitailizerPanel::TexrectInitailizerPanel(ungod::Entity e, WorldActionWrapper& waw, wxWindow* parent) :
        wxPanel(parent), mTexrectInitPanel(nullptr)
    {
        SetWindowStyle(wxBORDER_SUNKEN);

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        {
            wxArrayString texrectInitializers;
            texrectInitializers.Add(ungod::PS_EXPLICIT_TEXRECT);
            texrectInitializers.Add(ungod::PS_TEXRECT_BY_KEY);
            mTexrectInitChoice = new LabeledChoice(this, -1, texrectInitializers, _("texrect initializers: "));
            mTexrectInitChoice->bindChoice([this, e, &waw] (wxCommandEvent& event) { this->onTexrectInitChoice(event, e, waw); });

            std::string key = e.get<ungod::ParticleSystemComponent>().getSystem().getTexrectInitializerKey();
            int f = mTexrectInitChoice->getChoice()->FindString( key );
            if (f != wxNOT_FOUND)
                mTexrectInitChoice->getChoice()->SetSelection(f);

            vbox->Add(mTexrectInitChoice, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP | wxBOTTOM, 8);

            if (key == ungod::PS_TEXRECT_BY_KEY)
            {
                mTexrectInitPanel = new TexrectByKeyPanel(e, waw, this);
                vbox->Add(mTexrectInitPanel, 0, wxALIGN_CENTER_HORIZONTAL);
            }
            else if (key == ungod::PS_EXPLICIT_TEXRECT )
            {
                mTexrectInitPanel = new ExplicitTexrectPanel(e, waw, this);
                vbox->Add(mTexrectInitPanel, 0, wxALIGN_CENTER_HORIZONTAL);
            }
        }

        SetSizer(vbox);
        Layout();
        Fit();
    }


    void TexrectInitailizerPanel::onTexrectInitChoice(wxCommandEvent& event, ungod::Entity e, WorldActionWrapper& waw)
    {
        int sel = mTexrectInitChoice->getChoice()->GetSelection();
        if (sel == wxNOT_FOUND)
            return;

        std::string key {mTexrectInitChoice->getChoice()->GetString( sel ).mb_str()};

        //this will reset the pos dist and trigger a recreation of the universal emitter panel
        if (key == ungod::PS_EXPLICIT_TEXRECT)
        {
            waw.setTexrectInitializer<ungod::ExplicitTexrect>(e, key, sf::IntRect{0,0,64,64});
        }
        else if (key == ungod::PS_TEXRECT_BY_KEY)
        {
            waw.setTexrectInitializer<ungod::TexrectByKey>(e, key, "", "");
        }
    }


    ExplicitTexrectPanel::ExplicitTexrectPanel(ungod::Entity e, WorldActionWrapper& waw, wxWindow* parent) : wxPanel(parent)
    {
        SetWindowStyle(wxBORDER_SIMPLE);

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);


        {
            mLeft = new StatDisplay<int>("left:", this, -1);
            mLeft->connectSetter( [this, e, &waw](int l)
            {
                waw.alterTexrectInitializer<ungod::ExplicitTexrect>(e, [l] (ungod::ScopedAccessor<ungod::ExplicitTexrect>& f)
                                               {
                                                    f->rect.left = l;
                                               });
            } );
            mLeft->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().getTexrectInitializer<ungod::ExplicitTexrect>().rect.left;
            } );

            vbox->Add(mLeft,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mTop = new StatDisplay<int>("top:", this, -1);
            mTop->connectSetter( [this, e, &waw](int t)
            {
                waw.alterTexrectInitializer<ungod::ExplicitTexrect>(e, [t] (ungod::ScopedAccessor<ungod::ExplicitTexrect>& f)
                                               {
                                                    f->rect.top = t;
                                               });
            } );
            mTop->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().getTexrectInitializer<ungod::ExplicitTexrect>().rect.top;
            } );

            vbox->Add(mTop,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mWidth = new StatDisplay<int>("width:", this, -1);
            mWidth->connectSetter( [this, e, &waw](int w)
            {
                waw.alterTexrectInitializer<ungod::ExplicitTexrect>(e, [w] (ungod::ScopedAccessor<ungod::ExplicitTexrect>& f)
                                               {
                                                    f->rect.width = w;
                                               });
            } );
            mWidth->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().getTexrectInitializer<ungod::ExplicitTexrect>().rect.width;
            } );

            vbox->Add(mWidth,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mHeight = new StatDisplay<int>("height:", this, -1);
            mHeight->connectSetter( [this, e, &waw](int h)
            {
                waw.alterTexrectInitializer<ungod::ExplicitTexrect>(e, [h] (ungod::ScopedAccessor<ungod::ExplicitTexrect>& f)
                                               {
                                                    f->rect.height = h;
                                               });
            } );
            mHeight->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().getTexrectInitializer<ungod::ExplicitTexrect>().rect.height;
            } );

            vbox->Add(mHeight,0,wxALIGN_CENTER_HORIZONTAL);
        }

        mLeft->refreshValue();
        mTop->refreshValue();
        mWidth->refreshValue();
        mHeight->refreshValue();

        SetSizer(vbox);
        Layout();
        Fit();
    }


    TexrectByKeyPanel::TexrectByKeyPanel(ungod::Entity e, WorldActionWrapper& waw, wxWindow* parent) : wxPanel(parent)
    {
        SetWindowStyle(wxBORDER_SIMPLE);

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        mSetSelected = new wxButton(this, -1, _("set selected"));
        mSetSelected->Bind(wxEVT_BUTTON, [e, &waw] (wxCommandEvent& event)
            {
                std::string meta = waw.getEditorFrame()->getSheetPreview()->getCurrentMeta();
                std::string key = waw.getEditorFrame()->getSheetPreview()->getCurrentKey();
                waw.alterTexrectInitializer<ungod::TexrectByKey>(e, [meta, key] (ungod::ScopedAccessor<ungod::TexrectByKey>& a)
                     {
                        a->init(meta, key);
                     });
            });
        vbox->Add(mSetSelected,0,wxALIGN_CENTER_HORIZONTAL);

        SetSizer(vbox);
        Layout();
        Fit();
    }



    FixedPositionDistPanel::FixedPositionDistPanel(ungod::Entity e, WorldActionWrapper& waw, wxWindow* parent) : wxPanel(parent)
    {
        SetWindowStyle(wxBORDER_SIMPLE);

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);


        {
            mPosX = new StatDisplay<float>("emit pos x:", this, -1);
            mPosX->connectSetter( [this, e, &waw](float x)
            {
                waw.alterPositionDist<ungod::FixedPosition>(e, [x] (ungod::ScopedAccessor<ungod::FixedPosition>& f)
                                               {
                                                    f->position.x = x;
                                               });
            } );
            mPosX->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().
                                getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::FixedPosition>().position.x;
            } );

            vbox->Add(mPosX,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mPosY = new StatDisplay<float>("emit pos y:", this, -1);
            mPosY->connectSetter( [this, e, &waw](float y)
            {
                waw.alterPositionDist<ungod::FixedPosition>(e, [y] (ungod::ScopedAccessor<ungod::FixedPosition>& f)
                                               {
                                                    f->position.y = y;
                                               });
            } );
            mPosY->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().
                                getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::FixedPosition>().position.y;
            } );

            vbox->Add(mPosY,0,wxALIGN_CENTER_HORIZONTAL);
        }

        mPosX->refreshValue();
        mPosY->refreshValue();

        SetSizer(vbox);
        Layout();
        Fit();
    }

    EllipseDistPanel::EllipseDistPanel(ungod::Entity e, WorldActionWrapper& waw, wxWindow* parent) : wxPanel(parent)
    {
        SetWindowStyle(wxBORDER_SIMPLE);

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);


        {
            mCenterX = new StatDisplay<float>("center x:", this, -1);
            mCenterX->connectSetter( [this, e, &waw](float x)
            {
                waw.alterPositionDist<ungod::EllipseDist>(e, [x] (ungod::ScopedAccessor<ungod::EllipseDist>& f)
                                               {
                                                    f->center.x = x;
                                               });
            } );
            mCenterX->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().
                                getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::EllipseDist>().center.x;
            } );

            vbox->Add(mCenterX,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mCenterY = new StatDisplay<float>("center y:", this, -1);
            mCenterY->connectSetter( [this, e, &waw](float y)
            {
                waw.alterPositionDist<ungod::EllipseDist>(e, [y] (ungod::ScopedAccessor<ungod::EllipseDist>& f)
                                               {
                                                    f->center.y = y;
                                               });
            } );
            mCenterY->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().
                                getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::EllipseDist>().center.y;
            } );

            vbox->Add(mCenterY,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mRadiusX = new StatDisplay<float>("radius x:", this, -1);
            mRadiusX->connectSetter( [this, e, &waw](float x)
            {
                waw.alterPositionDist<ungod::EllipseDist>(e, [x] (ungod::ScopedAccessor<ungod::EllipseDist>& f)
                                               {
                                                    f->radius.x = x;
                                               });
            } );
            mRadiusX->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().
                                getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::EllipseDist>().radius.x;
            } );

            vbox->Add(mRadiusX,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mRadiusY = new StatDisplay<float>("radius y:", this, -1);
            mRadiusY->connectSetter( [this, e, &waw](float y)
            {
                waw.alterPositionDist<ungod::EllipseDist>(e, [y] (ungod::ScopedAccessor<ungod::EllipseDist>& f)
                                               {
                                                    f->radius.y = y;
                                               });
            } );
            mRadiusY->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().
                                getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::EllipseDist>().radius.y;
            } );

            vbox->Add(mRadiusY,0,wxALIGN_CENTER_HORIZONTAL);
        }

        mCenterX->refreshValue();
        mCenterY->refreshValue();
        mRadiusX->refreshValue();
        mRadiusY->refreshValue();

        SetSizer(vbox);
        Layout();
        Fit();
    }


    RectangleDistPanel::RectangleDistPanel(ungod::Entity e, WorldActionWrapper& waw, wxWindow* parent) : wxPanel(parent)
    {
        SetWindowStyle(wxBORDER_SIMPLE);

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);


        {
            mTopleftX = new StatDisplay<float>("topleft x:", this, -1);
            mTopleftX->connectSetter( [this, e, &waw](float x)
            {
                waw.alterPositionDist<ungod::RectangleDist>(e, [x] (ungod::ScopedAccessor<ungod::RectangleDist>& f)
                                               {
                                                    f->topleft.x = x;
                                               });
            } );
            mTopleftX->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().
                                getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::RectangleDist>().topleft.x;
            } );

            vbox->Add(mTopleftX,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mTopleftY = new StatDisplay<float>("topleft y:", this, -1);
            mTopleftY->connectSetter( [this, e, &waw](float y)
            {
                waw.alterPositionDist<ungod::RectangleDist>(e, [y] (ungod::ScopedAccessor<ungod::RectangleDist>& f)
                                               {
                                                    f->topleft.y = y;
                                               });
            } );
            mTopleftY->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().
                                getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::RectangleDist>().topleft.y;
            } );

            vbox->Add(mTopleftY,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mSizeX = new StatDisplay<float>("rect width:", this, -1);
            mSizeX->connectSetter( [this, e, &waw](float x)
            {
                waw.alterPositionDist<ungod::RectangleDist>(e, [x] (ungod::ScopedAccessor<ungod::RectangleDist>& f)
                                               {
                                                    f->sizes.x = x;
                                               });
            } );
            mSizeX->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().
                                getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::RectangleDist>().sizes.x;
            } );

            vbox->Add(mSizeX,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mSizeY = new StatDisplay<float>("rect height:", this, -1);
            mSizeY->connectSetter( [this, e, &waw](float y)
            {
                waw.alterPositionDist<ungod::RectangleDist>(e, [y] (ungod::ScopedAccessor<ungod::RectangleDist>& f)
                                               {
                                                    f->sizes.y = y;
                                               });
            } );
            mSizeY->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().
                                getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::RectangleDist>().sizes.y;
            } );

            vbox->Add(mSizeY,0,wxALIGN_CENTER_HORIZONTAL);
        }

        mTopleftX->refreshValue();
        mTopleftY->refreshValue();
        mSizeX->refreshValue();
        mSizeY->refreshValue();

        SetSizer(vbox);
        Layout();
        Fit();
    }


    LineSegmentDistPanel::LineSegmentDistPanel(ungod::Entity e, WorldActionWrapper& waw, wxWindow* parent) : wxPanel(parent)
    {
        SetWindowStyle(wxBORDER_SIMPLE);

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);


        {
            mPoint1X = new StatDisplay<float>("point 1 x:", this, -1);
            mPoint1X->connectSetter( [this, e, &waw](float x)
            {
                waw.alterPositionDist<ungod::LineSegmentDist>(e, [x] (ungod::ScopedAccessor<ungod::LineSegmentDist>& f)
                                               {
                                                    f->point1.x = x;
                                               });
            } );
            mPoint1X->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().
                                getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::LineSegmentDist>().point1.x;
            } );

            vbox->Add(mPoint1X,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mPoint1Y = new StatDisplay<float>("point 1 y:", this, -1);
            mPoint1Y->connectSetter( [this, e, &waw](float y)
            {
                waw.alterPositionDist<ungod::LineSegmentDist>(e, [y] (ungod::ScopedAccessor<ungod::LineSegmentDist>& f)
                                               {
                                                    f->point1.y = y;
                                               });
            } );
            mPoint1Y->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().
                                getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::LineSegmentDist>().point1.y;
            } );

            vbox->Add(mPoint1Y,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mPoint2X = new StatDisplay<float>("point 2 x:", this, -1);
            mPoint2X->connectSetter( [this, e, &waw](float x)
            {
                waw.alterPositionDist<ungod::LineSegmentDist>(e, [x] (ungod::ScopedAccessor<ungod::LineSegmentDist>& f)
                                               {
                                                    f->point2.x = x;
                                               });
            } );
            mPoint2X->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().
                                getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::LineSegmentDist>().point2.x;
            } );

            vbox->Add(mPoint2X,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mPoint2Y = new StatDisplay<float>("point 2 y:", this, -1);
            mPoint2Y->connectSetter( [this, e, &waw](float y)
            {
                waw.alterPositionDist<ungod::LineSegmentDist>(e, [y] (ungod::ScopedAccessor<ungod::LineSegmentDist>& f)
                                               {
                                                    f->point2.y = y;
                                               });
            } );
            mPoint2Y->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().
                                getEmitter<ungod::UniversalEmitter>().getPositionDist<ungod::LineSegmentDist>().point2.y;
            } );

            vbox->Add(mPoint2Y,0,wxALIGN_CENTER_HORIZONTAL);
        }

        mPoint1X->refreshValue();
        mPoint1Y->refreshValue();
        mPoint2X->refreshValue();
        mPoint2Y->refreshValue();

        SetSizer(vbox);
        Layout();
        Fit();
    }




    FixedVelocityDistPanel::FixedVelocityDistPanel(ungod::Entity e, WorldActionWrapper& waw, wxWindow* parent) : wxPanel(parent)
    {
        SetWindowStyle(wxBORDER_SIMPLE);

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);


        {
            mVelX = new StatDisplay<float>("velocity x:", this, -1);
            mVelX->connectSetter( [this, e, &waw](float x)
            {
                waw.alterVelocityDist<ungod::FixedVelocity>(e, [x] (ungod::ScopedAccessor<ungod::FixedVelocity>& f)
                                               {
                                                    f->velocity.x = x;
                                               });
            } );
            mVelX->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().
                                getEmitter<ungod::UniversalEmitter>().getVelocityDist<ungod::FixedVelocity>().velocity.x;
            } );

            vbox->Add(mVelX,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mVelY = new StatDisplay<float>("velocity y:", this, -1);
            mVelY->connectSetter( [this, e, &waw](float y)
            {
                waw.alterVelocityDist<ungod::FixedVelocity>(e, [y] (ungod::ScopedAccessor<ungod::FixedVelocity>& f)
                                               {
                                                    f->velocity.y = y;
                                               });
            } );
            mVelY->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().
                                getEmitter<ungod::UniversalEmitter>().getVelocityDist<ungod::FixedVelocity>().velocity.y;
            } );

            vbox->Add(mVelY,0,wxALIGN_CENTER_HORIZONTAL);
        }

        mVelX->refreshValue();
        mVelY->refreshValue();

        SetSizer(vbox);
        Layout();
        Fit();
    }



    ConeDistPanel::ConeDistPanel(ungod::Entity e, WorldActionWrapper& waw, wxWindow* parent) : wxPanel(parent)
    {
        SetWindowStyle(wxBORDER_SIMPLE);

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);


        {
            mDirection1X = new StatDisplay<float>("direction 1 x:", this, -1);
            mDirection1X->connectSetter( [this, e, &waw](float x)
            {
                waw.alterVelocityDist<ungod::ConeDist>(e, [x] (ungod::ScopedAccessor<ungod::ConeDist>& f)
                                               {
                                                    f->direction1.x = x;
                                               });
            } );
            mDirection1X->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().
                                getEmitter<ungod::UniversalEmitter>().getVelocityDist<ungod::ConeDist>().direction1.x;
            } );

            vbox->Add(mDirection1X,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mDirection1Y = new StatDisplay<float>("direction 1 y:", this, -1);
            mDirection1Y->connectSetter( [this, e, &waw](float y)
            {
                waw.alterVelocityDist<ungod::ConeDist>(e, [y] (ungod::ScopedAccessor<ungod::ConeDist>& f)
                                               {
                                                    f->direction1.y = y;
                                               });
            } );
            mDirection1Y->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().
                                getEmitter<ungod::UniversalEmitter>().getVelocityDist<ungod::ConeDist>().direction1.y;
            } );

            vbox->Add(mDirection1Y,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mDirection2X = new StatDisplay<float>("direction 2 x:", this, -1);
            mDirection2X->connectSetter( [this, e, &waw](float x)
            {
                waw.alterVelocityDist<ungod::ConeDist>(e, [x] (ungod::ScopedAccessor<ungod::ConeDist>& f)
                                               {
                                                    f->direction2.x = x;
                                               });
            } );
            mDirection2X->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().
                                getEmitter<ungod::UniversalEmitter>().getVelocityDist<ungod::ConeDist>().direction2.x;
            } );

            vbox->Add(mDirection2X,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mDirection2Y = new StatDisplay<float>("direction 2 y:", this, -1);
            mDirection2Y->connectSetter( [this, e, &waw](float y)
            {
                waw.alterVelocityDist<ungod::ConeDist>(e, [y] (ungod::ScopedAccessor<ungod::ConeDist>& f)
                                               {
                                                    f->direction2.y = y;
                                               });
            } );
            mDirection2Y->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().
                                getEmitter<ungod::UniversalEmitter>().getVelocityDist<ungod::ConeDist>().direction2.y;
            } );

            vbox->Add(mDirection2Y,0,wxALIGN_CENTER_HORIZONTAL);
        }

        mDirection1X->refreshValue();
        mDirection1Y->refreshValue();
        mDirection2X->refreshValue();
        mDirection2Y->refreshValue();

        SetSizer(vbox);
        Layout();
        Fit();
    }


    IntervalTickPanel::IntervalTickPanel(ungod::Entity e, WorldActionWrapper& waw, wxWindow* parent) : wxPanel(parent)
    {
        SetWindowStyle(wxBORDER_SIMPLE);

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);


        {
            mMin = new StatDisplay<float>("min tick:", this, -1);
            mMin->connectSetter( [this, e, &waw](float x)
            {
                waw.alterSpawnInterval<ungod::IntervalTick>(e, [x] (ungod::ScopedAccessor<ungod::IntervalTick>& f)
                                               {
                                                    f->msmin = x;
                                               });
            } );
            mMin->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().
                                getEmitter<ungod::UniversalEmitter>().getSpawnInterval<ungod::IntervalTick>().msmin;
            } );

            vbox->Add(mMin,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mMax = new StatDisplay<float>("max tick:", this, -1);
            mMax->connectSetter( [this, e, &waw](float x)
            {
                waw.alterSpawnInterval<ungod::IntervalTick>(e, [x] (ungod::ScopedAccessor<ungod::IntervalTick>& f)
                                               {
                                                    f->msmax = x;
                                               });
            } );
            mMax->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().
                                getEmitter<ungod::UniversalEmitter>().getSpawnInterval<ungod::IntervalTick>().msmax;
            } );

            vbox->Add(mMax,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mNum = new StatDisplay<int>("spawn count:", this, -1);
            mNum->connectSetter( [this, e, &waw](int n)
            {
                waw.alterSpawnInterval<ungod::IntervalTick>(e, [n] (ungod::ScopedAccessor<ungod::IntervalTick>& f)
                                               {
                                                    f->numparticle = n;
                                               });
            } );
            mNum->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().
                                getEmitter<ungod::UniversalEmitter>().getSpawnInterval<ungod::IntervalTick>().numparticle;
            } );

            vbox->Add(mNum,0,wxALIGN_CENTER_HORIZONTAL);
        }

        mMin->refreshValue();
        mMax->refreshValue();
        mNum->refreshValue();

        SetSizer(vbox);
        Layout();
        Fit();
    }



    IntervalLifetimePanel::IntervalLifetimePanel(ungod::Entity e, WorldActionWrapper& waw, wxWindow* parent) : wxPanel(parent)
    {
        SetWindowStyle(wxBORDER_SIMPLE);

        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);


        {
            mMin = new StatDisplay<float>("min lifetime:", this, -1);
            mMin->connectSetter( [this, e, &waw](float x)
            {
                waw.alterLifetimeDist<ungod::IntervalLifetime>(e, [x] (ungod::ScopedAccessor<ungod::IntervalLifetime>& f)
                                               {
                                                    f->msmin = x;
                                               });
            } );
            mMin->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().
                                getEmitter<ungod::UniversalEmitter>().getLifetimeDist<ungod::IntervalLifetime>().msmin;
            } );

            vbox->Add(mMin,0,wxALIGN_CENTER_HORIZONTAL);
        }

        {
            mMax = new StatDisplay<float>("max lifetime:", this, -1);
            mMax->connectSetter( [this, e, &waw](float x)
            {
                waw.alterLifetimeDist<ungod::IntervalLifetime>(e, [x] (ungod::ScopedAccessor<ungod::IntervalLifetime>& f)
                                               {
                                                    f->msmax = x;
                                               });
            } );
            mMax->connectGetter( [this, e]()
            {
                return e.get<ungod::ParticleSystemComponent>().getSystem().
                                getEmitter<ungod::UniversalEmitter>().getLifetimeDist<ungod::IntervalLifetime>().msmax;
            } );

            vbox->Add(mMax,0,wxALIGN_CENTER_HORIZONTAL);
        }

        mMin->refreshValue();
        mMax->refreshValue();

        SetSizer(vbox);
        Layout();
        Fit();
    }
}
