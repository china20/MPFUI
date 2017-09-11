
#include "stdafx.h"
#include "ChatSession.h"

ChatItem::ChatItem(UserUseId* userInfo)
{
    _userInfo = userInfo;

    _iStartCp = 0;
    _iEndCp = 0;
    _iMessageCp = 0;
    _messageWid = 0;
    _alignFlag = eAlignFlag::eAlignLeft;

    _messageBk = NULL;
}

ChatItem::~ChatItem()
{
}

void ChatItem::SetStartCp(int cp)
{
    _iStartCp = cp;
}

void ChatItem::SetMessageCp(int cp)
{
    _iMessageCp = cp;
}

void ChatItem::SetEndCp(int cp)
{
    _iEndCp = cp;
}

int ChatItem::GetStartCp()
{
    return _iStartCp;
}

int ChatItem::GetMessageCp()
{
    return _iMessageCp;
}

int ChatItem::GetEndCp()
{
    return _iEndCp;
}

int ChatItem::GetMessageWid() const
{
    return _messageWid;
}

void ChatItem::SetMessageWid(int v)
{
    _messageWid = v;
}

eAlignFlag ChatItem::GetAlignFlag() const
{
    return _alignFlag;
}

void ChatItem::SetAlignFlag(eAlignFlag val)
{
    _alignFlag = val;
}

void ChatItem::SetMessageBk(suic::Brush* brush)
{
    _messageBk = brush;
}

void ChatItem::DrawHeader(suic::Drawing* drawing, suic::Rect rect)
{
    if (_userInfo && _userInfo->GetHeadIcon())
    {
        suic::ImageSource* pImgSrc = suic::DynamicCast<suic::ImageSource>(_userInfo->GetHeadIcon());
        if (pImgSrc != NULL && pImgSrc->GetBitmap() != NULL)
        {
            suic::Bitmap* pBmp = pImgSrc->GetBitmap();
            suic::fRect rcImg(0, 0, pBmp->Width(), pBmp->Height());
            drawing->DrawImage(suic::DrawCtx::DefDraw, pBmp, rect.TofRect(), &rcImg);
        }
    }
}

void ChatItem::DrawMessageBk(suic::Drawing* drawing, suic::Rect rect)
{
    if (NULL != _messageBk)
    {
        rect.Inflate(12,12,15,12);
        drawing->DrawRect(suic::DrawCtx::DefDraw, _messageBk, NULL, rect.TofRect());
    }
}

//==============================================
//

ChatSession::ChatSession(suic::String sessionId)
{
    _sessionId = sessionId;
    _displayMode = eDisplayMode::eDisplayBubble;
}

ChatSession::~ChatSession()
{
    Clear();
}

void ChatSession::AddChatItem(ChatItem* pInfo)
{
    _chatItems.Add(pInfo);
}

void ChatSession::Clear()
{
    for (int i = 0; i < _chatItems.GetCount(); ++i)
    {
        delete _chatItems[i];
    }

    _chatItems.Clear();
}

suic::String ChatSession::GetSessionId() const
{
    return _sessionId;
}

eDisplayMode ChatSession::GetDisplayMode() const
{
    return _displayMode;
}

bool ChatSession::SetDisplayMode(eDisplayMode displayMode)
{
    if (_displayMode != displayMode)
    {
        _displayMode = displayMode;
        return true;
    }
    else
    {
        return false;
    }
}

int ChatSession::GetCount() const
{
    return _chatItems.GetCount();
}

ChatItem* ChatSession::GetChatItem(int index) const
{
    return _chatItems[index];
}

int ChatSession::FindFirstVisibleChatItem(int iCp)
{
    for (int i = 0; i < _chatItems.GetCount(); ++i)
    {
        if (_chatItems[i]->GetStartCp() <= iCp && 
            _chatItems[i]->GetEndCp() >= iCp)
        {
            return i;
        }
    }

    return -1;
}

//==============================================
//

