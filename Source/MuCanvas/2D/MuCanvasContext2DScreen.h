#ifndef Mural_MuCanvasContext2DScreen_h
#define Mural_MuCanvasContext2DScreen_h

#include "MuCanvasContext2D.h"
#include "MuCanvas2DTypes.h"

namespace mural {

  class MuCanvasContext2DScreen : public MuCanvasContext2D {
    public:
      void setStyle(MuRect newStyle);
      MuRect getFrame();

      MuCanvasContext2DScreen(int width, int height);
      ~MuCanvasContext2DScreen();

      void resizeTo(int newWidth, int newHeight);
      void create();
      void present();

    private:
      MuRect style;

      GLuint vao = 0, vbo = 0;
      GLuint textureId = 0;
  };

}

#endif
