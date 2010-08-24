#ifndef COLORGENERATOR_H
#define COLORGENERATOR_H

#include <QColor>

class ColorGenerator {
  public:
    virtual QColor nextColor() = 0;
};

#endif /* COLORGENERATOR_H */