ChatTextContainer::ChatTextContainer()
    : _chatSession(NULL)
    , _textDoc(NULL)
{
}

ChatTextContainer::~ChatTextContainer()
{
}

void ChatTextContainer::SetChatSession(ChatSession* chatSession)
{
    _chatSession = chatSession;
}

void ChatTextContainer::UpdateDisplayMode(eDisplayMode mode)
{
    if (NULL != _textDoc && NULL != _chatSession && _chatSession->SetDisplayMode(mode))
    {
        _textDoc->SetReadOnly(false);
        _textDoc->Clear();
        _textDoc->SetReadOnly(true);

        for (int i = 0; i < _chatSession->GetCount(); ++i)
        {
            ChatItem* pInfo = _chatSession->GetChatItem(i);
            AddChatItemToView(pInfo);
        }
    }
}

void ChatTextContainer::AddChatItem(ChatItem* pInfo)
{
    _chatSession->AddChatItem(pInfo);
    AddChatItemToView(pInfo);
}

void ChatTextContainer::AddChatItemToView(ChatItem* pInfo)
{
    if (NULL != _textDoc && NULL != _chatSession)
    {
        suic::ITextDoc* txtDoc = _textDoc;
        bool isBubbleMode = _chatSession->GetDisplayMode() == eDisplayMode::eDisplayBubble;
        int iCharCount = txtDoc->GetCharCount();

        txtDoc->SetReadOnly(false);

        pInfo->SetStartCp(txtDoc->GetCharCount());

        // 光标移到最后
        txtDoc->SelectEnd();

        if (isBubbleMode)
        {
            txtDoc->GetTextFont()->SetTextColor(suic::Color::FromRgb(0x8b8b8b));
            txtDoc->GetTextFont()->Commit();
            txtDoc->AddText(pInfo->GetUserInfo()->GetUserName(), false);
        }
        else
        {
            suic::String strTitle;

            txtDoc->GetTextFont()->SetTextColor(suic::Colors::Blue);
            txtDoc->GetTextFont()->Commit();

            strTitle.Format(_U("%s %s"), pInfo->GetUserInfo()->GetUserName().c_str()
                , pInfo->GetChatDate().ToLongDate().c_str());
            txtDoc->AddText(strTitle, false);
        }

        txtDoc->SelectEnd();
        txtDoc->AddText(_U("\n"), false);
        pInfo->SetMessageCp(txtDoc->GetCharCount());

        // 光标移到最后
        txtDoc->SelectEnd();
        txtDoc->SetStream(pInfo->GetChatMsg(), true);

        // 恢复当前指针到开始
        pInfo->GetChatMsg()->Seek(0);

        if (isBubbleMode)
        {
            txtDoc->AddText(_U("\n"), false);
        }

        pInfo->SetEndCp(txtDoc->GetCharCount());

        if (isBubbleMode)
        {
            suic::TextRange* pRange = NULL;

            pRange = txtDoc->GetRange(pInfo->GetStartCp(), pInfo->GetMessageCp() - 2);
            pRange->GetTextPara()->SetSpaceAfter(15);
            pRange->GetTextPara()->SetSpaceBefore(6);
            if (pInfo->GetAlignFlag() == eAlignFlag::eAlignRight)
            {
                pRange->GetTextPara()->SetIndents(0, 5, 43);
            }
            else
            {
                pRange->GetTextPara()->SetIndents(0, 43, 5);
            }
            pRange->unref();

            pRange = txtDoc->GetRange(pInfo->GetMessageCp(), pInfo->GetEndCp() - 2);
            pRange->GetTextPara()->SetSpaceAfter(0);
            pRange->GetTextPara()->SetSpaceBefore(0);

            if (pInfo->GetAlignFlag() == eAlignFlag::eAlignRight)
            {
                pRange->GetTextPara()->SetIndents(0, 10, 56);
            }
            else
            {
                pRange->GetTextPara()->SetIndents(0, 56, 10);
            }
            pRange->unref();
        }
        else
        {
            suic::TextRange* pRange = NULL;
            suic::TextPara* pTp = NULL;

            pRange = txtDoc->GetRange(pInfo->GetStartCp(), pInfo->GetMessageCp() - 2);
            pTp = pRange->GetTextPara();

            pTp->SetSpaceAfter(8);
            pTp->SetSpaceBefore(8);
            pTp->SetIndents(0, 6, 5);

            pRange->unref();

            pRange = txtDoc->GetRange(pInfo->GetMessageCp(), pInfo->GetEndCp() - 2);
            pRange->GetTextPara()->SetSpaceAfter(0);
            pRange->GetTextPara()->SetSpaceBefore(0);
            pRange->GetTextPara()->SetIndents(0, 16, 5);
            pRange->unref();
        }

        txtDoc->SetReadOnly(true);
    }
    else
    {
        delete pInfo;
    }
}

