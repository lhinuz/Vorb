#include "stdafx.h"
#include "macros.h"

#undef UNIT_TEST_BATCH
#define UNIT_TEST_BATCH Vorb_UI_

#include <include/Vorb.h>
#include <include/ui/MainGame.h>
#include <include/ui/ScreenList.h>
#include <include/ui/IGameScreen.h>
#include <include/ui/InputDispatcher.h>
#include <include/Timing.h>
#include <include/Graphics.h>
#include <glm/gtx/transform.hpp>

struct Vertex {
    f32v3 position;
    color4 color;
};

class TestScreen : public vui::IGameScreen {
public:
    virtual i32 getNextScreen() const {
        return SCREEN_INDEX_NO_SCREEN;
    }
    virtual i32 getPreviousScreen() const {
        return SCREEN_INDEX_NO_SCREEN;
    }
    virtual void build() {
    }
    virtual void destroy(const vui::GameTime& gameTime) {
    }
    virtual void onEntry(const vui::GameTime& gameTime) {
    }
    virtual void onExit(const vui::GameTime& gameTime) {
    }
    virtual void update(const vui::GameTime& gameTime) {
    }
    virtual void draw(const vui::GameTime& gameTime) {
    }
};
class App : public vui::MainGame {
public:
    App(i32 index) :
    m_index(index) {
        // Empty
    }

    virtual void onInit() {
        // Empty
    }
    virtual void addScreens() {
        m_screenList.addScreen(new TestScreen);
        m_screenList.setScreen(m_index);
    }
    virtual void onExit() {
        // Empty
    }
private:
    i32 m_index;
};

TEST(MainGame) {
    vorb::init(vorb::InitParam::ALL);
    { App(0).run(); }
    vorb::dispose(vorb::InitParam::ALL);
    return true;
}

TEST(InputFuncs) {
    vorb::init(vorb::InitParam::ALL);

    AutoDelegatePool pool;
    pool.addAutoHook(vui::InputDispatcher::key.onFocusGained, [] (Sender) {
        puts("Key focus gained");
    });
    pool.addAutoHook(vui::InputDispatcher::key.onFocusLost, [] (Sender) {
        puts("Key focus lost");
    });
    pool.addAutoHook(vui::InputDispatcher::key.onKeyDown, [] (Sender, const vui::KeyEvent& e) {
        printf("Key: %d-%d was pressed\n", e.keyCode, e.scanCode);
    });
    pool.addAutoHook(vui::InputDispatcher::key.onKeyUp, [] (Sender, const vui::KeyEvent& e) {
        printf("Key: %d-%d was released\n", e.keyCode, e.scanCode);
    });
    pool.addAutoHook(vui::InputDispatcher::key.onText, [] (Sender, const vui::TextEvent& e) {
        char c[2] = { e.text[0], 0 };
        printf("Text: %s\n", c);
    });
    pool.addAutoHook(vui::InputDispatcher::mouse.onButtonDown, [] (Sender, const vui::MouseButtonEvent& e) {
        printf("Mouse: %d was pressed at (%d,%d)\n", e.button, e.x, e.y);
    });
    pool.addAutoHook(vui::InputDispatcher::mouse.onButtonUp, [] (Sender, const vui::MouseButtonEvent& e) {
        printf("Mouse: %d was released at (%d,%d)\n", e.button, e.x, e.y);
    });
    pool.addAutoHook(vui::InputDispatcher::mouse.onFocusGained, [] (Sender, const vui::MouseEvent& e) {
        printf("Mouse: gained at (%d,%d)\n", e.x, e.y);
    });
    pool.addAutoHook(vui::InputDispatcher::mouse.onFocusLost, [] (Sender, const vui::MouseEvent& e) {
        printf("Mouse: lost at (%d,%d)\n", e.x, e.y);
    });
    pool.addAutoHook(vui::InputDispatcher::mouse.onMotion, [] (Sender, const vui::MouseMotionEvent& e) {
        printf("Mouse: motion (%d,%d) at (%d,%d)\n", e.dx, e.dy, e.x, e.y);
    });
    pool.addAutoHook(vui::InputDispatcher::mouse.onWheel, [] (Sender, const vui::MouseWheelEvent& e) {
        printf("Mouse: scroll (%d,%d) at (%d,%d)\n", e.dx, e.dy, e.x, e.y);
    });
    pool.addAutoHook(vui::InputDispatcher::window.onFile, [] (Sender, const vui::WindowFileEvent& e) {
        printf("File: %s\n", e.file);
    });
    pool.addAutoHook(vui::InputDispatcher::window.onResize, [] (Sender, const vui::WindowResizeEvent& e) {
        printf("Resize: (%d,%d)\n", e.w, e.h);
    });


    { App(0).run(); }

    pool.dispose();

    vorb::dispose(vorb::InitParam::ALL);
    return true;
}

TEST(SoloWindow) {
    vorb::init(vorb::InitParam::GRAPHICS);

    vui::GameWindow window;
    window.init();

#if defined(VORB_IMPL_GRAPHICS_OPENGL)
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
#elif defined(VORB_IMPL_GRAPHICS_D3D)
    auto dev = (IDirect3DDevice9*)window.getContext();
    f32m4 mat = glm::perspectiveFov(90.0f, 800.0f, 600.0f, 0.01f, 100.0f);
    dev->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&mat[0][0]);
    dev->SetRenderState(D3DRS_AMBIENT, RGB(255, 255, 255));
    dev->SetRenderState(D3DRS_LIGHTING, false);
    dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    dev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    dev->SetFVF((D3DFVF_XYZ | D3DFVF_DIFFUSE));
    IDirect3DVertexBuffer9* tri_buffer = nullptr;
    void* pData;
    Vertex aTriangle[3] = {
        { f32v3(-1.0f, -1.0f, -2.0f), color4(0x00, 0xff, 0x00, 0xff) },
        { f32v3(0.0f, 1.0f, -3.0f), color4(0x00, 0x00, 0xff, 0xff) },
        { f32v3(1.0f, 0.0f, -3.0f), color4(0xff, 0x00, 0x00, 0xff) }
    };

    dev->CreateVertexBuffer(sizeof(aTriangle), D3DUSAGE_WRITEONLY,
        (D3DFVF_XYZ | D3DFVF_DIFFUSE),
        D3DPOOL_MANAGED, &tri_buffer, NULL);
    tri_buffer->Lock(0, sizeof(pData), (void**)&pData, 0);
    memcpy(pData, aTriangle, sizeof(aTriangle));
    tri_buffer->Unlock();
#endif

    PreciseTimer timer;
    timer.start();
    while (!window.shouldQuit()) {
        // Do something here
#if defined(VORB_IMPL_GRAPHICS_OPENGL)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#elif defined(VORB_IMPL_GRAPHICS_D3D)
        dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

        dev->BeginScene();
        dev->SetStreamSource(0, tri_buffer, 0, sizeof(Vertex));
        dev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
        dev->EndScene();
#endif

        // Synchronize frame-step
        ui32 ms = (ui32)timer.stop();
        timer.start();

        // Update the window
        window.sync(ms);
    }
    window.dispose();

    vorb::dispose(vorb::InitParam::GRAPHICS);
    return true;
}
