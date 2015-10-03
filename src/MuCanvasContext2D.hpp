//
//  MuCanvasContext2D.hpp
//  ciEjecta
//
//  Created by Sean on 10/1/15.
//
//

#pragma once

#include "MuCanvas2DTypes.hpp"
#include "MuPath.hpp"

#define MU_CANVAS_STATE_STACK_SIZE 16

namespace mural {

  static const size_t MU_MAX_VERTICES = 2048;

  using namespace ci;
  using namespace ci::app;

  enum MuLineCap {
    kMuLineCapButt,
    kMuLineCapRound,
    kMuLineCapSquare
  };

  enum MuLineJoin {
    kMuLineJoinMiter,
    kMuLineJoinBevel,
    kMuLineJoinRound
  };

  enum MuTextBaseline {
    kMuTextBaselineAlphabetic,
    kMuTextBaselineMiddle,
    kMuTextBaselineTop,
    kMuTextBaselineHanging,
    kMuTextBaselineBottom,
    kMuTextBaselineIdeographic
  };

  enum MuTextAlign {
    kMuTextAlignStart,
    kMuTextAlignEnd,
    kMuTextAlignLeft,
    kMuTextAlignCenter,
    kMuTextAlignRight
  };

  enum MuCompositeOperation {
    kMuCompositeOperationSourceOver,
    kMuCompositeOperationLighter,
    kMuCompositeOperationDarker,
    kMuCompositeOperationDestinationOut,
    kMuCompositeOperationDestinationOver,
    kMuCompositeOperationSourceAtop,
    kMuCompositeOperationXOR,
    kMuCompositeOperationCopy,
    kMuCompositeOperationSourceIn,
    kMuCompositeOperationDestinationIn,
    kMuCompositeOperationSourceOut,
    kMuCompositeOperationDestinationAtop
  };

  struct MuCompositeOperationFunc {
    GLenum source;
    GLenum destination;
    float alphaFactor;
  };
  extern const MuCompositeOperationFunc MuCompositeOperationFuncs[];

  struct MuFillable {
    virtual ~MuFillable() {}
  };

  struct MuCanvasState {
    MuAffineTransform transform;

    MuCompositeOperation globalCompositeOperation = kMuCompositeOperationSourceOver;
    ColorAf fillColor = ColorAf::white();
    std::shared_ptr<MuFillable> fillObject = nullptr;
    ColorAf strokeColor = ColorAf::white();
    std::shared_ptr<MuFillable> *strokeObject = nullptr;
    float globalAlpha = 1.0f;

    float lineWidth = 1.0f;
    MuLineCap lineCap = kMuLineCapButt;
    MuLineJoin lineJoin = kMuLineJoinBevel;
    float miterLimit = 10.0f;

    MuTextAlign textAlign = kMuTextAlignStart;
    MuTextBaseline textBaseline = kMuTextBaselineAlphabetic;
    // MuFontDescriptor *font;

    std::shared_ptr<MuPath> clipPath = nullptr;
  };

  static inline ColorAf blendColor(MuCanvasState *state, const ColorAf &color) {
    float alpha = state->globalAlpha * color.a;
    ColorAf blendColor;
    blendColor.r = color.r * alpha;
    blendColor.g = color.g * alpha;
    blendColor.b = color.b * alpha;
    blendColor.a = MuCompositeOperationFuncs[state->globalCompositeOperation].alphaFactor * color.a * state->globalAlpha;
    return blendColor;
  }

  static inline ColorAf blendWhiteColor(MuCanvasState *state) {
    return blendColor(state, ColorAf::white());
  }

  static inline ColorAf blendFillColor(MuCanvasState *state) {
    return blendColor(state, state->fillColor);
  }

  static inline ColorAf blendStrokeColor(MuCanvasState *state) {
    return blendColor(state, state->strokeColor);
  }

  class MuCanvasContext2D {
    public:
      MuCanvasState *state;
      MuCompositeOperation globalCompositeOperation = kMuCompositeOperationSourceOver;

      virtual const gl::TextureRef getTexture();

      void setFillColor(const ColorAf &color);
      void setStrokeColor(const ColorAf &color);

      MuCanvasContext2D(int width, int height);
      virtual ~MuCanvasContext2D() {}

      virtual void create();
      virtual void resizeTo(int width, int height);
      void resetFramebuffer();
      void bindVertexBuffer();

      void save();
      void restore();
      void rotate(float angle);
      void translate(float x, float y);
      void scale(float x, float y);
      void transform(float m11, float m12, float m21, float m2, float dx, float dy);
      void setTransform(float m11, float m12, float m21, float m2, float dx, float dy);
      void drawImage(gl::TextureRef image, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh) {}
      void fillRect(float x, float y, float w, float h);
      void strokeRect(float x, float y, float w, float h);
      void clearRect(float x, float y, float w, float h);
      void beginPath();
      void closePath();
      void fill();
      void stroke();
      void moveTo(float x, float y);
      void lineTo(float x, float y);
      void rect(float x, float y, float w, float h);
      void bezierCurveTo(float cpx1, float cpy1, float cpx2, float cpy2, float x, float y) {}
      void quadraticCurveTo(float cpx, float cpy, float x, float y) {}
      void arcTo(float x1, float y1, float x2, float y2, float radius) {}
      void arc(float x, float y, float radius, float startAngle, float endAngle, bool antiClockwise) {}

      void prepare();
      void flushBuffers();

      void pushTri(float x1, float y1, float x2, float y2, float x3, float y3, const ColorAf &color, const MuAffineTransform &transform);
      void pushQuad(vec2 v1, vec2 v2, vec2 v3, vec2 v4, const ColorAf &color, const MuAffineTransform &transform);
      void pushRect(float x, float y, float w, float h, const ColorAf &color, const MuAffineTransform &transform);
      void pushFilledRect(float x, float y, float w, float h, MuFillable *fillable, const ColorAf &color, MuAffineTransform transform) {}
      // void pushGradientRect(float x, float y, float w, float h, MuCanvasGradient *gradient, const ColorAf &color, MuAffineTransform transform);
      // void pushPatternedRect(float x, float y, float w, float h, MuCanvasPattern *pattern, const ColorAf &color, MuAffineTransform transform);
      void pushTexturedRect(
        float x, float y, float w, float h,
        float tx, float ty, float tw, float th,
        const ColorAf &color,
        const MuAffineTransform &transform
      );

      virtual void present() {}

    protected:
      bool needsPresenting = false;

      ivec2 size = { 200, 150 };
      float devicePixelRatio = 1.0f;

      ivec2 bufferSize = { 200, 150 };
      gl::FboRef viewFramebuffer;

      int stateIndex = 0;
      MuCanvasState stateStack[MU_CANVAS_STATE_STACK_SIZE];

      std::shared_ptr<MuPath> path = std::make_shared<MuPath>();

      // Batch for drawing pathes
      gl::BatchRef    batch;
      gl::VboMeshRef  mesh;

      size_t vertexBufferIndex = 0;

      gl::VboRef positionVbo;
      gl::VboRef texCoordVbo;
      gl::VboRef colorVbo;

      vec2 *positions;
      vec2 *texCoords;
      vec4 *colors;

      void setProgram(const gl::GlslProgRef &glsl);
  };

}
