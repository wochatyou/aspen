/* Copyright 2022 the SumatraPDF project authors (see AUTHORS file).
   License: Simplified BSD (see COPYING.BSD) */

struct LabelWithCloseCreateArgs {
    HWND parent = nullptr;
    HFONT font = nullptr;
    int cmdId = 0;
};

struct LabelWithCloseWnd : Wnd {
    LabelWithCloseWnd() = default;
    ~LabelWithCloseWnd() = default;

    HWND Create(const LabelWithCloseCreateArgs&);

    void OnPaint(HDC hdc, PAINTSTRUCT* ps) override;
    LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;

    void SetLabel(const WCHAR*) const;
    void SetFont(HFONT);
    void SetBgCol(COLORREF);
    void SetTextCol(COLORREF);
    void SetPaddingXY(int x, int y);

    Size GetIdealSize() const;

    int cmdId = 0;

    Rect closeBtnPos{};
    COLORREF txtCol = 0;
    COLORREF bgCol = 0;

    // in points
    int padX = 0;
    int padY = 0;
};
