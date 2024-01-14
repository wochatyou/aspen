/// TTE: Tiny Text Editor using DirectWrite and Direct2D
/// We are using the source code of PadWrite in Microsoft SDK samples

#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef UNICODE
#define UNICODE
#endif

#include <string>
#include <vector>
#include <atlbase.h>
#include <atlwin.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

typedef D2D1_RECT_F RectF;

class DrawingEffect
{
protected:
    // The color is stored as BGRA, with blue in the lowest byte,
    // then green, red, alpha; which is what D2D, GDI+, and GDI DIBs use.
    // GDI's COLORREF stores red as the lowest byte.
    UINT32 color_;
    
public:
    DrawingEffect(UINT32 color)
    :   color_(color)
    { }

    inline UINT32 GetColor() const throw()
    {
        // Returns the BGRA value for D2D.
        return color_;
    }

    inline COLORREF GetColorRef() const throw()
    {
        // Returns color as COLORREF.
        return GetColorRef(color_);
    }

    static inline COLORREF GetColorRef(UINT32 bgra) throw()
    {
        // Swaps color order (bgra <-> rgba) from D2D/GDI+'s to a COLORREF.
        // This also leaves the top byte 0, since alpha is ignored anyway.
        return RGB(GetBValue(bgra), GetGValue(bgra), GetRValue(bgra));
    }

    static inline COLORREF GetBgra(COLORREF rgb) throw()
    {
        // Swaps color order (bgra <-> rgba) from COLORREF to D2D/GDI+'s.
        // Sets alpha to full opacity.
        return RGB(GetBValue(rgb), GetGValue(rgb), GetRValue(rgb)) | 0xFF000000;
    }
    
};

// Releases a COM object and nullifies pointer.
template <typename InterfaceType>
inline void SafeRelease(InterfaceType** currentObject)
{
    if (*currentObject != NULL)
    {
        (*currentObject)->Release();
        *currentObject = NULL;
    }
}

// Acquires an additional reference, if non-null.
template <typename InterfaceType>
inline InterfaceType* SafeAcquire(InterfaceType* newObject)
{
    if (newObject != NULL)
        newObject->AddRef();

    return newObject;
}


// Sets a new COM object, releasing the old one.
template <typename InterfaceType>
inline void SafeSet(InterfaceType** currentObject, InterfaceType* newObject)
{
    SafeAcquire(newObject);
    SafeRelease(currentObject);
    *currentObject = newObject;
}


// Releases a COM object and nullifies pointer.
template <typename InterfaceType>
inline InterfaceType* SafeDetach(InterfaceType** currentObject)
{
    InterfaceType* oldObject = *currentObject;
    *currentObject = NULL;
    return oldObject;
}


// Sets a new COM object, acquiring the reference.
template <typename InterfaceType>
inline void SafeAttach(InterfaceType** currentObject, InterfaceType* newObject)
{
    SafeRelease(currentObject);
    *currentObject = newObject;
}


const static wchar_t g_sampleText[] = 
    L"\tDirectWrite SDK sample\r\n\n"
    L"Feel free to play around with the formatting options to see just some of what DWrite is capable of:\n\n"
    L"Glyph rendering, Complex script shaping, Script analysis, Bidi ordering (\x202E" L"abc" L"\x202C), Line breaking, Font fallback, "
    L"Font enumeration, ClearType rendering, Bold/Italic/Underline/Strikethrough/Narrow/Light, OpenType styles, Inline objects \xFFFC\xFFFC, "
    L"Trimming, Selection hit-testing...\r\n"
    L"\r\n";
   

struct QiListNil {};

// When the QueryInterface list refers to itself as class,
// which hasn't fully been defined yet.
template <typename InterfaceName, typename InterfaceChain>
class QiListSelf : public InterfaceChain
{
public:
    inline void QueryInterfaceInternal(IID const& iid, OUT void** ppObject) throw()
    {
        if (iid != __uuidof(InterfaceName))
            return InterfaceChain::QueryInterfaceInternal(iid, ppObject);

        *ppObject = static_cast<InterfaceName*>(this);
    }
};

