#ifndef IRENDERER_H
#define IRENDERER_H
namespace odb {



    class CRenderer {
    private:
        CControlCallback mOnKeyPressedCallback;
        CControlCallback mOnKeyReleasedCallback;
     public:
        CRenderer( CControlCallback keyPressedCallback, CControlCallback keyReleasedCallback );
        void render( const CGame& game, long ms );
        void handleSystemEvents();
        void sleep( long ms );

        float castRay(const CGame &angle, short offset);
    };
}
#endif