void ChatTextContainer::AdjustChatItem(ChatItem* pInfo)
{
    if (_chatSession->GetDisplayMode() != eDisplayMode::eDisplayBubble)
    {
        return;
    }

    int iMaxWid = 0;
    suic::Rect rcClnt = GetTextRender()->GetViewRect();
    suic::Rect rcInset = GetTextRender()->GetPadding();

    rcClnt.Deflate(rcInset);

    int iViewWid = rcClnt.Width();

    int iStartLine = _textDoc->LineFromCharIndex(pInfo->GetStartCp());
    int iFirstLine = _textDoc->LineFromCharIndex(pInfo->GetMessageCp());
    int iEndLine = _textDoc->LineFromCharIndex(pInfo->GetEndCp() - 1);

    for (int i = iFirstLine; i <= iEndLine; ++i)
    {
        suic::LineDetail line;
        _textDoc->GetLineDetail(i, &line, NULL);

        if (line.xCount > 0)
        {
            iMaxWid = max(iMaxWid, line.xWidth + line.xWhite);
        }
    }

    int xLeft = iViewWid - iMaxWid - 56;

    for (int j = iFirstLine; j <= iEndLine; ++j)
    {
        if (pInfo->GetAlignFlag() == eAlignFlag::eAlignRight)
        {
            SetLineLeftOffset(j, xLeft);
        }
    }

    //pInfo->SetVersion(_version);
    pInfo->SetMessageWid(iMaxWid);

    if (pInfo->GetAlignFlag() == eAlignFlag::eAlignRight)
    {
        suic::LineDetail line;
        _textDoc->GetLineDetail(iStartLine, &line, NULL);
        int iTitleOffset = iViewWid - line.xWidth - 43 - 5 - 4;
        SetLineLeftOffset(iStartLine, iTitleOffset);
    }
}