// When this interface is implemented and more follow.
template <typename InterfaceName, typename InterfaceChain = QiListNil>
class QiList : public InterfaceName, public InterfaceChain
{
public:
    inline void QueryInterfaceInternal(IID const& iid, OUT void** ppObject) throw()
    {
        if (iid != __uuidof(InterfaceName))
            return InterfaceChain::QueryInterfaceInternal(iid, ppObject);

        *ppObject = static_cast<InterfaceName*>(this);
    }
};


// When the this is the last implemented interface in the list.
template <typename InterfaceName>
class QiList<InterfaceName, QiListNil> : public InterfaceName
{
public:
    inline void QueryInterfaceInternal(IID const& iid, OUT void** ppObject) throw()
    {
        if (iid != __uuidof(InterfaceName))
            return;

        *ppObject = static_cast<InterfaceName*>(this);
    }
};


// Generic COM base implementation for classes, since DirectWrite uses
// callbacks for several different kinds of objects, particularly the
// text renderer and inline objects.
//
// Example:
//
//  class RenderTarget : public ComBase<QiList<IDWriteTextRenderer> >
//
template <typename InterfaceChain>
class ComBase : public InterfaceChain
{
public:
    explicit ComBase() throw()
    :   refValue_(0)
    { }

    // IUnknown interface
    IFACEMETHOD(QueryInterface)(IID const& iid, OUT void** ppObject)
    {
        *ppObject = NULL;
        InterfaceChain::QueryInterfaceInternal(iid, ppObject);
        if (*ppObject == NULL)
            return E_NOINTERFACE;

        AddRef();
        return S_OK;
    }

    IFACEMETHOD_(ULONG, AddRef)()
    {
        return InterlockedIncrement(&refValue_);
    }

    IFACEMETHOD_(ULONG, Release)()
    {
        ULONG newCount = InterlockedDecrement(&refValue_);
        if (newCount == 0)
            delete this;

        return newCount;
    }

    virtual ~ComBase()
    { }

protected:
    ULONG refValue_;

private:
    // No copy construction allowed.
    ComBase(const ComBase& b);
    ComBase& operator=(ComBase const&);
};



class RenderTarget;

// Intermediate render target for UI to draw to either a D2D or GDI surface.
class DECLSPEC_UUID("4327AC14-3172-4807-BF40-02C7475A2520") RenderTarget
    :   public ComBase<QiListSelf<RenderTarget, QiList<IDWriteTextRenderer>>>
{
public:
    virtual ~RenderTarget() {};
    
    virtual void BeginDraw() = NULL;
    virtual void EndDraw() = NULL;
    virtual void Clear(UINT32 color) = NULL;
    virtual void Resize(UINT width, UINT height) = NULL;
    virtual void UpdateMonitor() = NULL;

    virtual void SetTransform(DWRITE_MATRIX const& transform) = NULL;
    virtual void GetTransform(DWRITE_MATRIX& transform) = NULL;
    virtual void SetAntialiasing(bool isEnabled) = NULL;

    virtual void DrawTextLayout(
        IDWriteTextLayout* textLayout,
        const RectF& rect
        ) = NULL;

    // Draws a single image, from the given coordinates, to the given coordinates.
    // If the height and width differ, they will be scaled, but mirroring must be
    // done via a matrix transform.
    virtual void DrawImage(
        IWICBitmapSource* image,
        const RectF& sourceRect,  // where in source atlas texture
        const RectF& destRect     // where on display to draw it
        ) = NULL;

    virtual void FillRectangle(
        const RectF& destRect,
        const DrawingEffect& drawingEffect
        ) = NULL;

protected:
    // This context is not persisted, only existing on the stack as it
    // is passed down through. This is mainly needed to handle cases
    // where runs where no drawing effect set, like those of an inline
    // object or trimming sign.
    struct Context
    {
        Context(RenderTarget* initialTarget, IUnknown* initialDrawingEffect)
        :   target(initialTarget),
            drawingEffect(initialDrawingEffect)
        { }

        // short lived weak pointers
        RenderTarget* target;
        IUnknown* drawingEffect;
    };

    IUnknown* GetDrawingEffect(void* clientDrawingContext, IUnknown* drawingEffect)
    {
        // Callbacks use this to use a drawing effect from the client context
        // if none was passed into the callback.
        if (drawingEffect != NULL)
            return drawingEffect;

        return (reinterpret_cast<Context*>(clientDrawingContext))->drawingEffect;
    }
    
};

