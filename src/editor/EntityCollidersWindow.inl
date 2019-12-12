#ifndef UEDIT_ENTITY_COLLIDERS_WINDOW_INL
#define UEDIT_ENTITY_COLLIDERS_WINDOW_INL

namespace uedit
{
    template<std::size_t CONTEXT>
    EntityCollidersWindow<CONTEXT>::EntityCollidersWindow(ungod::Entity e, WorldActionWrapper& waw, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& siz)
        : wxWindow(parent, id, pos, siz), mEntity(e), mWaw(waw), mSelection(0), mColliderDetail(nullptr)
    {
        wxBoxSizer* boxsizer = new wxBoxSizer(wxVERTICAL);

        mRigidbodies = new wxListCtrl(this, -1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
        wxListItem bodyID;
        bodyID.SetId(0);
        bodyID.SetText(_("ID"));
        bodyID.SetWidth(60);
        mRigidbodies->InsertColumn(0, bodyID);
        mRigidbodies->Bind(wxEVT_LIST_ITEM_ACTIVATED, &EntityCollidersWindow<CONTEXT>::onRigidbodyListSelect, this);
        boxsizer->Add(mRigidbodies, 2, wxALIGN_CENTER_HORIZONTAL);

        if (mEntity.has<ungod::MultiRigidbodyComponent<CONTEXT>>()) //otherwise this button is redundant
        {
            wxBoxSizer* countSetSizer = new wxBoxSizer(wxHORIZONTAL);

            mCompCountCtrl = new wxTextCtrl(this, -1, 
                _(std::to_string(mEntity.get<ungod::MultiRigidbodyComponent<CONTEXT>>().getComponentCount())));
            countSetSizer->Add(mCompCountCtrl, 0, wxALIGN_CENTER_HORIZONTAL);

            wxButton* setButton = new wxButton(this, -1, _("set collider count"));
            setButton->Bind(wxEVT_BUTTON, &EntityCollidersWindow<CONTEXT>::onSetColliderCountClicked, this);
            countSetSizer->Add(setButton, 0, wxALIGN_CENTER_HORIZONTAL);

            boxsizer->Add(countSetSizer, 1, wxALIGN_CENTER_HORIZONTAL);
        }

        updateRigidbodyList();

        SetSizer(boxsizer);

        if (mEntity.has<ungod::RigidbodyComponent<CONTEXT>>() ||
            (mEntity.has < ungod::MultiRigidbodyComponent<CONTEXT>() &&
                mEntity.get < ungod::MultiRigidbodyComponent<CONTEXT>().getComponentCount() > 0))
            displayRigidbody(0);
        else
            displayRigidbody(-1);
    }


    template<std::size_t CONTEXT>
    void EntityCollidersWindow<CONTEXT>::onColliderChanged(ungod::Entity e)
    {
        updateRigidbodyList();
        if (mSelection != -1 && (unsigned)mSelection < e.get<ungod::RigidbodyComponent<CONTEXT>>().getColliders().size())
            displayRigidbody(mSelection);
    }


    template<std::size_t CONTEXT>
    void EntityCollidersWindow<CONTEXT>::displayRigidbody(int index)
    {
        if (mColliderDetail)
        {
            GetSizer()->Remove(mColliderDetail);
            mColliderDetail->Destroy();
        }
        mSelection = index;
        if (mSelection == -1) //no rigidbody selected
        {
            mColliderDetail = new wxPanel(this);
        }
        else
        {
            const ungod::Collider* coll;
            int index = -1;
            if (mEntity.has<ungod::RigidbodyComponent<CONTEXT>>())
            {
                coll = &mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getCollider();
            }
            else
            {
                coll = &mEntity.get<ungod::MultiRigidbodyComponent<CONTEXT>>().getComponent(mSelection).getCollider();
                index = mSelection;
            }

            switch (coll->getType())
            {
            case ungod::ColliderType::ROTATED_RECT:
                mColliderDetail = new RectangleWindow(mEntity, *coll, this, mWaw, index);
                break;
            case ungod::ColliderType::CONVEX_POLYGON: case ungod::ColliderType::EDGE_CHAIN:
                mColliderDetail = new PointSetWindow(mEntity, *coll, this, mWaw, index);
                break;
            default:
                mColliderDetail = new wxPanel(this);
                break;
            }
            
        }

        GetSizer()->Add(mColliderDetail, 4, wxALIGN_CENTER_HORIZONTAL);
        Fit();
    }


    template<std::size_t CONTEXT>
    void EntityCollidersWindow<CONTEXT>::updateRigidbodyList()
    {
        mRigidbodies->DeleteAllItems();
        if (mEntity.has<ungod::RigidbodyComponent<CONTEXT>>())
        {
            wxListItem rigidbodyItem;
            rigidbodyItem.SetId(0);
            mRigidbodies->InsertItem(rigidbodyItem);
            mRigidbodies->SetItem(0, 0, "0");
        }
        else //multi component
        {
            for (unsigned i = 0; i < mEntity.get<ungod::MultiRigidbodyComponent<CONTEXT>>().getComponentCount(); i++)
            {
                wxListItem rigidbodyItem;
                rigidbodyItem.SetId(i);
                mRigidbodies->InsertItem(rigidbodyItem);
                mRigidbodies->SetItem(i, 0, std::to_string(i));
            }
        }
    }


    template<std::size_t CONTEXT>
    void EntityCollidersWindow<CONTEXT>::onRigidbodyListSelect(wxListEvent& event)
    {
        displayRigidbody(event.GetIndex());
    }


    template<std::size_t CONTEXT>
    void EntityCollidersWindow<CONTEXT>::onSetColliderCountClicked(wxCommandEvent& event)
    {
        try
        {
            std::size_t cc = (std::size_t)(std::stoi(std::string(mCompCountCtrl->GetValue().mb_str())));
            mEntity.initMulti<ungod::MultiRigidbodyComponent<CONTEXT>>(cc);
            updateRigidbodyList();
        }
        catch (const std::exception & e)
        {
            auto err = wxMessageDialog(this, _("Text field must contain a valid number."));
            err.ShowModal();
        }
    }

    template<std::size_t CONTEXT>
    RectangleWindow<CONTEXT>::RectangleWindow(ungod::Entity e, const ungod::Collider& c, 
        wxWindow* parent, WorldActionWrapper& waw, int multiIndex) : wxPanel(parent), mRect(c), mMulti(multiIndex)
    {
        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
        {
            mUpLeftX = new StatDisplay<float>("up left x:", this, -1);
            mUpLeftX->connectSetter([this](float x)
                {
                    if (mMulti >= 0)
                        waw.setRectUpLeft(e, mMulti, { x, mRect.getUpLeftY() });
                    else
                        waw.setRectUpLeft(e, { x, mRect.getUpLeftY() });
                });
            mUpLeftX->connectGetter([this]()
                {
                    return mRect.getUpLeftX();
                });
            vbox->Add(mUpLeftX, 0, wxALIGN_CENTER_HORIZONTAL);
        }
        {
            mUpLeftY = new StatDisplay<float>("up left y:", this, -1);
            mUpLeftY->connectSetter([this](float y)
                {
                    if (mMulti >= 0)
                        waw.setRectUpLeft(e, mMulti, { mRect.getUpLeftX(), y });
                    else
                        waw.setRectUpLeft(e, { mRect.getUpLeftX(), y });
                });
            mUpLeftY->connectGetter([this]()
                {
                    return mRect.getUpLeftY();
                });
            vbox->Add(mUpLeftY, 0, wxALIGN_CENTER_HORIZONTAL);
        }
        {
            mDownRightX = new StatDisplay<float>("down right x:", this, -1);
            mDownRightX->connectSetter([this](float x)
                {
                    if (mMulti >= 0)
                        waw.setRectDownRight(e, mMulti, { x, mRect.getDownRightY() });
                    else
                        waw.setRectDownRight(e, { x, mRect.getDownRightY() });
                });
            mDownRightX->connectGetter([this]()
                {
                    return mRect.getDownRightX();
                });
            vbox->Add(mDownRightX, 0, wxALIGN_CENTER_HORIZONTAL);
        }
        {
            mDownRightY = new StatDisplay<float>("down right y:", this, -1);
            mDownRightY->connectSetter([this](float y)
                {
                    if (mMulti >= 0)
                        waw.setRectDownRight(e, mMulti, { mRect.getDownRightX(), y });
                    else
                        waw.setRectDownRight(e, { mRect.getDownRightX(), y });
                });
            mDownRightY->connectGetter([this]()
                {
                    return mRect.getDownRightY();
                });
            vbox->Add(mDownRightY, 0, wxALIGN_CENTER_HORIZONTAL);
        }
        {
            mRotation = new StatDisplay<float>("rotation y:", this, -1);
            mRotation->connectSetter([this](float y)
                {
                    if (mMulti >= 0)
                        waw.setRectRotation(e, mMulti, mRect.getRotation());
                    else
                        waw.setRectRotation(e, mRect.getRotation());
                });
            mRotation->connectGetter([this]()
                {
                    return mRect.getRotation();
                });
            vbox->Add(mRotation, 0, wxALIGN_CENTER_HORIZONTAL);
        }
        SetSizer(vbox);
        Fit();
    }



    template<std::size_t CONTEXT>
    PointSetWindow<CONTEXT>::PointSetWindow(ungod::Entity e, const ungod::Collider& c, 
        wxWindow* parent, WorldActionWrapper& waw, int multiIndex) : wxPanel(parent), mPointSet(c), mMulti(multiIndex)
    {
        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
        mPointsX.reserve(mPointSet.getNumberOfPoints());
        mPointsY.reserve(mPointSet.getNumberOfPoints());
        for (unsigned i = 0; i < mPointSet.getNumberOfPoints(); i++)
        {
            auto* statX = new StatDisplay<float>("p"+str(i)+" x:", this, -1);
            statX->connectSetter([this, i](float x)
                {
                    if (mMulti >= 0)
                        waw.setColliderPoint(e, mMulti, { x, mPointSet.getPointY(i) }, i);
                    else
                        waw.setColliderPoint(e, { x, mPointSet.getPointY(i) }, i);
                });
            statX->connectGetter([this, i]()
                {
                    return mPointSet.getPointX(i);
                });
            vbox->Add(statX, 0, wxALIGN_CENTER_HORIZONTAL);
            mPointsX.emplace_back(statX);

            auto* statY = new StatDisplay<float>("p" + str(i) + " y:", this, -1);
            statY->connectSetter([this](float y)
                {
                    if (mMulti >= 0)
                        waw.setColliderPoint(e, mMulti, { mPointSet.getPointX(i), y }, i);
                    else
                        waw.setColliderPoint(e, { mPointSet.getPointX(i), y }, i);
                });
            statY->connectGetter([this, i]()
                {
                    return mPointSet.getPointY(i);
                });
            vbox->Add(statY, 0, wxALIGN_CENTER_HORIZONTAL);
            mPointsY.emplace_back(statX);
        }
        SetSizer(vbox);
        Fit();
    }
}

#endif