void ChatTextContainer::OnRender(suic::Drawing* drawing)
{
    suic::Point pt = GetCanvasOffset();
    suic::fRect rcClient(pt, GetRenderSize());
    suic::ITextRender* textRender = GetTextRender();

    UpdateViewRect();

    if (_chatSession != NULL && _chatSession->GetDisplayMode() == eDisplayMode::eDisplayBubble)
    {
        int iFirstChatInfo = 0;
        int iLineCount = _textDoc->GetLineCount();
        int iVisibleLine = _textDoc->GetFirstVisibleLine();
        int iMaxBot = GetActualHeight() + 16 + pt.y;

        UpdateView();

        int iFirstCp = _textDoc->CharIndexFromLine(iVisibleLine);
        iFirstChatInfo = _chatSession->FindFirstVisibleChatItem(iFirstCp);

        if (iFirstChatInfo >= 0)
        {
            bool bLoop = true;
            int i = iFirstChatInfo;
  
            for (; i < _chatSession->GetCount(); ++i)
            {
                ChatItem* pInfo = _chatSession->GetChatItem(i);
                if (pInfo->GetEndCp() >= iFirstCp)
                {
                    iFirstCp = pInfo->GetMessageCp();
                    iFirstChatInfo = i;
                    break;
                }
            }

            for (; i < _chatSession->GetCount(); ++i)
            {
                ChatItem* pInfo = _chatSession->GetChatItem(i);
                suic::Rect rect;

                AdjustChatItem(pInfo);

                if (iFirstChatInfo != i)
                {
                    iFirstCp = pInfo->GetMessageCp();
                }

                int iEndCp = pInfo->GetEndCp() - 2;

                suic::Point begPoint = _textDoc->GetPoint(iFirstCp, true);
                suic::Point endPoint = _textDoc->GetPoint(iEndCp, false);

                rect.top = begPoint.y - pt.y;
                rect.bottom = endPoint.y - pt.y;

                if (pInfo->GetAlignFlag() == eAlignFlag::eAlignRight)
                {
                    rect.right = GetActualWidth() - 2 - 56;
                    rect.left = rect.right - pInfo->GetMessageWid();
                }
                else
                {
                    rect.left = 56 + 2;
                    rect.right = rect.left + pInfo->GetMessageWid();
                }

                begPoint = _textDoc->GetPoint(pInfo->GetStartCp(), true);

                suic::Rect rcHead = rect;

                rcHead.top = begPoint.y - pt.y + 5;
                rcHead.bottom = rcHead.top + 32;

                if (pInfo->GetAlignFlag() == eAlignFlag::eAlignRight)
                {
                    rcHead.right = GetActualWidth() - 8;
                    rcHead.left = rcHead.right - 32;
                }
                else
                {
                    rcHead.left = 8;
                    rcHead.right = rcHead.left + 32;
                }

                pInfo->DrawMessageBk(drawing, rect);
                pInfo->DrawHeader(drawing, rcHead);

                if (!bLoop)
                {
                    break;
                }
            }
        }
    }

    textRender->Render(drawing);
}

//==============================================
//

ImplementRTTIOfClass(ChatTextBox, suic::RichTextBox)

ChatTextBox::ChatTextBox()
{
}

ChatTextBox::~ChatTextBox()
{
}

void ChatTextBox::AddChatItem(ChatItem* pInfo)
{
    GetChatTextContainer()->AddChatItem(pInfo);
}

ChatTextContainer* ChatTextBox::GetChatTextContainer()
{
    return (ChatTextContainer*)GetTextContainer();
}

suic::TextContainer* ChatTextBox::CreateTestHost()
{
    return new ChatTextContainer();
}

void ChatTextBox::OnInitialized(suic::EventArg* e)
{
    ChatTextContainer* txtContainer = GetChatTextContainer();
    txtContainer->_textDoc = GetTextDoc();

    suic::RichTextBox::OnInitialized(e);
}

void ChatTextBox::OnMouseLeftButtonDown(suic::MouseButtonEventArg* e)
{
    // 我们只关心双击事件
    if (e->GetClickCount() % 2 == 0)
    {
        suic::Point pt = e->GetMousePoint();
        ChatTextContainer* textContainer = GetChatTextContainer();
        suic::Embbed* pEmbbed = textContainer->HitTestEmbbed(pt);

        if (NULL != pEmbbed)
        {
            if (_imageView)
            {
                _imageView->Close();
            }
            else
            {
                _imageView = new suic::Window();
            }

            suic::XamlReader::LoadUri(NULL, _imageView.get(), "QQ/Layout/ImageViewWindow.xml");
            if (_imageView)
            {
                suic::ImageEmbbed* imgEmbbed = suic::DynamicCast<suic::ImageEmbbed>(pEmbbed);
                suic::Image* pImage = NULL;
                if (imgEmbbed)
                {
                    _imageView->Show();
                    pImage = _imageView->FindElem<suic::Image>("ImgView");
                    if (pImage)
                    {
                        pImage->SetSource(imgEmbbed->GetImageSource());
                    }
                    _imageView->CenterWindow();
                }
            }
            pEmbbed->unref();
            e->SetHandled(true);
        }
    }

    suic::RichTextBox::OnMouseLeftButtonDown(e);
}