class RenderTargetD2D : public RenderTarget
{
public:
    RenderTargetD2D(ID2D1Factory* d2dFactory, IDWriteFactory* dwriteFactory, HWND hwnd);

    HRESULT static Create(ID2D1Factory* d2dFactory, IDWriteFactory* dwriteFactory, HWND hwnd, OUT RenderTarget** renderTarget);

    virtual ~RenderTargetD2D();

    virtual void BeginDraw()
    {
        if(target_)
        {
            target_->BeginDraw();
            target_->SetTransform(D2D1::Matrix3x2F::Identity());
        }
    }
    
    virtual void EndDraw()
    {
        if(target_)
        {
            HRESULT hr = target_->EndDraw();
            // If the device is lost for any reason, we need to recreate it.
            if (hr == D2DERR_RECREATE_TARGET)
            {
                // Flush resources and recreate them.
                // This is very rare for a device to be lost,
                // but it can occur when connecting via Remote Desktop.
                imageCache_.clear();
                hmonitor_ = NULL;
                CreateTarget();
            }
        }
    }
    
    virtual void Clear(UINT32 color)
    {
         target_->Clear(D2D1::ColorF(color));
    }
    
    virtual void Resize(UINT width, UINT height)
    {
        D2D1_SIZE_U size;
        size.width = width;
        size.height = height;
        target_->Resize(size);
    }
    
    virtual void UpdateMonitor()
    {
        // Updates rendering parameters according to current monitor.
        HMONITOR monitor = MonitorFromWindow(hwnd_, MONITOR_DEFAULTTONEAREST);
        if (monitor != hmonitor_)
        {
            // Create based on monitor settings, rather than the defaults of
            // gamma=1.8, contrast=.5, and clearTypeLevel=.5

            IDWriteRenderingParams* renderingParams = NULL;

            dwriteFactory_->CreateMonitorRenderingParams(
                                monitor,
                                &renderingParams
                                );
            target_->SetTextRenderingParams(renderingParams);

            hmonitor_ = monitor;
            InvalidateRect(hwnd_, NULL, FALSE);

            SafeRelease(&renderingParams);
        }
    }

    virtual void SetTransform(DWRITE_MATRIX const& transform)
    {
        ATLASSERT(target_);
        target_->SetTransform(reinterpret_cast<const D2D1_MATRIX_3X2_F*>(&transform));
    }
    
    virtual void GetTransform(DWRITE_MATRIX& transform)
    {
        ATLASSERT(target_);
        target_->GetTransform(reinterpret_cast<D2D1_MATRIX_3X2_F*>(&transform));
    }
    
    virtual void SetAntialiasing(bool isEnabled)
    {
        ATLASSERT(target_);
        target_->SetAntialiasMode(isEnabled ? D2D1_ANTIALIAS_MODE_PER_PRIMITIVE : D2D1_ANTIALIAS_MODE_ALIASED);
    }

    virtual void DrawTextLayout(IDWriteTextLayout* textLayout, const RectF& rect)
    {
        ATLASSERT(target_);
        if (textLayout != NULL)
        {
            Context context(this, NULL);
            textLayout->Draw(&context, this, rect.left, rect.top);
        }
    }

    virtual void DrawImage(IWICBitmapSource* image,
        const RectF& sourceRect,  // where in source atlas texture
        const RectF& destRect     // where on display to draw it
        )
    {
        
    }

    void FillRectangle(const RectF& destRect, const DrawingEffect& drawingEffect)
    {
        ATLASSERT(target_);

        ID2D1Brush* brush = GetCachedBrush(&drawingEffect);
        if (brush != NULL)
        {
            // We will always get a strikethrough as a LTR rectangle
            // with the baseline origin snapped.
            target_->FillRectangle(destRect, brush);
        }
    }

