#pragma once

#include "base/global.h"
#include "base/gfx/point.h"
#include <math.h>

namespace base {
namespace gfx {

class Rect
{
public:
    Rect() : x1(0), y1(0), x2(-1), y2(-1) {}
    Rect(int left, int top, int width, int height)
        : x1(left), y1(top), x2(left + width - 1), y2(top + height - 1) {}
    
    inline int left() const { return x1; }
    inline int top() const { return y1; }
    inline int right() const { return x2; }
    inline int bottom() const { return y2; }

    inline int x() const { return x1; }
    inline int y() const { return y1; }
    inline void setLeft(int pos) { x1 = pos; }
    inline void setTop(int pos) { y1 = pos; }
    inline void setRight(int pos) { x2 = pos; }
    inline void setBottom(int pos) { y2 = pos; }
    inline void setX(int x) { x1 = x; }
    inline void setY(int y) { y1 = y; }
    inline void setLTRB(int left, int top, int right, int bottom) {
      x1 = left; x2 = right;
      y1 = top;  y2 = bottom;
    }

    inline Point topLeft() const { return Point(x1, y1); }
    inline Point bottomRight() const { return Point(x2, y2); }
    inline Point topRight() const { return Point(x2, y1); }
    inline Point bottomLeft() const { return Point(x1, y2); }
    inline Point center() const
        { return Point(
            int((int64_t(x1) + x2) / 2), int((int64_t(y1) + y2) / 2)); }

    inline int width() const { return  x2 - x1 + 1; }
    inline int height() const { return  y2 - y1 + 1; }
    inline void setWidth(int w) { x2 = (x1 + w - 1); }
    inline void setHeight(int h) { y2 = (y1 + h - 1); }

    inline bool isValid() const{
        return (x2 > x1 && y2 > y1);
    }
    inline bool contain(const Rect &rect){
        return (rect.left() >= left() && rect.right() <= right() &&
               rect.top() >= top() && rect.bottom() <= bottom());
    }
    inline bool contain(const Point &point){
        return point.x() >= left() && point.x() <= right() &&
               point.y() >= top() && point.y() <= bottom();
    }
    inline void intersect(const Rect& rect){
        x1 = std::max<int>(left(), rect.left());
        y1 = std::max<int>(top(), rect.top());
        x2 = std::min<int>(right(), rect.right());
        y2 = std::min<int>(bottom(), rect.bottom());
        if (!isValid()){
            x1 = 0; y1 = 0;
            x2 = -1; y2 = -1;
        }
    }
    inline void move(int dx, int dy){
        x1 += dx; x2 += dx;
        y1 += dy; y2 += dy;
    }
    inline void move(const Point &point) { move(point.x(), point.y()); }

    inline Rect &operator=(const Rect &other) {
        x1 = other.x1;
        y1 = other.y1;
        x2 = other.x2;
        y2 = other.y2;
        return *this;
    }

    friend inline bool operator==(const Rect &, const Rect &);
    friend inline bool operator!=(const Rect &, const Rect &);

private:
    int x1;
    int y1;
    int x2;
    int y2;
};

inline bool operator==(const Rect &r1, const Rect &r2)
{
    return r1.x1 == r2.x1 && r1.x2 == r2.x2 && r1.y1 == r2.y1 && r1.y2 == r2.y2;
}

inline bool operator!=(const Rect &r1, const Rect &r2)
{
    return r1.x1 != r2.x1 || r1.x2 != r2.x2 || r1.y1 != r2.y1 || r1.y2 != r2.y2;
}

class RectF
{
public:
    RectF() : xp(0.), yp(0.), w(0.), h(0.) {}
    RectF(double left, double top, double width, double height)
        : xp(left), yp(top), w(width), h(height) {}
    RectF(const RectF &other) : xp(other.xp), yp(other.yp), w(other.w), h(other.h) {}
    
    inline double left() const { return xp; }
    inline double top() const { return yp; }
    inline double right() const { return xp + w; }
    inline double bottom() const { return yp + h; }

    inline double x() const { return xp; }
    inline double y() const { return yp; }
    inline void setLeft(double pos)
        { double diff = pos - xp; xp += diff; w -= diff; }
    inline void setTop(double pos)
        { double diff = pos - yp; yp += diff; h -= diff; }
    inline void setRight(double pos) { w = pos - xp; }
    inline void setBottom(double pos) { h = pos - yp; }
    inline void setX(double x) { xp = x; }
    inline void setY(double y) { yp = y; }

    inline PointF topLeft() const { return PointF(xp, yp); }
    inline PointF bottomRight() const { return PointF(xp + w, yp + h); }
    inline PointF topRight() const { return PointF(xp + w, yp); }
    inline PointF bottomLeft() const { return PointF(xp, yp + h); }
    inline PointF center() const { return PointF(xp + w / 2, yp + h / 2); }

    inline double width() const { return  w; }
    inline double height() const { return  h; }
    inline void setWidth(double aw) { w = aw; }
    inline void setHeight(double ah) { h = ah; }

    inline RectF &operator=(const RectF &other) {
        xp = other.xp;
        yp = other.yp;
        w = other.w;
        h = other.h;
        return *this;
    }

    friend inline bool operator==(const RectF &, const RectF &);
    friend inline bool operator!=(const RectF &, const RectF &);

    inline Rect realRect(int width, int height) {
        Rect rect;
        rect.setLeft((int)floor(xp * (float)width));
        rect.setTop((int)floor(yp * (float)height));
        rect.setRight((int)ceil((xp + w) * (float)width) - 1);
        rect.setBottom((int)ceil((yp + h) * (float)height) - 1);
        return rect;
    }

private:
    double xp;
    double yp;
    double w;
    double h;
};

inline bool operator==(const RectF &r1, const RectF &r2)
{
    return fuzzyCompare(r1.xp, r2.xp) && fuzzyCompare(r1.yp, r2.yp)
           && fuzzyCompare(r1.w, r2.w) && fuzzyCompare(r1.h, r2.h);
}

inline bool operator!=(const RectF &r1, const RectF &r2)
{
    return !fuzzyCompare(r1.xp, r2.xp) || !fuzzyCompare(r1.yp, r2.yp)
           || !fuzzyCompare(r1.w, r2.w) || !fuzzyCompare(r1.h, r2.h);
}

} // End of namespace gfx
} // End of namespace base
