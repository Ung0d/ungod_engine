#ifndef UEDIT_ENTITY_COLLIDERS_WINDOW_INL
#define UEDIT_ENTITY_COLLIDERS_WINDOW_INL

namespace uedit
{
    template<std::size_t CONTEXT>
    EntityCollidersWindow<CONTEXT>::EntityCollidersWindow(ungod::Entity e, WorldActionWrapper& waw, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& siz)
        : wxWindow(parent, id, pos, siz), mEntity(e), mWaw(waw), mSelection(-1), mSingleSelected(false), mColliderDetail(nullptr)
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

        if (mEntity.has<ungod::MultiRigidbodyComponent<CONTEXT>>()) //otherwise redundant
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

        {
            wxBoxSizer* initButtonsSizer = new wxBoxSizer(wxHORIZONTAL);

            wxButton* rectButton = new wxButton(this, -1, _("reactangle"));
            rectButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) 
                {
                    ungod::Collider rect; 
                    if (mEntity.get<ungod::TransformComponent>().getSize().x == 0.0f || 
                        mEntity.get<ungod::TransformComponent>().getSize().y == 0.0f)
                        rect = ungod::makeRotatedRect({ 0,0 }, { 50, 50 });
                    else
                        rect = ungod::makeRotatedRect({ 0,0 }, mEntity.get<ungod::TransformComponent>().getSize());
                    if (mSingleSelected)
                        mWaw.addCollider(mEntity, rect);
                    else if (mSelection >= 0)
                        mWaw.addCollider(mEntity, mSelection, rect);
                });
            initButtonsSizer->Add(rectButton, 0, wxALIGN_CENTER);

            wxButton* polyButton = new wxButton(this, -1, _("polygon"));
            polyButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event)
                {
                    ungod::Collider poly;
                    if (mEntity.get<ungod::TransformComponent>().getSize().x == 0.0f ||
                        mEntity.get<ungod::TransformComponent>().getSize().y == 0.0f)
                        poly = ungod::makeConvexPolygon({ { 0,0 }, {50, 0}, {25, 50} });
                    else
                        poly = ungod::makeConvexPolygon({ { 0,0 }, {mEntity.get<ungod::TransformComponent>().getSize().x, 0},
                                                {mEntity.get<ungod::TransformComponent>().getSize().x/2, mEntity.get<ungod::TransformComponent>().getSize().y} });
                    if (mSingleSelected)
                        mWaw.addCollider(mEntity, poly);
                    else if (mSelection >= 0)
                        mWaw.addCollider(mEntity, mSelection, poly);
                });
            initButtonsSizer->Add(polyButton, 0, wxALIGN_CENTER);

            wxButton* edgechainButton = new wxButton(this, -1, _("edge chain"));
            edgechainButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event)
                {
                    ungod::Collider chain = ungod::makeEdgeChain({ { 0,0 }, {100, 0} });
                    if (mSingleSelected)
                        mWaw.addCollider(mEntity, chain);
                    else if (mSelection >= 0)
                        mWaw.addCollider(mEntity, mSelection, chain);
                });
            initButtonsSizer->Add(edgechainButton, 0, wxALIGN_CENTER);

            boxsizer->Add(initButtonsSizer, 1, wxALIGN_CENTER_HORIZONTAL);
        }


        updateRigidbodyList();

        SetSizer(boxsizer);

        if (mEntity.has<ungod::RigidbodyComponent<CONTEXT>>())
            displayRigidbody(0, true);
        else if (mEntity.has < ungod::MultiRigidbodyComponent<CONTEXT>>() &&
                mEntity.get < ungod::MultiRigidbodyComponent<CONTEXT>>().getComponentCount() > 0)
            displayRigidbody(0, false);
        else
            displayRigidbody(-1, false);
    }


    template<std::size_t CONTEXT>
    void EntityCollidersWindow<CONTEXT>::onColliderChanged(ungod::Entity e)
    {
        if (mColliderDetail)
        {
            auto* disp = static_cast<BaseColliderDisplay*>(mColliderDetail);
            if (mSingleSelected)
            {
                if (e.get<ungod::RigidbodyComponent<CONTEXT>>().getCollider().getType() == disp->getType())
                    disp->refresh();
                else
                    displayRigidbody(0, true);
            }
            else //if (mSelection != -1)
            {
                if (e.get<ungod::MultiRigidbodyComponent<CONTEXT>>().getComponent(mSelection).getCollider().getType() == disp->getType())
                    disp->refresh();
                else
                    displayRigidbody(mSelection, false);
            }
        }
    }


    template<std::size_t CONTEXT>
    void EntityCollidersWindow<CONTEXT>::displayRigidbody(int index, bool single)
    {
        if (mColliderDetail)
        {
            GetSizer()->Detach(mColliderDetail);
            mColliderDetail->Destroy();
        }
        mSelection = index;
        mSingleSelected = single;
        if (mSelection == -1) //no rigidbody selected
        {
            mColliderDetail = new wxPanel(this);
        }
        else
        {
            const ungod::Collider* coll;
            int isel = -1;
            if (mSingleSelected)
            {
                coll = &mEntity.get<ungod::RigidbodyComponent<CONTEXT>>().getCollider();
            }
            else
            {
                coll = &mEntity.get<ungod::MultiRigidbodyComponent<CONTEXT>>().getComponent(mSelection).getCollider();
                isel = mSelection;
            }

            switch (coll->getType())
            {
            case ungod::ColliderType::ROTATED_RECT:
                mColliderDetail = new RectangleWindow<CONTEXT>(mEntity, *coll, this, mWaw, isel);
                break;
            case ungod::ColliderType::CONVEX_POLYGON: case ungod::ColliderType::EDGE_CHAIN:
                mColliderDetail = new PointSetWindow<CONTEXT>(mEntity, *coll, this, mWaw, isel);
                break;
            default:
                mColliderDetail = new wxPanel(this);
                break;
            }
            
        }

        GetSizer()->Add(mColliderDetail, 4, wxALIGN_CENTER_HORIZONTAL);
        Layout();
        Fit();
    }


    template<std::size_t CONTEXT>
    void EntityCollidersWindow<CONTEXT>::updateRigidbodyList()
    {
        mRigidbodies->DeleteAllItems();
        int offset = 0;
        if (mEntity.has<ungod::RigidbodyComponent<CONTEXT>>())
        {
            wxListItem rigidbodyItem;
            rigidbodyItem.SetId(0);
            mRigidbodies->InsertItem(rigidbodyItem);
            mRigidbodies->SetItem(0, 0, "0");
            offset = 1;
        }

        if (mEntity.has<ungod::MultiRigidbodyComponent<CONTEXT>>())
        {
            for (unsigned i = 0; i < mEntity.get<ungod::MultiRigidbodyComponent<CONTEXT>>().getComponentCount(); i++)
            {
                wxListItem rigidbodyItem;
                rigidbodyItem.SetId(i);
                mRigidbodies->InsertItem(rigidbodyItem);
                mRigidbodies->SetItem(i+ offset, 0, std::to_string(i));
            }
        }
    }


    template<std::size_t CONTEXT>
    ungod::RigidbodyComponent<CONTEXT>* EntityCollidersWindow<CONTEXT>::getActiveComponent() const
    {
        if (mSingleSelected)
            return &mEntity.modify<ungod::RigidbodyComponent<CONTEXT>>();
        else if (mSelection != -1)
            return &mEntity.modify<ungod::MultiRigidbodyComponent<CONTEXT>>().getComponent(mSelection);
        else
            return nullptr;
    }


    template<std::size_t CONTEXT>
    void EntityCollidersWindow<CONTEXT>::onRigidbodyListSelect(wxListEvent& event)
    {
        if (mEntity.has<ungod::MultiRigidbodyComponent<CONTEXT>>())
        {
            if (event.GetIndex() == 0)
                displayRigidbody(0, true);
            else
                displayRigidbody(event.GetIndex()-1, false);
        }
        else
            displayRigidbody(event.GetIndex(), false);
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
        wxWindow* parent, WorldActionWrapper& waw, int multiIndex) : BaseColliderDisplay(parent), mEntity(e), mRect(c), mMulti(multiIndex), mWaw(waw)
    {
        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
        {
            mUpLeftX = new StatDisplay<float>("up left x:", this, -1);
            mUpLeftX->connectSetter([this](float x)
                {
                    if (mMulti >= 0)
                        mWaw.setRectUpLeft(mEntity, mMulti, { x, mRect.getUpLeftY() });
                    else
                        mWaw.setRectUpLeft(mEntity, { x, mRect.getUpLeftY() });
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
                        mWaw.setRectUpLeft(mEntity, mMulti, { mRect.getUpLeftX(), y });
                    else
                        mWaw.setRectUpLeft(mEntity, { mRect.getUpLeftX(), y });
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
                        mWaw.setRectDownRight(mEntity, mMulti, { x, mRect.getDownRightY() });
                    else
                        mWaw.setRectDownRight(mEntity, { x, mRect.getDownRightY() });
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
                        mWaw.setRectDownRight(mEntity, mMulti, { mRect.getDownRightX(), y });
                    else
                        mWaw.setRectDownRight(mEntity, { mRect.getDownRightX(), y });
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
                        mWaw.setRectRotation(mEntity, mMulti, mRect.getRotation());
                    else
                        mWaw.setRectRotation(mEntity, mRect.getRotation());
                });
            mRotation->connectGetter([this]()
                {
                    return mRect.getRotation();
                });
            vbox->Add(mRotation, 0, wxALIGN_CENTER_HORIZONTAL);
        }
        refresh();
        SetSizer(vbox);
        Fit();
    }


    template<std::size_t CONTEXT>
    void RectangleWindow<CONTEXT>::refresh()
    {
        mUpLeftX->refreshValue();
        mUpLeftY->refreshValue();
        mDownRightX->refreshValue();
        mDownRightY->refreshValue();
        mRotation->refreshValue();
    }


    template<std::size_t CONTEXT>
    PointSetWindow<CONTEXT>::PointSetWindow(ungod::Entity e, const ungod::Collider& c, 
        wxWindow* parent, WorldActionWrapper& waw, int multiIndex) : BaseColliderDisplay(parent), 
        mEntity(e), mPointSet(c), mMulti(multiIndex), mWaw(waw), mVbox(new wxBoxSizer(wxVERTICAL)), mType(mPointSet.getType())
    {
        mPointsX.reserve(mPointSet.getNumberOfPoints());
        mPointsY.reserve(mPointSet.getNumberOfPoints());
        for (unsigned i = 0; i < mPointSet.getNumberOfPoints(); i++)
            addPoint(i);
        refresh();
        SetSizer(mVbox);
        Fit();
    }


    template<std::size_t CONTEXT>
    void PointSetWindow<CONTEXT>::addPoint(unsigned i)
    {
        auto* statX = new StatDisplay<float>("p" + std::to_string(i) + " x:", this, -1);
        statX->connectSetter([this, i](float x)
            {
                if (mMulti >= 0)
                    mWaw.setColliderPoint(mEntity, mMulti, { x, mPointSet.getPointY(i) }, i);
                else
                    mWaw.setColliderPoint(mEntity, { x, mPointSet.getPointY(i) }, i);
            });
        statX->connectGetter([this, i]()
            {
                return mPointSet.getPointX(i);
            });
        mVbox->Add(statX, 0, wxALIGN_CENTER_HORIZONTAL);
        mPointsX.emplace_back(statX);

        auto* statY = new StatDisplay<float>("p" + std::to_string(i) + " y:", this, -1);
        statY->connectSetter([this,i](float y)
            {
                if (mMulti >= 0)
                    mWaw.setColliderPoint(mEntity, mMulti, { mPointSet.getPointX(i), y }, i);
                else
                    mWaw.setColliderPoint(mEntity, { mPointSet.getPointX(i), y }, i);
            });
        statY->connectGetter([this, i]()
            {
                return mPointSet.getPointY(i);
            });
        mVbox->Add(statY, 0, wxALIGN_CENTER_HORIZONTAL);
        mPointsY.emplace_back(statY);
    }


    template<std::size_t CONTEXT>
    void PointSetWindow<CONTEXT>::refresh()
    {
        if (mPointSet.getNumberOfPoints() > mPointsX.size())
        {
            for (unsigned i = mPointsX.size(); i < mPointSet.getNumberOfPoints(); i++)
                addPoint(i);
            Fit();
        }
        else if (mPointSet.getNumberOfPoints() < mPointsX.size())
        {
            mPointsX.clear();
            mPointsY.clear();
            for (unsigned i = 0; i < mPointSet.getNumberOfPoints(); i++)
                addPoint(i);
            Fit();
        }
        for (auto d : mPointsX)
            d->refreshValue();
        for (auto d : mPointsY)
            d->refreshValue();
    }
}

#endif