    // IDWriteTextRenderer implementation

    IFACEMETHOD(DrawGlyphRun)(
        void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        DWRITE_MEASURING_MODE measuringMode,
        const DWRITE_GLYPH_RUN* glyphRun,
        const DWRITE_GLYPH_RUN_DESCRIPTION* glyphRunDescription,
        IUnknown* clientDrawingEffect
        )
    {
#if 0        
        // If no drawing effect is applied to run, but a clientDrawingContext
        // is passed, use the one from that instead. This is useful for trimming
        // signs, where they don't have a color of their own.
        clientDrawingEffect = GetDrawingEffect(clientDrawingContext, clientDrawingEffect);

        // Since we use our own custom renderer and explicitly set the effect
        // on the layout, we know exactly what the parameter is and can
        // safely cast it directly.
        DrawingEffect* effect = static_cast<DrawingEffect*>(clientDrawingEffect);
        ID2D1Brush* brush = GetCachedBrush(effect);
        if (brush == NULL)
            return E_FAIL;

        target_->DrawGlyphRun(
            D2D1::Point2(baselineOriginX, baselineOriginY),
            glyphRun,
            brush,
            measuringMode
        );
#endif
        return S_OK;
    }

    IFACEMETHOD(DrawUnderline)(
        void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        const DWRITE_UNDERLINE* underline,
        IUnknown* clientDrawingEffect
        )
    {
        clientDrawingEffect = GetDrawingEffect(clientDrawingContext, clientDrawingEffect);
        
        DrawingEffect* effect = static_cast<DrawingEffect*>(clientDrawingEffect);
        ID2D1Brush* brush = GetCachedBrush(effect);
        if (brush == NULL)
            return E_FAIL;

        // We will always get a strikethrough as a LTR rectangle
        // with the baseline origin snapped.
        D2D1_RECT_F rectangle =
        {
            baselineOriginX,
            baselineOriginY + underline->offset,
            baselineOriginX + underline->width,
            baselineOriginY + underline->offset + underline->thickness
       };
        // Draw this as a rectangle, rather than a line.
        target_->FillRectangle(&rectangle, brush); 
        return S_OK;
    }

    IFACEMETHOD(DrawStrikethrough)(
        void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        const DWRITE_STRIKETHROUGH* strikethrough,
        IUnknown* clientDrawingEffect
        );

    IFACEMETHOD(DrawInlineObject)(
        void* clientDrawingContext,
        FLOAT originX,
        FLOAT originY,
        IDWriteInlineObject* inlineObject,
        BOOL isSideways,
        BOOL isRightToLeft,
        IUnknown* clientDrawingEffect
        );

    IFACEMETHOD(IsPixelSnappingDisabled)(
        void* clientDrawingContext,
        OUT BOOL* isDisabled
        );

    IFACEMETHOD(GetCurrentTransform)(
        void* clientDrawingContext,
        OUT DWRITE_MATRIX* transform
        );

    IFACEMETHOD(GetPixelsPerDip)(
        void* clientDrawingContext,
        OUT FLOAT* pixelsPerDip
        );

public:
    // For cached images, to avoid needing to recreate the textures each draw call.
    struct ImageCacheEntry
    {
        ImageCacheEntry(IWICBitmapSource* initialOriginal, ID2D1Bitmap* initialConverted)
        :   original(SafeAcquire(initialOriginal)),
            converted(SafeAcquire(initialConverted))
        { }

        ImageCacheEntry(const ImageCacheEntry& b)
        {
            original  = SafeAcquire(b.original);
            converted = SafeAcquire(b.converted);
        }

        ImageCacheEntry& operator=(const ImageCacheEntry& b)
        {
            if (this != &b)
            {
                // Define assignment operator in terms of destructor and
                // placement new constructor, paying heed to self assignment.
                this->~ImageCacheEntry();
                new(this) ImageCacheEntry(b);
            }
            return *this;
        }

        ~ImageCacheEntry()
        {
            SafeRelease(&original);
            SafeRelease(&converted);
        }

