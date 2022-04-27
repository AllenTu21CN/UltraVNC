#pragma once

namespace base {
namespace gfx {

class Point
{
public:
    Point() : xp(0), yp(0) {}
    Point(int xpos, int ypos) : xp(xpos), yp(ypos) {}

    inline int x() const { return xp; }
    inline int y() const { return yp; }
    inline void setX(int xpos) { xp = xpos; }
    inline void setY(int ypos) { yp = ypos; }
    inline void set(int xpos, int ypos) {
        xp = xpos; yp = ypos;
    }

    inline void move(int xoff, int yoff){
        xp += xoff; yp += xoff;
    }

    friend inline bool operator==(const Point &, const Point &);
    friend inline bool operator!=(const Point &, const Point &);

private:
    int xp;
    int yp;
};

inline bool operator==(const Point &p1, const Point &p2)
{ return p1.xp == p2.xp && p1.yp == p2.yp; }

inline bool operator!=(const Point &p1, const Point &p2)
{ return p1.xp != p2.xp || p1.yp != p2.yp; }

class PointF
{
public:
    PointF() : xp(0), yp(0) {}
    PointF(double xpos, double ypos) : xp(xpos), yp(ypos) {}

    inline double x() const { return xp; }
    inline double y() const { return yp; }
    inline void setX(double xpos) { xp = xpos; }
    inline void setY(double ypos) { yp = ypos; }

    friend inline bool operator==(const PointF &, const PointF &);
    friend inline bool operator!=(const PointF &, const PointF &);

private:
    double xp;
    double yp;
};

inline bool operator==(const PointF &p1, const PointF &p2)
{ return p1.xp == p2.xp && p1.yp == p2.yp; }

inline bool operator!=(const PointF &p1, const PointF &p2)
{ return p1.xp != p2.xp || p1.yp != p2.yp; }

} // End of namespace gfx
} // End of namespace base