        IWICBitmapSource* original;
        ID2D1Bitmap* converted;
    };

protected:
    HRESULT CreateTarget()
    {
         HRESULT hr = S_OK;
         
         return hr;
    }
    ID2D1Bitmap* GetCachedImage(IWICBitmapSource* image)
    {
        return nullptr;
    }
    
    ID2D1Brush*  GetCachedBrush(const DrawingEffect* effect)
    {
        if (effect == NULL || brush_ == NULL)
            return NULL;

        // Update the D2D brush to the new effect color.
        UINT32 bgra = effect->GetColor();
        float alpha = (bgra >> 24) / 255.0f;
        brush_->SetColor(D2D1::ColorF(bgra, alpha));

        return brush_;
    }

protected:
    IDWriteFactory* dwriteFactory_;
    ID2D1Factory* d2dFactory_;
    ID2D1HwndRenderTarget* target_;     // D2D render target
    ID2D1SolidColorBrush* brush_;       // reusable scratch brush for current color

    std::vector<ImageCacheEntry> imageCache_;

    HWND hwnd_;
    HMONITOR hmonitor_;

};


class TextEditor
{
public:
    enum SetSelectionMode
    {
        SetSelectionModeLeft,               // cluster left
        SetSelectionModeRight,              // cluster right
        SetSelectionModeUp,                 // line up
        SetSelectionModeDown,               // line down
        SetSelectionModeLeftChar,           // single character left (backspace uses it)
        SetSelectionModeRightChar,          // single character right
        SetSelectionModeLeftWord,           // single word left
        SetSelectionModeRightWord,          // single word right
        SetSelectionModeHome,               // front of line
        SetSelectionModeEnd,                // back of line
        SetSelectionModeFirst,              // very first position
        SetSelectionModeLast,               // very last position
        SetSelectionModeAbsoluteLeading,    // explicit position (for mouse click)
        SetSelectionModeAbsoluteTrailing,   // explicit position, trailing edge
        SetSelectionModeAll                 // select all text
    };

    TextEditor(IDWriteFactory* factory)
    {
        
    }
    
     ~TextEditor()
     {
         
     }

protected:
    //RenderTarget*               renderTarget_;
    DrawingEffect*              pageBackgroundEffect_;
    DrawingEffect*              textSelectionEffect_;
    DrawingEffect*              imageSelectionEffect_;
    DrawingEffect*              caretBackgroundEffect_;
    IDWriteTextLayout*          textLayout_;
    //EditableLayout              layoutEditor_;

    std::wstring text_;

    ////////////////////
    // Selection/Caret navigation
    ///
    // caretAnchor equals caretPosition when there is no selection.
    // Otherwise, the anchor holds the point where shift was held
    // or left drag started.
    //
    // The offset is used as a sort of trailing edge offset from
    // the caret position. For example, placing the caret on the
    // trailing side of a surrogate pair at the beginning of the
    // text would place the position at zero and offset of two.
    // So to get the absolute leading position, sum the two.
    UINT32 caretAnchor_;
    UINT32 caretPosition_;
    UINT32 caretPositionOffset_;    // > 0 used for trailing edge

};

CAtlWinModule _Module;

class XWindow : public ATL::CWindowImpl<XWindow>
{
public:
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP(XWindow)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_DISPLAYCHANGE, OnPaint)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()
	
	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL bHandled)
	{
		PostQuitMessage(0);
		return 0;
	}
    
	LRESULT OnPaint(UINT, WPARAM, LPARAM, BOOL bHandled)
	{
		PAINTSTRUCT ps;
        BeginPaint(&ps);
        
        
        EndPaint(&ps);
		return 0;
	}
    
};

int WINAPI  _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nShowCmd*/)
{
	HRESULT hr;
    MSG msg = { 0 };
	XWindow xw;
	
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if(S_OK != hr) return 0;

	xw.Create(NULL, CWindow::rcDefault, _T("Tiny Text Editor"), WS_OVERLAPPEDWINDOW|WS_VISIBLE);
	if(xw.IsWindow()) xw.ShowWindow(SW_SHOW); 
	else goto ExitThisApplication;
	
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

ExitThisApplication:
	CoUninitialize();
    return 0;
}
